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

#ifndef FTY_CONFIG_MANAGER_H_INCLUDED
#define FTY_CONFIG_MANAGER_H_INCLUDED

#include <augeas.h>

/**
 * \brief Agent config server actor
 */

namespace config
{
    #define FILE_SEPARATOR      "/"
    #define ANY_NODES           FILE_SEPARATOR "*"
    #define COMMENTS_DELIMITER  "#"
    #define AUGEAS_FILES        "/files"
    #define FEATURE_SEPARATOR   ","
    
    class ConfigurationManager 
    {

        public:
            explicit ConfigurationManager(const std::map<std::string, std::string> & parameters);
            ~ConfigurationManager();

        private:

            std::map<std::string, std::string> m_parameters;
            augeas *m_aug;
            messagebus::MessageBus *m_msgBus;

            void init();
            void handleRequest(messagebus::Message msg);

            void getConfigurationToJson(cxxtools::SerializationInfo &si, std::string &path);
            int setConfiguration(cxxtools::SerializationInfo *si, const std::string &path);

            std::string getConfigurationFileName (const std::string& featureName);

            void setSaveResponse (const std::map<std::string, cxxtools::SerializationInfo>& configSiList, dto::config::ConfigResponseDto& respDto);
            void sendResponse(const messagebus::Message& msg, const messagebus::UserData& userData, const std::string& subject);

            // Utility
            void dumpConfiguration(std::string& path);
            std::string findMemberFromMatch(const std::string& input);
            int getAugeasFlags(std::string& augeasOpts);
            std::vector<std::string> splitString(const std::string& inputString);
        };
        
} // namespace config

#endif