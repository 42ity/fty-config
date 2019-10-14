/*  =========================================================================
    fty_config_manager - Fty config manager

    Copyright (C) 2014 - 2018 Eaton

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

#include "fty_config_classes.h"

using namespace std::placeholders;

namespace config
{
    /**
     * Constructor
     * @param parameters
     * @param streamPublisher
     */
    ConfigurationManager::ConfigurationManager(const std::map<std::string, std::string> & parameters)
    : m_parameters(parameters), m_aug(NULL)
    {
        init();
    }

    /**
     * Destructor
     */
    ConfigurationManager::~ConfigurationManager()
    {
        log_debug("Release all config resources");
        if (!m_aug)
        {
            aug_close(m_aug);
            log_debug("Augeas resource released");
        }
        if (m_msgBus) 
        {
            delete m_msgBus;
            log_debug("Message bus resource released");
        }
    }

    /**
     * Initialization class
     */
    void ConfigurationManager::init()
    {
        try
        {
            // Augeas tool init
            int augeasOpt = getAugeasFlags(m_parameters.at(AUGEAS_OPTIONS));
            log_debug("augeas options: %d", augeasOpt);

            m_aug = aug_init(FILE_SEPARATOR, m_parameters.at(AUGEAS_LENS_PATH).c_str(), augeasOpt);
            if (!m_aug)
            {
                throw ConfigurationException("Augeas tool initialization failed");
            }

            // Message bus init
            m_msgBus = messagebus::MlmMessageBus(m_parameters.at(ENDPOINT_KEY), m_parameters.at(AGENT_NAME_KEY));
            m_msgBus->connect();
            
            // Listen all incoming request
            auto fct = std::bind(&ConfigurationManager::handleRequest, this, _1);
            m_msgBus->receive(m_parameters.at(QUEUE_NAME_KEY), fct);
        }
        catch (messagebus::MessageBusException& ex)
        {
            log_error("Message bus error: %s", ex.what());
        } catch (...)
        {
            log_error("Unexpected error: unknown");
        }
    }

    /**
     * Handle any request
     * @param sender
     * @param payload
     * @return 
     */
    void ConfigurationManager::handleRequest(messagebus::Message msg)
    {
        dto::config::ConfigResponseDto responseDto("?", STATUS_FAILED);
        try
        {
            log_debug("Config handleRequest:");
            messagebus::UserData data = msg.userData();
            dto::config::ConfigQueryDto configQuery;
            data >> configQuery;

            log_debug("Config query: action:'%s', feature name:'%s'", configQuery.action.c_str(), configQuery.featureName.c_str());
            
            if (configQuery.action.size() == 0 || configQuery.featureName.size() == 0)
            {
                throw ConfigurationException("Empty request");
            }

            responseDto.featureName = configQuery.featureName;
            // Check if the command is implemented
            if (configQuery.action.compare(SAVE_ACTION) == 0)
            {
                // Get the configuration file path name from class variable m_parameters
                std::string configurationFileName = AUGEAS_FILES + m_parameters.at(responseDto.featureName) + ANY_NODES;
                log_debug("Configuration file name '%s'", configurationFileName.c_str());
                cxxtools::SerializationInfo si;
                getConfigurationToJson(si, configurationFileName);
                // Set response
                setResponse(responseDto, si);
            } 
            else if (configQuery.action.compare(RESTORE_ACTION) == 0)
            {
                std::string configurationFileName = AUGEAS_FILES + m_parameters.at(responseDto.featureName);
                // Set all values for a feature name.
                log_debug("Payload to set: %s", configQuery.data.c_str());
                cxxtools::SerializationInfo si;
                JSON::readFromString (configQuery.data, si);
                // Test if the data is well formated
                if (si.category () != cxxtools::SerializationInfo::Array ) 
                {
                    throw std::invalid_argument("Input datat must be an array");
                }
                // Iterate on array
                cxxtools::SerializationInfo::Iterator it;
                for (it = si.begin(); it != si.end(); ++it)
                {
                    for (auto &siFeature : (cxxtools::SerializationInfo)*it)
                    {
                        cxxtools::SerializationInfo siData = siFeature.getMember(DATA_MEMBER);
                        int returnValue = setConfiguration(&siData, configurationFileName);
                        if (returnValue == 0)
                        {
                            log_debug("Set configuration for: %s succeed!", responseDto.featureName.c_str());
                            responseDto.status = STATUS_SUCCESS;
                        } else
                        {
                            std::string errorMsg = "Set configuration for: " + responseDto.featureName + " failed!";
                            log_error(errorMsg.c_str());
                            responseDto.error= errorMsg;
                        }
                    
                    }
                }
            } else
            {
                throw ConfigurationException("Wrong command");
            }
            // Send response
            sendResponse(msg, responseDto, configQuery.action);

        } catch (const std::out_of_range& oor)
        {
            log_error("Feature name not found");
        } catch (std::exception &e)
        {
            log_error("Unexpected error: %s", e.what());
        } catch (...)
        {
            log_error("Unexpected error: unknown");
        }
    }
    
    /**
     * Set response before to sent it.
     * @param msg
     * @param responseDto
     * @param configQuery
     */
    void ConfigurationManager::setResponse (dto::config::ConfigResponseDto& respDto, const cxxtools::SerializationInfo& siInput)
    {
        // Array si
        cxxtools::SerializationInfo jsonResp;
        jsonResp.setCategory(cxxtools::SerializationInfo::Category::Array);
        // Main si 
        cxxtools::SerializationInfo si;
        // Feature si
        cxxtools::SerializationInfo siFeature;
        siFeature.addMember(respDto.featureName);;
        // Content si                
        cxxtools::SerializationInfo siTemp;
        siTemp.addMember(SRR_VERSION) <<= ACTIVE_VERSION;
        cxxtools::SerializationInfo& siData = siTemp.addMember(DATA_MEMBER);
        siData <<= siInput;
        siData.setName(DATA_MEMBER);
        // Add si version + datat in si feature
        siFeature <<= siTemp;
        siFeature.setName(respDto.featureName);
        // Add the feature in the main si
        si.addMember("") <<= siFeature;
        // Put in the array
        jsonResp.addMember("") <<= si;
        // Serialize the response
        respDto.data = JSON::writeToString (jsonResp, false);
        respDto.status = STATUS_SUCCESS;
    }
    
    /**
     * Send response on message bus.
     * @param msg
     * @param responseDto
     * @param configQuery
     */
    void ConfigurationManager::sendResponse(const messagebus::Message& msg, const dto::config::ConfigResponseDto& responseDto, const std::string& subject)
    {
        try
        {
            messagebus::Message resp;
            resp.userData() << responseDto;
            resp.metaData().emplace(messagebus::Message::SUBJECT, subject);
            resp.metaData().emplace(messagebus::Message::FROM, m_parameters.at(AGENT_NAME_KEY));
            resp.metaData().emplace(messagebus::Message::TO, msg.metaData().find(messagebus::Message::FROM)->second);
            resp.metaData().emplace(messagebus::Message::COORELATION_ID, msg.metaData().find(messagebus::Message::COORELATION_ID)->second);
            m_msgBus->sendReply(msg.metaData().find(messagebus::Message::REPLY_TO)->second, resp);
        }
        catch (messagebus::MessageBusException& ex)
        {
            log_error("Message bus error: %s", ex.what());
        } catch (...)
        {
            log_error("Unexpected error: unknown");
        }
    }

    /**
     * Set a configuration.
     * @param si
     * @param path
     * @return 
     */
    int ConfigurationManager::setConfiguration(cxxtools::SerializationInfo* si, const std::string& path)
    {
        cxxtools::SerializationInfo::Iterator it;
        for (it = si->begin(); it != si->end(); ++it)
        {
            cxxtools::SerializationInfo *member = &(*it);
            std::string memberName = member->name();

            cxxtools::SerializationInfo::Iterator itElement;
            for (itElement = member->begin(); itElement != member->end(); ++itElement)
            {
                cxxtools::SerializationInfo *element = &(*itElement);
                std::string elementName = element->name();
                std::string elementValue;
                element->getValue(elementValue);
                // Build augeas full path
                std::string fullPath = path + FILE_SEPARATOR + memberName + FILE_SEPARATOR + elementName;
                // Set value
                int setReturn = aug_set(m_aug, fullPath.c_str(), elementValue.c_str());
            }
        }
        return aug_save(m_aug);
    }

    /**
     * Get a configuration serialized to json format.
     * @param path
     */
    void ConfigurationManager::getConfigurationToJson(cxxtools::SerializationInfo& si, std::string& path)
    {
        char **matches;
        int nmatches = aug_match(m_aug, path.c_str(), &matches);

        // no matches, stop it.
        if (nmatches < 0) return;

        // Iterate on all matches
        for (int i = 0; i < nmatches; i++)
        {
            std::string temp = matches[i];
            // Skip all comments
            if (temp.find(COMMENTS_DELIMITER) == std::string::npos)
            {
                const char *value, *label;
                aug_get(m_aug, matches[i], &value);
                aug_label(m_aug, matches[i], &label);
                if (!value)
                {
                    // It's a member
                    si.addMember(label);
                } else
                {
                    std::string t = findMemberFromMatch(temp);
                    cxxtools::SerializationInfo *siTemp = si.findMember(t);
                    siTemp->addMember(label) <<= value;
                }
                getConfigurationToJson(si, temp.append(ANY_NODES));
            }
        }
    }

    /**
     * Find a member
     * @param input
     * @return 
     */
    std::string ConfigurationManager::findMemberFromMatch(const std::string& input)
    {
        std::string returnValue = "";
        if (input.length() > 0)
        {
            // Try to find last /
            std::size_t found = input.find_last_of(FILE_SEPARATOR);
            if (found != -1)
            {
                std::string temp = input.substr(0, found);
                found = temp.find_last_of(FILE_SEPARATOR);
                returnValue = temp.substr(found + 1, temp.length());
            }
        }
        return returnValue;
    }

    /**
     * Utilitary to dump a configuration.
     * @param path
     */
    void ConfigurationManager::dumpConfiguration(std::string& path)
    {
        char **matches;
        int nmatches = aug_match(m_aug, path.c_str(), &matches);

        // Stop if not matches.
        if (nmatches < 0) return;

        // Iterate on all matches
        for (int i = 0; i < nmatches; i++)
        {
            std::string temp = matches[i];
            // Skip all comments
            if (temp.find(COMMENTS_DELIMITER) == std::string::npos)
            {
                const char *value, *label;
                aug_get(m_aug, matches[i], &value);
                aug_label(m_aug, matches[i], &label);
                dumpConfiguration(temp.append(ANY_NODES));
            }
        }
    }

    /**
     * Get augeas tool flag
     * @param augeasOpts
     * @return 
     */
    int ConfigurationManager::getAugeasFlags(std::string& augeasOpts)
    {
        int returnValue = AUG_NONE;
        // Build static augeas option
        static std::map<const std::string, aug_flags> augFlags;
        augFlags["AUG_NONE"] = AUG_NONE;
        augFlags["AUG_SAVE_BACKUP"] = AUG_SAVE_BACKUP;
        augFlags["AUG_SAVE_NEWFILE"] = AUG_SAVE_NEWFILE;
        augFlags["AUG_TYPE_CHECK"] = AUG_TYPE_CHECK;
        augFlags["AUG_NO_STDINC"] = AUG_NO_STDINC;
        augFlags["AUG_SAVE_NOOP"] = AUG_SAVE_NOOP;
        augFlags["AUG_NO_LOAD"] = AUG_NO_LOAD;
        augFlags["AUG_NO_MODL_AUTOLOAD"] = AUG_NO_MODL_AUTOLOAD;
        augFlags["AUG_ENABLE_SPAN"] = AUG_ENABLE_SPAN;
        augFlags["AUG_NO_ERR_CLOSE"] = AUG_NO_ERR_CLOSE;
        augFlags["AUG_TRACE_MODULE_LOADING"] = AUG_TRACE_MODULE_LOADING;
        
        if (augeasOpts.size() > 1 )
        {
            // Replace '|' by ' '
            std::replace(augeasOpts.begin(), augeasOpts.end(), '|', ' ');

            // Build all options parameters in array
            std::vector<std::string> augOptsArray;
            std::stringstream ss(augeasOpts);
            std::string temp;
            while (ss >> temp)
            {
                augOptsArray.push_back(temp);
            }

            // Builds augeas options
            std::vector<std::string>::iterator it;
            for (it = augOptsArray.begin(); it != augOptsArray.end(); ++it)
            {
                returnValue |= augFlags.at(*it);
            }
        }
        return returnValue;
    }
}