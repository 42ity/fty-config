cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

########################################################################################################################

etn_target(exe ${PROJECT_NAME}
    SOURCES
        src/fty_config_exception.h
        src/fty-config.h
        src/fty-config.cc
        src/fty_config_manager.cc
        src/fty_config_manager.h
        src/file.cc
        src/file.h
        src/ntp_service.cc
        src/ntp_service.h
    FLAGS
        -Wno-disabled-macro-expansion
    USES
        cxxtools
        protobuf
        augeas
        fty_common
        fty_common_dto
        fty_common_logging
        fty_common_messagebus
        fty_common_mlm
        fty-utils
)

add_subdirectory(tests)
