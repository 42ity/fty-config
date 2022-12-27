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

namespace ntpservice
{
    /**
     * @brief Gets the current state of the Network Time Protocol (NTP) service.
     *
     * @param state A reference to a bool variable where the state of the NTP service will be stored.
     *
     * @return int An integer indicating the result of the function. A value of 0 indicates success,
     *         while a negative value indicates an error.
     *
     * This function uses the "systemctl" command to get the current state of the NTP service,
     * and stores the result in the bool variable referred to by the "state" parameter.
     */
    int getState(bool& state);

    /**
     * @brief Sets the Network Time Protocol (NTP) service to the specified state.
     *
     * @param state A boolean value indicating the desired state of the NTP service.
     *               A value of true indicates that the service should be enabled and running,
     *               while a value of false indicates that it should be stopped and disabled.
     *
     * @return int An integer indicating the result of the function. A value of 0 indicates success,
     *         while a negative value indicates an error.
     *
     * This function first gets the current state of the NTP service using the `getState` function.
     * If the current state matches the desired state, the function returns 0 immediately. Otherwise,
     * it change the service state.
     */
    int applyState(bool state);
} // namespace
