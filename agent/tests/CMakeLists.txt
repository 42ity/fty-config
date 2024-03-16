cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

etn_test_target(${PROJECT_NAME}
    CONFIGS
        selftests-ro/*
    SOURCES
        ../src/file.*

        src/main.cc
        src/file.cc
    USES
        cxxtools
        fty_common
    PREPROCESSOR
        -DCATCH_CONFIG_FAST_COMPILE
)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/selftests-rw/")
