cmake_minimum_required(VERSION 3.20)
project(SpectraVision VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configuration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ═════════════════════════════════════════════════════════════════════════════
# DEPENDENCIES
# ═════════════════════════════════════════════════════════════════════════════

find_package(Threads REQUIRED)

# External dependencies
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/cpp-httplib
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nlohmann-json
)

# ═════════════════════════════════════════════════════════════════════════════
# SOURCE FILES
# ═════════════════════════════════════════════════════════════════════════════

set(SV_CORE_SOURCES
    src/logger.cpp
    src/emotion_detection.cpp
    src/model_factory.cpp
    src/api_handler.cpp
    src/main.cpp
)

# ═════════════════════════════════════════════════════════════════════════════
# EXECUTABLE TARGET
# ═════════════════════════════════════════════════════════════════════════════

add_executable(${PROJECT_NAME} ${SV_CORE_SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Linked libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Threads::Threads
)

# Platform-specific linking
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()

# ═════════════════════════════════════════════════════════════════════════════
# COMPILER CONFIGURATION
# ═════════════════════════════════════════════════════════════════════════════

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
        /permissive-
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wpedantic
        -Wshadow -Wconversion
    )
endif()


