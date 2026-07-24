cmake_minimum_required(VERSION 3.16)
project(SpectraVision)

# ═════════════════════════════════════════════════════════════════════════════
# ROOT PROJECT CONFIGURATION
# ═════════════════════════════════════════════════════════════════════════════
#
# This is the root CMakeLists.txt for the Spectra Vision platform.
# It orchestrates the entire multi-module build.
#
# Usage:
#   mkdir build && cd build
#   cmake -DCMAKE_BUILD_TYPE=Release -DSV_BUILD_TESTS=ON ..
#   cmake --build . --parallel
#   ctest
#   cmake --install . --prefix /usr/local
#

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Policy for modern CMake behavior
cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0077 NEW)

# Add cmake modules directory
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include helper functions
include(SpectraVisionHelpers)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ═════════════════════════════════════════════════════════════════════════════
# ROOT-LEVEL OPTIONS
# ═════════════════════════════════════════════════════════════════════════════

option(SV_BUILD_TESTS           "Build unit tests"                    ON)
option(SV_BUILD_DOCUMENTATION  "Generate documentation with Doxygen" OFF)
option(SV_ENABLE_SANITIZERS    "Enable AddressSanitizer + UBSan"     OFF)
option(SV_ENABLE_LTO           "Enable Link-Time Optimization"        OFF)
option(SV_VERBOSE_CMAKE        "Print detailed CMake messages"        OFF)

# ═════════════════════════════════════════════════════════════════════════════
# PROJECT INFORMATION
# ═════════════════════════════════════════════════════════════════════════════

set(PROJECT_VERSION 1.0.0)
set(PROJECT_DESCRIPTION "Enterprise computer vision platform with emotion, face, and object detection")
set(PROJECT_HOMEPAGE_URL "https://github.com/spectra/vision")

# ═════════════════════════════════════════════════════════════════════════════
# BUILD TYPE DEFAULTS
# ═════════════════════════════════════════════════════════════════════════════

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(SV_VERBOSE_CMAKE)
    message(STATUS "[SV] Build type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "[SV] C++ Standard: C++${CMAKE_CXX_STANDARD}")
    message(STATUS "[SV] Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
endif()

# ═════════════════════════════════════════════════════════════════════════════
# SUBDIRECTORIES
# ═════════════════════════════════════════════════════════════════════════════

add_subdirectory(backend)

if(SV_BUILD_DOCUMENTATION)
    find_package(Doxygen REQUIRED)
    add_subdirectory(docs)
endif()

# ═════════════════════════════════════════════════════════════════════════════
# SUMMARY & STATUS
# ═════════════════════════════════════════════════════════════════════════════

message(STATUS "")
message(STATUS "╔═════════════════════════════════════════════════════════════╗")
message(STATUS "║          Spectra Vision — Root Build Configuration         ║")
message(STATUS "╠═════════════════════════════════════════════════════════════╣")
message(STATUS "║ Version:            ${PROJECT_VERSION}")
message(STATUS "║ Build Type:         ${CMAKE_BUILD_TYPE}")
message(STATUS "║ C++ Standard:       C++${CMAKE_CXX_STANDARD}")
message(STATUS "║ Compiler:           ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "║                                                             ║")
message(STATUS "║ Options:                                                    ║")
message(STATUS "║  - Build Tests:     ${SV_BUILD_TESTS}")
message(STATUS "║  - Sanitizers:      ${SV_ENABLE_SANITIZERS}")
message(STATUS "║  - LTO:             ${SV_ENABLE_LTO}")
message(STATUS "║  - Documentation:   ${SV_BUILD_DOCUMENTATION}")
message(STATUS "╚═════════════════════════════════════════════════════════════╝")
message(STATUS "")
message(STATUS "Next steps:")
message(STATUS "  1. Build:  cmake --build . --parallel")
if(SV_BUILD_TESTS)
message(STATUS "  2. Test:   ctest --verbose")
endif()
message(STATUS "  3. Install: cmake --install . --prefix /usr/local")
message(STATUS "")
