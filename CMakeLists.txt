cmake_minimum_required(VERSION 3.24)

project(goreecloud_browser
  VERSION 0.1.0
  DESCRIPTION "GoreeCloud first-party native browser"
  LANGUAGES CXX
)

# GoreeCloud Browser follows the latest approved Stable Glaze UI release.
# This value is an acceptance baseline and must advance when Glaze UI promotes
# a newer Stable release; it is not permission to remain on a superseded release.
set(GOREECLOUD_GLAZE_UI_STABLE_VERSION "1.5.0" CACHE STRING
    "Current approved Stable Glaze UI version required by GoreeCloud Browser")
set(GOREECLOUD_GLAZE_UI_STABLE_REVISION
    "2e1618397f6ebcdd254a76bfdd7e98846f2c5aa3" CACHE STRING
    "Exact reviewed Glaze UI Stable source revision")

if(NOT GOREECLOUD_GLAZE_UI_STABLE_VERSION STREQUAL "1.5.0")
  message(FATAL_ERROR
    "GoreeCloud Browser source currently accepts Glaze UI Stable 1.5.0. "
    "Update the Browser Glaze adoption contract and acceptance evidence before changing the build baseline.")
endif()

add_library(goreecloud_browser_core INTERFACE)

target_include_directories(goreecloud_browser_core
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(goreecloud_browser_core INTERFACE cxx_std_20)

target_compile_definitions(goreecloud_browser_core INTERFACE
  GOREECLOUD_GLAZE_UI_STABLE_VERSION="${GOREECLOUD_GLAZE_UI_STABLE_VERSION}"
  GOREECLOUD_GLAZE_UI_STABLE_REVISION="${GOREECLOUD_GLAZE_UI_STABLE_REVISION}"
)

add_executable(goreecloud_browser
  apps/browser/main.cpp
)

target_link_libraries(goreecloud_browser
  PRIVATE
    goreecloud_browser_core
)

if(MSVC)
  target_compile_options(goreecloud_browser PRIVATE /W4 /permissive-)
else()
  target_compile_options(goreecloud_browser PRIVATE -Wall -Wextra -Wpedantic)
endif()
