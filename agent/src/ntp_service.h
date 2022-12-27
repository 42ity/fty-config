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

#pragma once

#include <string>

namespace ntpservice {

    /**
     * @brief Gets the current status of the Network Time Protocol (NTP) service.
     * 
     * @param status A reference to a bool variable where the status of the NTP service will be stored.
     * 
     * @return int An integer indicating the result of the function. A value of 0 indicates success, 
     *         while a value of -1 indicates an error.
     *  
     * This function uses the "systemctl" command to get the current status of the NTP service, 
     * and stores the result in the bool variable referred to by the "status" parameter.
     */
    int getNtpStatus(bool& status);

    /**
     * @brief Sets the Network Time Protocol (NTP) service to the specified status.
     * 
     * @param status A boolean value indicating the desired status of the NTP service. 
     *               A value of true indicates that the service should be enabled and running, 
     *               while a value of false indicates that it should be stopped and disabled.
     * 
     * @return int An integer indicating the result of the function. A value of 0 indicates success, 
     *         while a value of -1 indicates an error.
     * 
     * This function first gets the current status of the NTP service using the `getNtpStatus` function. 
     * If the current status matches the desired status, the function returns 0 immediately. Otherwise, 
     * it uses the `setNtpStatus` function to enable and start the service if the desired status is true, 
     * or the `resetNtpStatus` function to stop and disable the service if the desired status is false.
     */
    int ntpStatus(bool status);

    /**
     * @brief Enables and starts the Network Time Protocol (NTP) service.
     * 
     * @return int An integer indicating the result of the function. 
     *         A value of 0 indicates success, while a value of -1 indicates an error.
     * 
     * This function uses the "systemctl" command to enable, unmask, and restart the NTP service. 
     * If any of these operations fail, the function logs an error message and returns -1. Otherwise, it returns 0.
     */
    int setNtpStatus();

    /**
     * @brief Stops and disables the Network Time Protocol (NTP) service.
     * 
     * @return int An integer indicating the result of the function. 
     *         A value of 0 indicates success, while a value of -1 indicates an error.
     * 
     * This function uses the "systemctl" command to stop, disable, and mask the NTP service. 
     * If any of these operations fail, the function logs an error message and returns -1. Otherwise, it returns 0.
     */
    int resetNtpStatus();

    /**
     * @brief Executes a command to control the Network Time Protocol (NTP) service.
     * 
     * @param param A string representing the command to be executed (e.g. "start", "stop", "restart").
     * 
     * @return int An integer indicating the result of the function. A value of 0 indicates success, 
     *         while a value of -1 indicates an error.
     * 
     * This function uses the "systemctl" command to control the NTP service, using the parameter provided. 
     * If the command execution is successful, the function returns 0. Otherwise, it logs an error message and returns -1.
     */
    int ntpRunProcess(const std::string& param);

}