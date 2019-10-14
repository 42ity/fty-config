/*  =========================================================================
    fty-config - Configuration agent for 42ITy ecosystem

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

#ifndef FTY_CONFIG_H_H_INCLUDED
#define FTY_CONFIG_H_H_INCLUDED

//  Include the project library file
#include "fty_config_library.h"

//  Add your own public definitions here, if you need them
#define AGENT_NAME_KEY              "agentName"
#define AGENT_NAME                  "fty-config"
#define ENDPOINT_KEY                "endPoint"
#define DEFAULT_ENDPOINT            "ipc://@/malamute"
#define CONFIG_DEFAULT_LOG_CONFIG   "/etc/fty/ftylog.cfg"
// Queue definition
#define QUEUE_NAME_KEY              "queueName"
#define MSG_QUEUE_NAME              "ETN.Q.IPMCORE.CONFIG"
// Action definition
#define SAVE_ACTION                 "save"
#define RESTORE_ACTION              "restore"
#define RESET_ACTION                "reset"
// Status definition
#define STATUS_SUCCESS              "success"
#define STATUS_FAILED               "failed"
// Features definition
#define MONITORING_FEATURE_NAME     "monitoring"
#define NOTIFICATION_FEATURE_NAME   "notification"
#define AUTOMATION_SETTINGS         "automation-settings"
#define USER_SESSION_FEATURE_NAME   "user-session"
#define DISCOVERY                   "discovery"
#define GENERAL_CONFIG              "general-config"
#define NETWORK                     "network"

// Augeas definition
#define AUGEAS_LENS_PATH            "AugeasLensPath"
#define AUGEAS_OPTIONS              "augeasOptions"
// Properties definition
#define SRR_VERSION                 "version"
#define ACTIVE_VERSION              "1.0"
#define DATA_MEMBER                 "data"

#endif
