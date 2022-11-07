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

/** tools to handle file save & restore (bulk save & restore) */

#include "file.h"
#include <fty_log.h>
#include <cxxtools/base64codec.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

// read fileName, returns char* buffer of size bytes
// returned ptr must be freed by caller
static char* s_fileRead(const std::string& fileName, size_t& size)
{
    char* buffer = nullptr;
    try {
        std::ifstream is(fileName, std::ios::in | std::ios::binary);
        if (is.is_open()) {
            is.seekg(0, is.end);
            size_t fSize = size_t(is.tellg());
            is.seekg(0, is.beg);
            if (fSize != 0) {
                buffer = static_cast<char*>(malloc(sizeof(char) * (fSize + 1)));
                if (!buffer) {
                    throw std::runtime_error("malloc() failed");
                }
                memset(buffer, 0, fSize + 1);
                is.read(buffer, std::streamsize(fSize));
            }
            is.close();
            size = fSize;
            return buffer;
        }
    }
    catch (const std::exception& e) {
        logError("{}", e.what());
    }

    if (buffer)
        { free(buffer); }

    size = 0;
    return nullptr;
}

// write char* buffer of size bytes in fileName
// returns 0 if ok, else <0
static int s_fileWrite(const std::string& fileName, const char* buffer, size_t size)
{
    try {
        std::ofstream os(fileName, std::ios::out | std::ios::binary);
        if (buffer && (size != 0)) {
            os.write(buffer, std::streamsize(size));
        }
        os.close();
        return 0;
    }
    catch (const std::exception& e) {
        logError("{}", e.what());
    }
    return -1;
}

// read fileName content to b64 encoded
// returns 0 if ok (b64 is set), else <0
int fileReadToBase64(const std::string& fileName, std::string& b64)
{
    b64.clear();

    char* buffer = nullptr;
    int ret = 0;
    try {
        size_t size = 0;
        buffer = s_fileRead(fileName, size);
        if (buffer && (size != 0)) {
            b64 = cxxtools::Base64Codec::encode(buffer, unsigned(size));
        }
    }
    catch (const std::exception& e) {
        logError("{}", e.what());
        ret = -1;
    }

    if (buffer)
        { free(buffer); }
    return ret;
}

// write fileName content from b64 decoded
// returns 0 if ok, else <0
int fileRestoreFromBase64(const std::string& fileName, const std::string& b64)
{
    try {
        logDebug("fileName: {}", fileName);
        logDebug("b64: {}", b64);

        std::string buffer;
        if (!b64.empty()) {
            buffer = cxxtools::Base64Codec::decode(b64);
        }
        //logDebug("buffer: {}", buffer);
        logDebug("buffer size: {}", buffer.size());

        int r = s_fileWrite(fileName, buffer.c_str(), buffer.size());
        if (r != 0) {
            throw std::runtime_error("s_fileWrite() failed");
        }
        return 0;
    }
    catch (const std::exception& e) {
        logError("{}", e.what());
    }
    return -1;
}

//DBG, dump file content
void fileDumpToConsole(const std::string& fileName, const std::string& msg)
{
    if (!ftylog_isLogDebug(ftylog_getInstance()))
        return; // debug only

    try {
        logDebug("{}{}", msg, fileName);
        std::ifstream is(fileName, std::ifstream::in);
        if (is.is_open()) {
            std::string line;
            while (std::getline(is, line)) {
                logDebug("{}", line);
            }
            is.close();
        }
    }
    catch (const std::exception& e) {
        logDebug("fileDumpToConsole failed ({}, e: {})", fileName, e.what());
    }
}
