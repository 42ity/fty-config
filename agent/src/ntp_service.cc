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
#include <fty_common.h>

namespace ntpservice{

    int getNtpStatus(bool& status) {
        int result = -1;
        std::string s_out;

        if(fty::Process::run("sudo", {"systemctl", "show", "ntp", "-p", "ActiveState"}, s_out)) {
            result = 0;
            if(s_out == "ActiveState=inactive\n") {
                status = false;
            } else if(s_out == "ActiveState=active\n") {
                status = true;
            }
        }

        return result;
    }

    int ntpStatus(bool status) {
        bool oldStatus;
        int result = 0;

        if(!getNtpStatus(oldStatus)) {
            if(oldStatus == status) {
                return 0;
            } 
        }
        
        if(status) {
            result = setNtpStatus();
        } else {
            result = resetNtpStatus();
        }

        return result;
    }

    int ntpRunProcess(const std::string& param) {
        int result = 0;

        if(!fty::Process::run("sudo", {"systemctl", param, "ntp"})) {
            logError("ntp set status failed ( sudo systemctl {} ntp)", param);
            return -1;
        }

        return result;
    }

    int setNtpStatus() {

        if(ntpRunProcess("unmask")) {
            return -1;
        }

         if(ntpRunProcess("enable")) {
            return -1;
        }

         if(ntpRunProcess("restart")) {
            return -1;
        }

        return 0;
    }

    int resetNtpStatus() {

        if(ntpRunProcess("stop")) {
            return -1;
        }

         if(ntpRunProcess("disable")) {
            return -1;
        }

         if(ntpRunProcess("mask")) {
            return -1;
        }

        return 0;
    }
} //namespace ntpconfig