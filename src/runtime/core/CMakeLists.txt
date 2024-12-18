# Copyright (C) 2024. All rights reserved.

# Get the current folder name
string(REGEX REPLACE ".*/(.*)" "\\1" CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Get namespace
get_namespace(CUR_SUPERIOR_NAMESPACE)
string(REPLACE "::" "_" CUR_SUPERIOR_NAMESPACE_UNDERLINE ${CUR_SUPERIOR_NAMESPACE})

# Set target name
set(CUR_TARGET_NAME ${CUR_SUPERIOR_NAMESPACE_UNDERLINE}_${CUR_DIR})
set(CUR_TARGET_ALIAS_NAME ${CUR_SUPERIOR_NAMESPACE}::${CUR_DIR})

# Set file collection
file(GLOB_RECURSE head_files ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Add target
add_library(${CUR_TARGET_NAME} STATIC)
add_library(${CUR_TARGET_ALIAS_NAME} ALIAS ${CUR_TARGET_NAME})

# Set source files of target
target_sources(${CUR_TARGET_NAME} PRIVATE ${src})

# Set head files of target
target_sources(${CUR_TARGET_NAME} INTERFACE FILE_SET HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES ${head_files})

# Set include path of target
target_include_directories(
  ${CUR_TARGET_NAME}
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/>)

# Set link libraries of target
target_link_libraries(
  ${CUR_TARGET_NAME}
  PUBLIC nxpilot::utils::common
         yaml-cpp::yaml-cpp
)

# Add -Werror option
include(AddWerror)
add_werror(${CUR_TARGET_NAME})

get_filename_component(ABSOLUTE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE} ABSOLUTE)
file(RELATIVE_PATH RELATIVE_PATH ${CMAKE_SOURCE_DIR} ${ABSOLUTE_PATH})
target_compile_definitions(${CUR_TARGET_NAME} PRIVATE "SOURCE_FILE=${RELATIVE_PATH}")

if(NXPILOT_INSTALL)
  set_property(TARGET ${CUR_TARGET_NAME} PROPERTY EXPORT_NAME ${CUR_TARGET_ALIAS_NAME})
  string(REPLACE "::" "/" INSTALL_PATH ${CUR_TARGET_ALIAS_NAME})
  install(
    TARGETS ${CUR_TARGET_NAME}
    EXPORT ${INSTALL_CONFIG_NAME}
    LIBRARY DESTINATION lib
    FILE_SET HEADERS DESTINATION include/${INSTALL_PATH})
endif()


