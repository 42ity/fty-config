/*  =========================================================================
    ntp_service - ntp save&restore handlers

    Copyright (C) 2014 - 2022 Eaton

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

#include "ntp_service.h"
#include <fty/process.h>
#include <fty_log.h>

namespace ntpservice
{
    static const std::string SERVICE_NAME{"ntpsec"};

    /**
     * @brief Executes a command to control the Network Time Protocol (NTP) service.
     *
     * @param param A string representing the command to be executed (e.g. "start", "stop", "restart").
     *
     * @return int An integer indicating the result of the function. A value of 0 indicates success,
     *         while a value of -1 indicates an error.
     *
     * This function uses the "systemctl" command to control the NTP service, using the parameter provided.
     * If the command execution is successful, the function returns 0.
     * Otherwise, it logs an error message and returns -1.
     */
    static int ntpRunProcess(const std::string& param)
    {
        if (auto ret = fty::Process::run("sudo", {"/bin/systemctl", param, SERVICE_NAME}); !ret) {
            logError("run process failed (sudo systemctl {} {}), ret: {}", param, SERVICE_NAME, ret.error());
            return -1;
        }
        return 0;
    }

    /**
     * @brief Enables and starts the Network Time Protocol (NTP) service.
     *
     * @return int An integer indicating the result of the function.
     *         A value of 0 indicates success, while a negative value indicates an error.
     *
     * This function uses the "systemctl" command to enable, unmask, and restart the NTP service.
     * If any of these operations fail, the function logs an error message and returns a negative value.
     * Otherwise, it returns 0.
     */
    static int enableService()
    {
        if (ntpRunProcess("unmask") != 0) {
            return -1;
        }
        if (ntpRunProcess("enable") != 0) {
            return -2;
        }
        // after the restart ntp service will be started automatically
        // if (ntpRunProcess("restart") != 0) { // useful?
        //     return -3;
        // }
        return 0;
    }

    /**
     * @brief Stops and disables the Network Time Protocol (NTP) service.
     *
     * @return int An integer indicating the result of the function.
     *         A value of 0 indicates success, while a negative value indicates an error.
     *
     * This function uses the "systemctl" command to stop, disable, and mask the NTP service.
     * If any of these operations fail, the function logs an error message and returns a negative value.
     * Otherwise, it returns 0.
     */
    static int disableService()
    {
        // after the restart ntp service will be stopped automatically
        // if (ntpRunProcess("stop") != 0) { // useful?
        //     return -1;
        // }

        if (ntpRunProcess("disable") != 0) {
            return -2;
        }
        if (ntpRunProcess("mask") != 0) {
            return -3;
        }
        return 0;
    }

    int getState(bool& state)
    {
        std::string s_out;
        if (auto ret = fty::Process::run("sudo", {"/bin/systemctl", "show", SERVICE_NAME, "-p", "ActiveState"}, s_out); !ret) {
            logError("run process failed (sudo systemctl {} {}), ret: {}", "show", SERVICE_NAME, ret.error());
            return -1;
        }

        state = (s_out.find("=active") != std::string::npos);
        logDebug("{} getState: state: {}", SERVICE_NAME, state);
        return 0;
    }

    int applyState(bool state)
    {
        bool currentState = false;
        if (getState(currentState) != 0) {
            return -1; // error
        }

        if (currentState == state) {
            return 0; // ok (nothing to do)
        }

        // apply changes
        int result = state ? enableService() : disableService();
        return result;
    }
} // namespace
