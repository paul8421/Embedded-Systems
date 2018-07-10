cmake_minimum_required(VERSION 3.3)

# include configuration
include(Config.cmake)

set(CMAKE_TOOLCHAIN_FILE libs/Toolchain.cmake)


project(EmbeddedSystems18)

include(libs/Arduino.cmake)
include(libs/Pi.cmake)
include(libs/Zumo.cmake)

include_directories(libs/rf24)

# Scout Executable
add_pi_executable(scout src/scout/main.cpp
        src/scout/nrf24.cpp
        src/radioPinFunctions.cpp)


# Collector Executable
add_zumo_executable(collector
        src/collector/main.cpp
        src/collector/nrf24.cpp
        src/collector/radioPinFunctions.cpp)

