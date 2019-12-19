/*  =========================================================================
    fty_config - Binary

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
    fty_config - Binary
@discuss
@end
 */

#include <csignal>
#include <mutex>

#include "fty_config_classes.h"

#include "fty_common_mlm_zconfig.h"

//functions

void usage();
volatile bool g_exit = false;
std::condition_variable g_cv;
std::mutex g_cvMutex;

void sigHandler(int)
{
    g_exit = true;
    g_cv.notify_one();
}

/**
 * Set Signal handler
 */
void setSignalHandler()
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = sigHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
}

/**
 * Set Signal handler
 */
void terminateHandler()
{
    log_error((AGENT_NAME + std::string(" Error")).c_str());
    exit(EXIT_FAILURE);
}

/**
 * Main program
 * @param argc
 * @param argv
 * @return 
 */
int main(int argc, char *argv [])
{
    using Parameters = std::map<std::string, std::string>;
    Parameters paramsConfig;

    // Set signal handler
    setSignalHandler();
    // Set terminate pg handler
    std::set_terminate (terminateHandler);

    ftylog_setInstance(AGENT_NAME, "");

    int argn;
    char *config_file = NULL;
    bool verbose = false;
    // Parse command line
    for (argn = 1; argn < argc; argn++)
    {
        char *param = NULL;
        if (argn < argc - 1) param = argv [argn + 1];

        if (streq(argv [argn], "--help")
                || streq(argv [argn], "-h"))
        {
            usage();
            return EXIT_SUCCESS;
        } else if (streq(argv [argn], "--verbose") || streq(argv [argn], "-v"))
        {
            verbose = true;
        } else if (streq(argv [argn], "--config") || streq(argv [argn], "-c"))
        {
            if (param) config_file = param;
            ++argn;
        }
    }

    // Default configuration.
    paramsConfig[ENDPOINT_KEY] = DEFAULT_ENDPOINT;
    paramsConfig[AGENT_NAME_KEY] = AGENT_NAME;
    paramsConfig[QUEUE_NAME_KEY] = MSG_QUEUE_NAME;
    // Default configuration files path.
    paramsConfig[MONITORING_FEATURE_NAME] = "/etc/fty-nut/fty-nut.cfg";
    paramsConfig[NOTIFICATION_FEATURE_NAME] = "/etc/fty-email/fty-email.cfg";
    paramsConfig[AUTOMATION_SETTINGS] = "/etc/etn-automation/etn-automation.cfg";
    paramsConfig[USER_SESSION_FEATURE_NAME] = "/etc/fty/fty-session.cfg";
    paramsConfig[DISCOVERY] = "/etc/fty-discovery/fty-discovery.cfg";
    paramsConfig[NETWORK] = "/etc/network/interfaces";
    paramsConfig[MASS_MANAGEMENT] = "/var/lib/fty/etn-mass-management/settings.cfg";
    // Default augeas configuration.
    paramsConfig[AUGEAS_LENS_PATH] = "/usr/share/fty/lenses/";
    paramsConfig[AUGEAS_OPTIONS] = AUG_NONE;

    if (config_file)
    {
        log_debug((AGENT_NAME + std::string(": loading configuration file from ") + config_file).c_str());
        mlm::ZConfig config(config_file);
        // verbose mode
        std::istringstream(config.getEntry("server/verbose", "0")) >> verbose;
        // Message bus configuration.
        paramsConfig[ENDPOINT_KEY] = config.getEntry("srr-msg-bus/endpoint", DEFAULT_ENDPOINT);
        paramsConfig[QUEUE_NAME_KEY] = config.getEntry("srr-msg-bus/queueName", MSG_QUEUE_NAME);
        // Configuration file path
        paramsConfig[MONITORING_FEATURE_NAME] = config.getEntry("available-features/monitoring", "");
        paramsConfig[NOTIFICATION_FEATURE_NAME] = config.getEntry("available-features/notification", "");
        paramsConfig[AUTOMATION_SETTINGS] = config.getEntry("available-features/automation-settings", "");
        paramsConfig[USER_SESSION_FEATURE_NAME] = config.getEntry("available-features/user-session", "");
        paramsConfig[DISCOVERY] = config.getEntry("available-features/discovery", "");
        paramsConfig[GENERAL_CONFIG] = config.getEntry("available-features/general-config", "");
        paramsConfig[NETWORK] = config.getEntry("available-features/network", "");
        paramsConfig[MASS_MANAGEMENT] = config.getEntry("available-features/etn-mass-management", "");
        // Augeas configuration
        paramsConfig[AUGEAS_LENS_PATH] = config.getEntry("augeas/lensPath", "/usr/share/fty/lenses/");
        paramsConfig[AUGEAS_OPTIONS] = config.getEntry("augeas/augeasOptions", "0");
    }

    if (verbose)
    {
        ftylog_setVeboseMode(ftylog_getInstance());
        log_trace("Verbose mode OK");
    }

    log_info((AGENT_NAME + std::string(" starting")).c_str());

    // Start config agent
    config::ConfigurationManager configManager(paramsConfig);

    //wait until interrupt
    std::unique_lock<std::mutex> lock(g_cvMutex);
    g_cv.wait(lock, [] { return g_exit; });

    log_info((AGENT_NAME + std::string(" interrupted")).c_str());

    // Exit application
    return EXIT_SUCCESS;
}

void usage()
{
    puts((AGENT_NAME + std::string(" [options] ...")).c_str());
    puts("  -v|--verbose        verbose test output");
    puts("  -h|--help           this information");
    puts("  -c|--config         path to config file");
}
