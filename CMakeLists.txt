# Copyright (C) 2024. All rights reserved.

cmake_minimum_required(VERSION 3.24)

project(nxpilot LANGUAGES C CXX)

# Set cmake path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(CMakeDependentOption)

# Some necessary settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type as Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(INSTALL_CONFIG_NAME ${PROJECT_NAME}-config)

#set(CMAKE_CXX_VISIBILITY_PRESET hidden)
#set(CMAKE_CXX_VISIBLITY_INLINES_HIDDEN ON)

message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Include cmake module
include(NamespaceTool)
include(GetGFlags)
include(GetGTest)
include(GetYamlCpp)

# Add subdirectory
add_subdirectory(src)

