/*  =========================================================================
    fty-config - Configuration agent for 42ITy ecosystem

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

#ifndef FTY_CONFIG_H_H_INCLUDED
#define FTY_CONFIG_H_H_INCLUDED

//  Include the project library file
#include "fty_config_library.h"

//  Add your own public definitions here, if you need them
constexpr auto AGENT_NAME_KEY            = "agentName";
constexpr auto AGENT_NAME                = "fty-config";
constexpr auto ENDPOINT_KEY              = "endPoint";
constexpr auto DEFAULT_ENDPOINT          = "ipc://@/malamute";
constexpr auto CONFIG_DEFAULT_LOG_CONFIG = "/etc/fty/ftylog.cfg";
// Queue definition
constexpr auto QUEUE_NAME_KEY            = "queueName";
constexpr auto MSG_QUEUE_NAME            = "ETN.Q.IPMCORE.CONFIG";
// Features definition
constexpr auto MONITORING_FEATURE_NAME   = "monitoring";
constexpr auto NOTIFICATION_FEATURE_NAME = "notification";
constexpr auto AUTOMATION_SETTINGS       = "automation-settings";
constexpr auto USER_SESSION_FEATURE_NAME = "user-session";
constexpr auto DISCOVERY                 = "discovery";
constexpr auto MASS_MANAGEMENT           = "etn-mass-management";
constexpr auto NETWORK                   = "network";

// Augeas definition
constexpr auto AUGEAS_LENS_PATH          = "AugeasLensPath";
constexpr auto AUGEAS_OPTIONS            = "augeasOptions";
// Properties definition
constexpr auto CONFIG_VERSION_KEY        = "version";
constexpr auto ACTIVE_VERSION            = "1.0";
constexpr auto DATA_MEMBER               = "data";


#endif
