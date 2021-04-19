cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

########################################################################################################################

project(fty-config
    VERSION 1.0.0
    DESCRIPTION "Configuration agent for 42ITy ecosystem"
)

########################################################################################################################
find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR}/fty-cmake)
########################################################################################################################

etn_target(exe ${PROJECT_NAME}
    SOURCES
        src/fty_config_exception.h
        src/fty-config.h
        src/fty-config.cc
        src/fty_config_manager.cc
        src/fty_config_manager.h
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
)

########################################################################################################################
install(FILES zconfig.aug DESTINATION /usr/share/bios/lenses)
########################################################################################################################
etn_configure_file(
    ${PROJECT_NAME}.service.in

    TARGET      ${PROJECT_NAME}
    DESTINATION /usr/lib/systemd/system/
)
########################################################################################################################
etn_configure_file(
    ${PROJECT_NAME}.cfg.in

    TARGET      ${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}/fty-config/
)
########################################################################################################################
