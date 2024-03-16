/*  =========================================================================
    Copyright (C) 2024 Eaton.

    This software is confidential and licensed under Eaton Proprietary License
    (EPL or EULA).
    This software is not authorized to be used, duplicated or disclosed to
    anyone without the prior written permission of Eaton.
    Limitations, restrictions and exclusions of the Eaton applicable standard
    terms and conditions, such as its EPL and EULA, apply.
    =========================================================================
*/

#include <catch2/catch.hpp>
#include "../src/file.h"
#include <string>
#include <iostream>
#include <cxxtools/serializationinfo.h>
#include <fty_common_json.h>

TEST_CASE("base64-file-io")
{
    SECTION("fake-file")
    {
        std::string fileName = "./selftests-ro/fakefile.conf";
        std::cout << "== fileReadToBase64: " << fileName << std::endl;
        std::string encoded = "foo";
        int r = fileReadToBase64(fileName, encoded);
        CHECK(r == 0);
        CHECK(encoded.empty());
        //std::cout << "== encoded: " << encoded << std::endl;

        fileName = "./selftests-rw/fakefile.conf";
        std::cout << "== fileRestoreFromBase64: " << fileName << std::endl;
        r = fileRestoreFromBase64(fileName, encoded);
        CHECK(r == 0);
    }

    SECTION("real-file")
    {
        std::string fileName = "./selftests-ro/file.conf";
        std::cout << "== fileReadToBase64: " << fileName << std::endl;
        std::string encoded;
        int r = fileReadToBase64(fileName, encoded);
        CHECK(r == 0);
        CHECK(!encoded.empty());
        //std::cout << "== encoded: " << encoded << std::endl;

        fileName = "./selftests-rw/file.conf";
        std::cout << "== fileRestoreFromBase64: " << fileName << std::endl;
        r = fileRestoreFromBase64(fileName, encoded);
        CHECK(r == 0);

        std::cout << "== fileReadToBase64: " << fileName << std::endl;
        std::string encoded2;
        r = fileReadToBase64(fileName, encoded2);
        CHECK(r == 0);
        CHECK(!encoded2.empty());
        //std::cout << "== encoded2: " << encoded2 << std::endl;

        CHECK(encoded == encoded2);
    }
}

TEST_CASE("json-base64-file-io")
{
    std::string fileName = "./selftests-ro/file.conf";
    std::cout << "== fileReadToBase64: " << fileName << std::endl;
    std::string encoded;
    int r = fileReadToBase64(fileName, encoded);
    CHECK(r == 0);
    CHECK(!encoded.empty());

    cxxtools::SerializationInfo si;
    si.addMember("data") <<= encoded;
    JSON::writeToFile("./selftests-rw/file.conf.json", si, true);

    cxxtools::SerializationInfo si2;
    JSON::readFromFile("./selftests-rw/file.conf.json", si2);
    std::string encoded2;
    *(si2.findMember("data")) >>= encoded2;
    //std::cout << "== encoded2: " << encoded2 << std::endl;

    fileName = "./selftests-rw/file2.conf";
    std::cout << "== fileRestoreFromBase64: " << fileName << std::endl;
    r = fileRestoreFromBase64(fileName, encoded2);
    CHECK(r == 0);

    std::cout << "== fileReadToBase64: " << fileName << std::endl;
    encoded2.clear();
    r = fileReadToBase64(fileName, encoded2);
    CHECK(r == 0);
    CHECK(!encoded2.empty());
    //std::cout << "== encoded2: " << encoded2 << std::endl;

    CHECK(encoded == encoded2);
}
