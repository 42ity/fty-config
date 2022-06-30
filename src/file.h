/*  =========================================================================
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

#pragma once

#include <string>

/// read fileName content to b64 encoded
/// returns 0 if ok (b64 is set), else <0
int fileReadToBase64(const std::string& fileName, std::string& b64);

/// write fileName content from b64 decoded
/// returns 0 if ok, else <0
int fileRestoreFromBase64(const std::string& fileName, const std::string& b64);

//dump fileName to console, log debug only
void fileDumpToConsole(const std::string& fileName, const std::string& msg = "");
