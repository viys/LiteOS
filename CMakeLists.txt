cmake_minimum_required(VERSION 3.15)

# C99
set(CMAKE_C_FLAGS "-std=c99")

# Project name
set(PROJECT_NAME "LiteOS")
project(${PROJECT_NAME})

# Scripts and build output directories
set(KCONFIG_DIR "${CMAKE_SOURCE_DIR}/scripts/kconfig")
set(KCONFIG_CONFIG_H "my_config.h")

# Custom target: menuconfig (outputs to build directory)
if(CMAKE_HOST_WIN32)
    add_custom_target(
        menuconfig
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND menuconfig.exe ${KCONFIG_DIR}/Kconfig
        COMMAND python ${KCONFIG_DIR}/kconfig.py ${KCONFIG_DIR}/Kconfig .config ${CMAKE_BINARY_DIR}/${KCONFIG_CONFIG_H} ${CMAKE_BINARY_DIR}/log.txt ${CMAKE_BINARY_DIR}/.config
        COMMENT "Running menuconfig"
    )
elseif(CMAKE_HOST_APPLE)
    add_custom_target(
        menuconfig
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND menuconfig.exe ${KCONFIG_DIR}/Kconfig
        COMMAND python3 ${KCONFIG_DIR}/kconfig.py ${KCONFIG_DIR}/Kconfig .config ${CMAKE_BINARY_DIR}/${KCONFIG_CONFIG_H} ${CMAKE_BINARY_DIR}/log.txt ${CMAKE_BINARY_DIR}/.config
        COMMENT "Running menuconfig"
    )
elseif(CMAKE_HOST_UNIX)
    add_custom_target(
        menuconfig
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND kconfig-mconf ${KCONFIG_DIR}/Kconfig
        COMMAND python3 ${KCONFIG_DIR}/kconfig.py ${KCONFIG_DIR}/Kconfig .config ${CMAKE_BINARY_DIR}/${KCONFIG_CONFIG_H} ${CMAKE_BINARY_DIR}/log.txt ${CMAKE_BINARY_DIR}/.config
        COMMENT "Running menuconfig"
    )
endif()

# Source files
file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/project/app/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/project/os/*.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Include release/ so that my_config.h is found
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/release
    ${CMAKE_CURRENT_SOURCE_DIR}/project/os
)
