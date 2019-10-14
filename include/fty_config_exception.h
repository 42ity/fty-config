/*  =========================================================================
    fty_config_exception - Fty config exceptions

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

#ifndef FTY_CONFIG_EXCEPTION_H_INCLUDED
#define FTY_CONFIG_EXCEPTION_H_INCLUDED

#include <exception>

namespace config
{
    /**
    * Configuration class exception
    */
    class ConfigurationException : public std::runtime_error {
      public:
        ConfigurationException(const std::string& what) : std::runtime_error(what) {}
        ConfigurationException(const char* what) : std::runtime_error(what) {}
        ~ConfigurationException() = default;
    };

} // namepsace config

#endif
