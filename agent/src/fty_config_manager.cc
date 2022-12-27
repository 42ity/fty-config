/*  =========================================================================
    fty_config_manager - Fty config manager

    Copyright (C) 2014 - 2020 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    =========================================================================
 */

/*
@header
    fty_config_manager - Fty config manager
@discuss
@end
 */

#include "fty_config_manager.h"
#include "fty-config.h"
#include "fty_config_exception.h"
#include "file.h"
#include "ntp_service.h"
#include <augeas.h>
#include <fty_common.h>
#include <iostream>
#include <list>
#include <memory>
#include <regex>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std::placeholders;
using namespace JSON;
using namespace dto::srr;

namespace config {

//__>> HOTFIX Network config is not a proper JSON due to several "iface" attribute name in the Json
// Solution: index the iface (rename ifacename) for the output and remove the index when restore
// Functions
static std::string createIndexForIface(const std::string& json);
static std::string removeIndexForIface(const std::string& member);
//__<< HOTFIX

//__>> HOTFIX Network config is not a proper JSON due to several same attribute name in the Json
static std::string createIndexForOthers(const std::string& json);
static std::string removeIndexForOthers(const std::string& member);
//__<< HOTFIX

//__>> HOTFIX Network config is not a proper JSON due to several "string" attribute name in the Json for array
static std::string createIndexForArray(const std::string& json);
static std::string updateIndexForArray(const std::string& member);
//__<< HOTFIX

#define FILE_SEPARATOR     "/"
#define AUGEAS_FILES       FILE_SEPARATOR "files"
#define ANY_NODES          FILE_SEPARATOR "*"
#define COMMENTS_DELIMITER "#"

#define DATA_VERSION_2_0 "2.0" // data 2.0 (IPM 2.6)
#define DATA_2_base64_encoded "base64_encoded" // data 2.x
#define DATA_2_enable "enable" // data 2.x

const static std::regex augeasArrayregex("(\\w+\\[.*\\])$", std::regex::optimize);

ConfigurationManager::ConfigurationManager(const std::map<std::string, std::string>& parameters)
    : m_parameters(parameters)
    , m_aug(nullptr, aug_close)
{
    init();
}

void ConfigurationManager::init()
{
    try {
        logDebug("initialization");

        // Augeas tool init
        int augeasOpt = getAugeasFlags(m_parameters.at(AUGEAS_OPTIONS));
        log_debug("augeas options: %d", augeasOpt);

        m_aug = AugeasSmartPtr(
            aug_init(FILE_SEPARATOR, m_parameters.at(AUGEAS_LENS_PATH).c_str(), AUG_NONE /*augeasOpt*/), aug_close);
        if (!m_aug) {
            throw ConfigurationException("Augeas tool initialization failed");
        }

        // Message bus init
        m_msgBus = std::unique_ptr<messagebus::MessageBus>(
            messagebus::MlmMessageBus(m_parameters.at(ENDPOINT_KEY), m_parameters.at(AGENT_NAME_KEY)));
        m_msgBus->connect();

        // Bind all processor handler.
        m_processor.saveHandler    = std::bind(&ConfigurationManager::saveConfiguration, this, _1);
        m_processor.restoreHandler = std::bind(&ConfigurationManager::restoreConfiguration, this, _1);
        m_processor.resetHandler   = std::bind(&ConfigurationManager::resetConfiguration, this, _1);

        // agent format current version
        m_configVersion = m_parameters.at(CONFIG_VERSION_KEY);
        logDebug("configVersion: {}", m_configVersion);

        // Listen all incoming request
        auto fct = std::bind(&ConfigurationManager::handleRequest, this, _1);
        m_msgBus->receive(m_parameters.at(QUEUE_NAME_KEY), fct);
    }
    catch (messagebus::MessageBusException& ex) {
        log_error("Message bus error: %s", ex.what());
    }
    catch (...) {
        log_error("Unexpected error: unknown");
    }
}

void ConfigurationManager::handleRequest(messagebus::Message msg)
{
    try {
        log_debug("handleRequest...");

        // Load augeas for any request (to avoid any cache)
        aug_load(m_aug.get());

        // Get the query
        Query query;
        dto::UserData data = msg.userData();
        data >> query;

        // Process the query
        Response response = m_processor.processQuery(query);

        // Send the response
        dto::UserData dataResponse;
        dataResponse << response;
        sendResponse(msg, dataResponse);

        log_debug("handleRequest succeeded");
   }
    catch (const std::exception& ex) {
        log_error("handleRequest error: %s", ex.what());
    }
}

// assume version X.Y formatted (major.minor)
static bool isVersion_1_x (const std::string& version)
{
    return (version.find("1.") == 0);
}
static bool isVersion_2_x (const std::string& version)
{
    return (version.find("2.") == 0);
}

// data 2.x features
static bool isFeature_Version2(const std::string& featureName)
{
    return (featureName == NETWORK)
        || (featureName == NETWORK_HOST_NAME)
        || (featureName == NETWORK_AGENT_SETTINGS)
        || (featureName == NETWORK_PROXY)
        || (featureName == DISCOVERY_SETTINGS)
        || (featureName == DISCOVERY_AGENT_SETTINGS)
        || (featureName == TIMEZONE_SETTINGS)
        || (featureName == NTP_SETTINGS);
}

SaveResponse ConfigurationManager::saveConfiguration(const SaveQuery& query)
{
    log_debug("Save configuration...");

    std::map<FeatureName, FeatureAndStatus> mapFeaturesData;

    for (const auto& featureName : query.features()) {
        // Get the full configuration file path name from class variable m_parameters
        const std::string fileName(m_parameters.at(featureName));

        logDebug("Save feature {} (file: {})", featureName, fileName);

        // Get the last pattern
        std::size_t found = fileName.find_last_of(FILE_SEPARATOR);
        if (found != std::string::npos) {
            std::string featureVersion{m_configVersion}; // current (default)

            cxxtools::SerializationInfo si;
            bool useAugeas = false; // 'augeas conf.' vs. 'bulk save'
            bool saveSuccess = false;

            if (isFeature_Version2(featureName)) {
                // data 2.0, save file content base64 encoded (bulk save)
                std::string b64;
                int r = fileReadToBase64(fileName, b64);
                if (r != 0) {
                    logError("fileReadToBase64 failed (r: {}, file: {})", r, fileName);
                }
                else {
                    si.addMember(DATA_2_base64_encoded) <<= b64;
                    // ntp settings exception
                    if (featureName == NTP_SETTINGS) {
                        bool state = false;
                        ntpservice::getState(state);
                        si.addMember(DATA_2_enable) <<= state;
                    }
                    featureVersion = DATA_VERSION_2_0; // break retro compat 1.x
                    saveSuccess = true;
                }
            }
            else {
                // data 1.x, get augeas configuration
                std::string fileNameFullPath = AUGEAS_FILES + fileName + ANY_NODES;
                std::string confFileName = fileName.substr(found + 1);
                getConfigurationToJson(si, fileNameFullPath, confFileName);
                saveSuccess = true;
                useAugeas = true;
            }

            logDebug("save {}: version: {}, success: {}", featureName, featureVersion, saveSuccess);
            //logDebug("save {}: {}", featureName, JSON::writeToString(si, true));

            // Persist DTO
            std::string buffer = JSON::writeToString(si, false);
            if (useAugeas) {
                // apply augeas hotfixes
                buffer = createIndexForIface(buffer);
                buffer = createIndexForOthers(buffer);
                buffer = createIndexForArray(buffer);
            }

            Feature feature;
            feature.set_version(featureVersion);
            feature.set_data(buffer);

            FeatureStatus featureStatus;
            if (saveSuccess) {
                featureStatus.set_status(Status::SUCCESS);
            }
            else {
                featureStatus.set_status(Status::FAILED);
                std::string errorMsg =
                    TRANSLATE_ME("Save configuration for: (%s) failed, access right issue!", featureName.c_str());
                featureStatus.set_error(errorMsg);
            }

            FeatureAndStatus fs;
            *(fs.mutable_status())       = featureStatus;
            *(fs.mutable_feature())      = feature;

            mapFeaturesData[featureName] = fs;
        }
    }

    log_debug("Save configuration done");

    return (createSaveResponse(mapFeaturesData, m_configVersion)).save();
}

RestoreResponse ConfigurationManager::restoreConfiguration(const RestoreQuery& query)
{
    logDebug("Restore configuration... (configVersion: {})", m_configVersion);

    RestoreQuery queryAux = query;
    google::protobuf::Map<FeatureName, Feature>& mapFeaturesData = *(queryAux.mutable_map_features_data());

    std::map<FeatureName, FeatureStatus> mapStatus;

    for (const auto& item : mapFeaturesData) {
        const std::string& featureName = item.first;
        const Feature&     feature     = item.second;
        const std::string  fileName(m_parameters.at(featureName));

        logDebug("Restore feature {} (version: {}, file: {})", featureName, feature.version(), fileName);

        FeatureStatus featureStatus;

        // data 1.x (augeas) && data 2.x (bulk restore)
        bool compatible = isVersion_1_x(feature.version()) || isVersion_2_x(feature.version());

        if (compatible) {
            // Get data member
            cxxtools::SerializationInfo siData;
            JSON::readFromString(feature.data(), siData);

            int returnValue = -1; // failed (default)

            if (isVersion_2_x(feature.version())) {
                // data 2.x, bulk restore
                if (siData.findMember(DATA_2_base64_encoded)) {
                    std::string b64;
                    siData.getMember(DATA_2_base64_encoded, b64);
                    int r = fileRestoreFromBase64(fileName, b64);
                    if (r != 0) {
                        logError("fileRestoreFromBase64 failed (r: {}, file: {})", r, fileName);
                    }
                    else {
                        logDebug("fileRestoreFromBase64 succeeded (file: {})", fileName);
                        returnValue = 0; // bulk restore success
                    }
                }
                else {
                    logError("data {}: member '{}' is missing", feature.version(), DATA_2_base64_encoded);
                }

                // ntp settings exception
                if ((returnValue == 0) && (featureName == NTP_SETTINGS)) {
                    if (siData.findMember(DATA_2_enable)) {
                        bool state = false;
                        siData.getMember(DATA_2_enable, state);

                        if (ntpservice::applyState(state) == 0) {
                            logDebug("apply ntp state successfully set to {}", state);
                        }
                        else {
                            logError("apply ntp state failed");
                            returnValue = -1;
                        }
                    }
                    else {
                        logError("ntp '{}' property not found", DATA_2_enable);
                        returnValue = -1;
                    }
                }
            }
            else {
                // data 1.x: restore with augeas
                const std::string configurationFileName = AUGEAS_FILES + fileName;
                returnValue = setConfiguration(siData, configurationFileName);
            }

            if (returnValue == 0) {
                logInfo("Restore {} succeed", featureName);
                featureStatus.set_status(Status::SUCCESS);

                // dbg, dump restored file
                fileDumpToConsole(fileName, "Restored: ");
            }
            else { // restore failed
                logInfo("Restore {} failed, returnValue: {}", featureName, returnValue);

                featureStatus.set_status(Status::FAILED);
                std::string errorMsg =
                    TRANSLATE_ME("Restore configuration for: (%s) failed, access right issue!", featureName.c_str());
                featureStatus.set_error(errorMsg);
                log_error("%s", featureStatus.error().c_str());
            }
        }
        else {
            logError("Restore unavailable due to format compatibility (feature {}, version: {}, configVersion: {})",
                featureName, feature.version(), m_configVersion);

            std::string errorMsg =
                TRANSLATE_ME("Config version (%s) is not compatible with the restore version request: (%s)",
                    m_configVersion.c_str(), feature.version().c_str());
            log_error("%s", errorMsg.c_str());

            featureStatus.set_status(Status::FAILED);
            featureStatus.set_error(errorMsg);
        }

        mapStatus[featureName] = featureStatus;
    }

    log_debug("Restore configuration done");
    return (createRestoreResponse(mapStatus)).restore();
}

ResetResponse ConfigurationManager::resetConfiguration(const dto::srr::ResetQuery& /*query*/)
{
    throw ConfigurationException("Not implemented yet!");
}

void ConfigurationManager::sendResponse(const messagebus::Message& msg, const dto::UserData& userData)
{
    try {
        messagebus::Message resp;
        resp.userData() = userData;
        resp.metaData().emplace(messagebus::Message::SUBJECT, msg.metaData().at(messagebus::Message::SUBJECT));
        resp.metaData().emplace(messagebus::Message::FROM, m_parameters.at(AGENT_NAME_KEY));
        resp.metaData().emplace(messagebus::Message::TO, msg.metaData().find(messagebus::Message::FROM)->second);
        resp.metaData().emplace(messagebus::Message::CORRELATION_ID, msg.metaData().find(messagebus::Message::CORRELATION_ID)->second);

        m_msgBus->sendReply(msg.metaData().find(messagebus::Message::REPLY_TO)->second, resp);
    }
    catch (messagebus::MessageBusException& ex) {
        log_error("Message bus error: %s", ex.what());
    }
    catch (...) {
        log_error("Unexpected error: unknown");
    }
}

int ConfigurationManager::setConfiguration(cxxtools::SerializationInfo& si, const std::string& rootPath)
{
    setConfigurationRecursive(si, rootPath);
    return aug_save(m_aug.get());
}

void ConfigurationManager::setConfigurationRecursive(cxxtools::SerializationInfo& si, const std::string& rootPath, const std::string& path)
{
    cxxtools::SerializationInfo::Iterator it;
    for (it = si.begin(); it != si.end(); ++it) {
        cxxtools::SerializationInfo* member     = &(*it);
        std::string                  memberName = member->name();

        // ignore non augeas members (secure, normally never reached)
        if (memberName == DATA_2_base64_encoded) {
            logWarn("Ignore unexpected augeas json member ({})", memberName);
            continue;
        }

        // unapply augeas hotfixes
        memberName = removeIndexForIface(memberName);
        memberName = removeIndexForOthers(memberName);
        memberName = updateIndexForArray(memberName);

        if (member->category() == cxxtools::SerializationInfo::Category::Object) {
            std::string pathCompute = path;
            if (!pathCompute.empty()) {
               pathCompute += FILE_SEPARATOR;
            }
            pathCompute += memberName;
            setConfigurationRecursive(*member, rootPath, pathCompute);
        }
        else {
            std::string fullPath;
            if (!path.empty()) {
                fullPath = rootPath + FILE_SEPARATOR + path + FILE_SEPARATOR + memberName;
            }
            else {
                fullPath = rootPath + FILE_SEPARATOR + memberName;
            }
            std::string elementValue;
            member->getValue(elementValue);
            // Set value
            persistValue(fullPath, elementValue);
            logDebug("Set fullPath={} elementValue={}", fullPath, elementValue);
        }
    }
}

void ConfigurationManager::persistValue(const std::string& fullPath, const std::string& value)
{
    int setReturn = aug_set(m_aug.get(), fullPath.c_str(), value.c_str());
    log_debug("Set values, %s = %s => %d", fullPath.c_str(), value.c_str(), setReturn);
    if (setReturn == -1) {
        log_error("Error to set the following values, %s = %s", fullPath.c_str(), value.c_str());
    }
}

void ConfigurationManager::getConfigurationToJson(
    cxxtools::SerializationInfo& si, std::string& path, std::string& rootMember)
{
    std::smatch arrayMatch;
    char**      matches = nullptr;
    int         nmatches = aug_match(m_aug.get(), path.c_str(), &matches);

    // no matches, stop it.
    if (nmatches < 0)
        return;

    // Iterate on all matches
    for (int i = 0; i < nmatches; i++) {
        std::string temp = matches[i];
        // Skip all comments
        if (temp.find(COMMENTS_DELIMITER) == std::string::npos) {
            const char *value, *label;
            aug_get(m_aug.get(), matches[i], &value);
            aug_label(m_aug.get(), matches[i], &label);

            if (value) {
                // Find all members to insert
                std::vector<std::string>     members = findMembersFromMatch(temp, rootMember);
                cxxtools::SerializationInfo* siTemp  = &(si);
                for (const auto& elem : members) {
                    cxxtools::SerializationInfo* siTmp = siTemp->findMember(elem);
                    if (!siTmp) {
                        if (elem.compare(members.back()) != 0) {
                            siTmp  = &(siTemp->addMember(elem));
                            siTemp = siTmp;
                        } else {
                            siTemp->addMember(label) <<= value;
                            siTmp  = siTemp->findMember(members.front());
                            siTemp = siTmp;
                        }
                    } else {
                        // Reset pointer
                        siTmp  = siTemp->findMember(elem);
                        siTemp = siTmp;
                    }
                }
            } else if (regex_search(temp, arrayMatch, augeasArrayregex) == true && arrayMatch.str(1).length() > 0) {
                // In an array case, it's member too.
                si.addMember(arrayMatch.str(1));
            }
            getConfigurationToJson(si, temp.append(ANY_NODES), rootMember);
        }
    }
}

std::vector<std::string> ConfigurationManager::findMembersFromMatch(
    const std::string& input, const std::string& rootMember)
{
    std::vector<std::string> members;
    if (input.length() > 0) {
        // Try to find root member
        std::size_t found = input.find(rootMember);
        if (found != std::string::npos) {
            std::string       remain = input.substr(found + rootMember.size(), input.size());
            std::string       tmp;
            std::stringstream ss(remain);
            while (std::getline(ss, tmp, FILE_SEPARATOR[0])) {
                if (tmp.size() > 0) {
                    members.push_back(tmp);
                }
            }
        }
    }
    return members;
}

void ConfigurationManager::dumpConfiguration(std::string& path)
{
    char** matches = nullptr;
    int    nmatches = aug_match(m_aug.get(), path.c_str(), &matches);

    // Stop if not matches.
    if (nmatches < 0)
        return;

    // Iterate on all matches
    for (int i = 0; i < nmatches; i++) {
        std::string temp = matches[i];
        // Skip all comments
        if (temp.find(COMMENTS_DELIMITER) == std::string::npos) {
            const char *value, *label;
            aug_get(m_aug.get(), matches[i], &value);
            aug_label(m_aug.get(), matches[i], &label);
            dumpConfiguration(temp.append(ANY_NODES));
        }
    }
}

int ConfigurationManager::getAugeasFlags(std::string& augeasOpts)
{
    // Build static augeas option
    static std::map<const std::string, aug_flags> augFlags;
    augFlags["AUG_NONE"]                 = AUG_NONE;
    augFlags["AUG_SAVE_BACKUP"]          = AUG_SAVE_BACKUP;
    augFlags["AUG_SAVE_NEWFILE"]         = AUG_SAVE_NEWFILE;
    augFlags["AUG_TYPE_CHECK"]           = AUG_TYPE_CHECK;
    augFlags["AUG_NO_STDINC"]            = AUG_NO_STDINC;
    augFlags["AUG_SAVE_NOOP"]            = AUG_SAVE_NOOP;
    augFlags["AUG_NO_LOAD"]              = AUG_NO_LOAD;
    augFlags["AUG_NO_MODL_AUTOLOAD"]     = AUG_NO_MODL_AUTOLOAD;
    augFlags["AUG_ENABLE_SPAN"]          = AUG_ENABLE_SPAN;
    augFlags["AUG_NO_ERR_CLOSE"]         = AUG_NO_ERR_CLOSE;
    augFlags["AUG_TRACE_MODULE_LOADING"] = AUG_TRACE_MODULE_LOADING;

    int returnValue = AUG_NONE;

    if (augeasOpts.size() > 1) {
        // Replace '|' by ' '
        std::replace(augeasOpts.begin(), augeasOpts.end(), '|', ' ');

        // Build all options parameters in array
        std::vector<std::string> augOptsArray;
        std::stringstream        ss(augeasOpts);
        std::string              temp;
        while (ss >> temp) {
            augOptsArray.push_back(temp);
        }

        // Builds augeas options
        std::vector<std::string>::iterator it;
        for (it = augOptsArray.begin(); it != augOptsArray.end(); ++it) {
            returnValue |= augFlags.at(*it);
        }
    }
    return returnValue;
}

//__>> HOTFIX Network config is not a proper JSON due to several "iface" attribute name in the Json
static std::string createIndexForIface(const std::string& json)
{
    std::regex  regex("\"iface\"");
    std::smatch submatch;
    std::string output;
    std::string jsonProcessing = json;
    int index = 0;

    // Search all interface
    while (std::regex_search(jsonProcessing, submatch, regex)) {
        // Build resulting string
        output += std::string(submatch.prefix()) + "\"ifacename[" + std::to_string(index++) + "]\"";
        // Continue to search with the rest of the string
        jsonProcessing = submatch.suffix();
    }
    // If there is still a suffix, add it
    output += jsonProcessing;

    return output;
}

static std::string removeIndexForIface(const std::string& member)
{
    std::regex regex("^ifacename\\[(\\d+)\\]$");
    std::smatch submatch;
    // replace "ifacename[index]" by "iface[index+1]"
    if (std::regex_search(member, submatch, regex)) {
        std::stringstream buffer;
        int index = std::stoi(submatch[1]) + 1;
        buffer << "iface[" << std::to_string(index) << "]";
        return buffer.str();
    }
    return member;
}
//__<< HOTFIX

//__>> HOTFIX Network config is not a proper JSON due to several same attributes name
// (e.g. "entry" or "dns-nameserver") in the Json
// Need to indexed these attributes to have unique key for the json parser
// eg: Replace "entry" by "entry.#index"
//     Replace "dns-nameserver" by "dns-nameserver.#index"
static std::string createIndexForOthers(const std::string& json)
{
    std::regex  regex("\"(entry|dns-nameserver)\"");
    std::smatch submatch;
    std::string jsonProcessing = json;
    std::stringstream buffer;
    int index = 0;

    // Search all interface
    while (std::regex_search(jsonProcessing, submatch, regex)) {
        // Build resulting string
        buffer << std::string(submatch.prefix()) << "\"" << submatch[1] << ".#" << std::to_string(index++) << "\"";
        // Continue to search with the rest of the string
        jsonProcessing = submatch.suffix();
    }
    // If there is still a suffix, add it
    buffer << jsonProcessing;

    return buffer.str();
}

// remove previously added index for key which is not needed for set value in augeas
// e.g: "entry.#index" -> "entry"
//      "dns-nameserver.#index" -> "dns-nameserver"
static std::string removeIndexForOthers(const std::string& member)
{
    std::regex  regex("^(entry|dns-nameserver)\\.\\#\\d+$");
    std::smatch submatch;
    std::string output = member;

    std::stringstream buffer;
    if (std::regex_search(member, submatch, regex)) {
        // Replace by "entry" or "dns-nameserver"
        output = submatch[1];
    }
    return output;
}
//__<< HOTFIX

//__>> HOTFIX Network config is not a proper JSON due to several "string" attribute in the Json for array
// ex: { "my_array": [ "127.0.0.1", "127.0.0.2", "127.0.0.3"]} will produce with augeas:
//     { "my_array": { "array": { "string":"127.0.0.1", "string":"127.0.0.2", "string":"127.0.0.3" }}}
// Need to indexed the "string" to have unique key for the json parser:
//     { "my_array": { "array":{ "string.#1":"127.0.0.1", "string.#2":"127.0.0.2", "string.#3":"127.0.0.3" }}}
static std::string createIndexForArray(const std::string& json)
{
    std::regex  regex("\"array\"\\:\\{\"string\"");
    std::regex  regex2("\"string\"\\:");
    std::smatch submatch;
    std::string output;
    std::string jsonProcessing = json;

    // Search all array
    while (std::regex_search(jsonProcessing, submatch, regex)) {
        // Build resulting string
        output += std::string(submatch.prefix()) + "\"array\":{\"string.#1\"";
        std::string after = submatch.suffix();
        // try to find the end of array
        auto pos = after.find("}");
        if (pos != std::string::npos) {
            std::string stringArray = after.substr(0, pos + 1);
            after = after.substr(pos + 1);
            std::smatch submatch2;
            int indexString = 2;
            // replace all "string" by "string.#index"
            while (std::regex_search(stringArray, submatch2, regex2)) {
                output += std::string(submatch2.prefix()) + "\"string.#" + std::to_string(indexString++) + "\":";
                stringArray = submatch2.suffix();
            }
            // If there is still a suffix, add it
            output += stringArray;
        }
        // Continue to search with the rest of the string
        jsonProcessing = after;
    }
    // If there is still a suffix, add it
    output += jsonProcessing;

    return output;
}

// replace "string.#index" by "string[index]" for set value in augeas for array
static std::string updateIndexForArray(const std::string& member)
{
    std::regex  regex("^string\\.\\#(\\d+)$");
    std::smatch submatch;
    // replace "string.#index" by "string[index]"
    if (std::regex_search(member, submatch, regex)) {
        std::stringstream buffer;
        buffer << "string[" << submatch[1] << "]";
        return buffer.str();
    }
    return member;
}
//__<< HOTFIX
} // namespace config
