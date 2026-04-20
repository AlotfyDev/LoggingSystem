cmake_minimum_required(VERSION 3.16)
project(LoggingSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Header-only library
add_library(logging_system INTERFACE)
target_include_directories(logging_system INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Enable all warnings
if(MSVC)
    target_compile_options(logging_system INTERFACE /W4)
else()
    target_compile_options(logging_system INTERFACE -Wall -Wextra -Wpedantic)
endif()

# Install headers
install(DIRECTORY include/ DESTINATION include)

# Export compile commands for language server
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)