cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

########################################################################################################################

project(fty-config
    VERSION 1.0.0
    DESCRIPTION "fty-config SRR agent"
)

########################################################################################################################

find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR}/fty-cmake)

########################################################################################################################

add_subdirectory(agent)

set(res "resources")

## zconfig.aug -> usr/share/bios/lenses/
install(FILES "${res}/zconfig.aug" DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/bios/lenses/")

## .service -> lib/systemd/system/
set(file "${PROJECT_NAME}.service")
configure_file("${PROJECT_SOURCE_DIR}/${res}/${file}.in" "${PROJECT_BINARY_DIR}/${res}/${file}" @ONLY)
install(FILES "${PROJECT_BINARY_DIR}/${res}/${file}" DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/systemd/system/")

## .cfg -> etc/
set(file "${PROJECT_NAME}.cfg")
configure_file("${PROJECT_SOURCE_DIR}/${res}/${file}.in" "${PROJECT_BINARY_DIR}/${res}/${file}" @ONLY)
install(FILES "${PROJECT_BINARY_DIR}/${res}/${file}" DESTINATION "${CMAKE_INSTALL_FULL_SYSCONFDIR}/${PROJECT_NAME}/")

########################################################################################################################
