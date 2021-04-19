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

#pragma once

#include <augeas.h>
#include <cxxtools/serializationinfo.h>
#include <fty_common_messagebus.h>
#include <fty_srr_dto.h>
#include <map>
#include <string>

/**
 * \brief Agent config server actor
 */

namespace config {
class ConfigurationManager
{

public:
    explicit ConfigurationManager(const std::map<std::string, std::string>& parameters);
    ~ConfigurationManager() = default;

private:
    std::map<std::string, std::string> m_parameters;
    using AugeasSmartPtr = std::unique_ptr<augeas, decltype(&aug_close)>;
    AugeasSmartPtr                          m_aug;
    std::unique_ptr<messagebus::MessageBus> m_msgBus;
    dto::srr::SrrQueryProcessor             m_processor;
    std::string                             m_configVersion;

    void init();
    void handleRequest(messagebus::Message msg);

    // Request processor
    dto::srr::SaveResponse    saveConfiguration(const dto::srr::SaveQuery& query);
    dto::srr::RestoreResponse restoreConfiguration(const dto::srr::RestoreQuery& query);
    dto::srr::ResetResponse   resetConfiguration(const dto::srr::ResetQuery& query);

    void getConfigurationToJson(cxxtools::SerializationInfo& si, std::string& path, std::string& rootMember);
    int  setConfiguration(cxxtools::SerializationInfo& si, const std::string& path);
    void sendResponse(const messagebus::Message& msg, const dto::UserData& userData);

    // Utility
    std::string              getConfigurationFileName(const std::string& featureName);
    void                     dumpConfiguration(std::string& path);
    std::vector<std::string> findMembersFromMatch(const std::string& input, const std::string& rootMember);
    int                      getAugeasFlags(std::string& augeasOpts);
    bool                     isVerstionCompatible(const std::string& version);
    void                     persistValue(const std::string& fullPath, const std::string& value);
};

} // namespace config
