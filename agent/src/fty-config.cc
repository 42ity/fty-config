/*  =========================================================================
    fty_config - Binary

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


#include "fty-config.h"
#include "fty_config_manager.h"
#include <fty_common_mlm_zconfig.h>
#include <fty_log.h>
#include <augeas.h>

#include <condition_variable>
#include <csignal>
#include <map>
#include <mutex>
#include <sstream>

static bool                    g_exit = false;
static std::condition_variable g_cv;
static std::mutex              g_cvMutex;

static void usage()
{
    printf("%s [options]\n", AGENT_NAME);
    printf("  -h|--help           this information\n");
    printf("  -v|--verbose        verbose test output\n");
    printf("  -c|--config <path>  path to config file\n");
}

static void sigHandler(int)
{
    g_exit = true;
    g_cv.notify_one();
}

/**
 * Set Signal handler
 */
static void setSignalHandler()
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = sigHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);
}

/**
 * Set Signal handler
 */
static void terminateHandler()
{
    log_error("%s: Error", AGENT_NAME);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    // Set signal handler
    setSignalHandler();
    // Set terminate pg handler
    std::set_terminate(terminateHandler);

    char* config_file = nullptr;
    bool  verbose     = false;

    // Parse command line
    for (int argn = 1; argn < argc; argn++) {
        char* arg = argv[argn];
        char* param = (argn < (argc - 1)) ? argv[argn + 1] : nullptr;

        if (streq(arg, "--help") || streq(arg, "-h")) {
            usage();
            return EXIT_SUCCESS;
        }
        else if (streq(arg, "--verbose") || streq(arg, "-v")) {
            verbose = true;
        }
        else if (streq(arg, "--config") || streq(arg, "-c")) {
            if (!param) {
                fprintf(stderr, "%s: missing <path> argument\n", arg);
                return EXIT_FAILURE;
            }
            config_file = param;
            argn++;
        }
        else {
            fprintf(stderr, "%s: unknown argument\n", arg);
            return EXIT_FAILURE;
        }
    }

    ftylog_setInstance(AGENT_NAME, FTY_COMMON_LOGGING_DEFAULT_CFG);

    // Default configuration.
    std::map<std::string, std::string> paramsConfig;

    paramsConfig[ENDPOINT_KEY]   = DEFAULT_ENDPOINT;
    paramsConfig[AGENT_NAME_KEY] = AGENT_NAME;
    paramsConfig[QUEUE_NAME_KEY] = MSG_QUEUE_NAME;

    // Default configuration files path.
    paramsConfig[MONITORING_FEATURE_NAME]   = "/etc/fty-nut/fty-nut.cfg";
    paramsConfig[NOTIFICATION_FEATURE_NAME] = "/etc/fty-email/fty-email.cfg";
    paramsConfig[AUTOMATION_SETTINGS]       = "/etc/fty/etn-automation.cfg";
    paramsConfig[USER_SESSION_FEATURE_NAME] = "/etc/fty/fty-session.cfg";
    paramsConfig[DISCOVERY_SETTINGS]        = "/etc/fty-discovery-ng/config-discovery.conf";
    paramsConfig[DISCOVERY_AGENT_SETTINGS]  = "/etc/fty-discovery-ng/discovery.conf";
    paramsConfig[MASS_MANAGEMENT]           = "/var/lib/fty/etn-mass-management/settings.cfg";
    paramsConfig[NETWORK]                   = "/etc/network/interfaces";
    paramsConfig[NETWORK_AGENT_SETTINGS]    = "/var/lib/fty/etn-ipm2-network/etn-ipm2-network.json";
    paramsConfig[NETWORK_HOST_NAME]         = "/etc/hostname";
    paramsConfig[NETWORK_PROXY]             = "/etc/default/fty-proxy";
    paramsConfig[TIMEZONE_SETTINGS]         = "/etc/fty/fty-datetime.cfg";
    paramsConfig[NTP_SETTINGS]              = "/etc/ntp.conf";

    // Default augeas configuration.
    paramsConfig[AUGEAS_LENS_PATH] = "/usr/share/fty/lenses/";
    paramsConfig[AUGEAS_OPTIONS]   = AUG_NONE;

    // version
    paramsConfig[CONFIG_VERSION_KEY] = CONFIG_VERSION;

    if (config_file) {
        log_debug((AGENT_NAME + std::string(": loading configuration file from ") + config_file).c_str());
        mlm::ZConfig config(config_file);

        // verbose mode
        std::istringstream(config.getEntry("server/verbose", "0")) >> verbose;

        // Message bus configuration.
        paramsConfig[ENDPOINT_KEY]   = config.getEntry("srr-msg-bus/endpoint", DEFAULT_ENDPOINT);
        paramsConfig[QUEUE_NAME_KEY] = config.getEntry("srr-msg-bus/queueName", MSG_QUEUE_NAME);

        // Configuration file path
        paramsConfig[MONITORING_FEATURE_NAME]   = config.getEntry("available-features/monitoring", "");
        paramsConfig[NOTIFICATION_FEATURE_NAME] = config.getEntry("available-features/notification", "");
        paramsConfig[AUTOMATION_SETTINGS]       = config.getEntry("available-features/automation-settings", "");
        paramsConfig[USER_SESSION_FEATURE_NAME] = config.getEntry("available-features/user-session", "");
        paramsConfig[DISCOVERY_SETTINGS]        = config.getEntry("available-features/discovery-ng-settings", "");
        paramsConfig[DISCOVERY_AGENT_SETTINGS]  = config.getEntry("available-features/discovery-ng-agent-settings", "");
        paramsConfig[MASS_MANAGEMENT]           = config.getEntry("available-features/etn-mass-management", "");
        paramsConfig[NETWORK]                   = config.getEntry("available-features/network", "");
        paramsConfig[NETWORK_AGENT_SETTINGS]    = config.getEntry("available-features/network-agent-settings", "");
        paramsConfig[NETWORK_HOST_NAME]         = config.getEntry("available-features/network-host-name", "");
        paramsConfig[NETWORK_PROXY]             = config.getEntry("available-features/network-proxy", "");
        paramsConfig[TIMEZONE_SETTINGS]         = config.getEntry("available-features/timezone-settings", "");
        paramsConfig[NTP_SETTINGS]              = config.getEntry("available-features/ntp-settings", "");

        // Augeas configuration
        paramsConfig[AUGEAS_LENS_PATH] = config.getEntry("augeas/lensPath", "/usr/share/fty/lenses/");
        paramsConfig[AUGEAS_OPTIONS]   = config.getEntry("augeas/augeasOptions", "0");

        // version
        paramsConfig[CONFIG_VERSION_KEY] = config.getEntry("config/version", CONFIG_VERSION);
    }

    if (verbose) {
        ftylog_setVerboseMode(ftylog_getInstance());
        log_trace("Set verbose mode");
    }

    log_info("%s: Starting...", AGENT_NAME);

    // Start config agent
    config::ConfigurationManager configManager(paramsConfig);

    log_info("%s: Started", AGENT_NAME);

    // wait until interrupt
    std::unique_lock<std::mutex> lock(g_cvMutex);
    g_cv.wait(lock, [] {
        return g_exit;
    });

    log_info("%s: Ended", AGENT_NAME);

    return EXIT_SUCCESS;
}