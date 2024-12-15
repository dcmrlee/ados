# Copyright (C) 2024. All rights reserved.

include(FetchContent)

message(STATUS "get gflags ...")

set(gflags_DOWNLOAD_URL
    "https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"
    CACHE STRING "")

if(gflags_LOCAL_SOURCE)
  FetchContent_Declare(
    gflags
    SOURCE_DIR ${gflags_LOCAL_SOURCE}
    OVERRIDE_FIND_PACKAGE)
else()
  FetchContent_Declare(
    gflags
    URL ${gflags_DOWNLOAD_URL}
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    OVERRIDE_FIND_PACKAGE)
endif()

function(get_gflags)
  FetchContent_MakeAvailable(gflags)
  FetchContent_GetProperties(gflags)
  if(NOT gflags_POPULATED)
    set(BUILD_TESTING OFF)  
    add_subdirectory(${gflags_SOURCE_DIR} ${gflags_BINARY_DIR})  
  endif()
endfunction()

get_gflags()
