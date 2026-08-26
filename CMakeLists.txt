cmake_minimum_required(VERSION 3.24)

project(goreecloud_browser
  VERSION 0.1.0
  DESCRIPTION "GoreeCloud first-party native browser"
  LANGUAGES CXX
)

include(CTest)

option(GOREECLOUD_ENABLE_CHROMIUM
  "Build the GoreeCloud Chromium engine adapter integration target"
  OFF
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

function(goreecloud_apply_warnings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive-)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

if(GOREECLOUD_ENABLE_CHROMIUM)
  add_library(goreecloud_browser_chromium STATIC
    src/chromium_adapter.cpp
  )
  target_link_libraries(goreecloud_browser_chromium
    PUBLIC
      goreecloud_browser_core
  )
  target_compile_definitions(goreecloud_browser_chromium
    PUBLIC
      GOREECLOUD_ENABLE_CHROMIUM=1
  )
  goreecloud_apply_warnings(goreecloud_browser_chromium)
endif()

add_executable(goreecloud_browser
  apps/browser/main.cpp
)

target_link_libraries(goreecloud_browser
  PRIVATE
    goreecloud_browser_core
)

if(GOREECLOUD_ENABLE_CHROMIUM)
  target_link_libraries(goreecloud_browser PRIVATE goreecloud_browser_chromium)
  target_compile_definitions(goreecloud_browser PRIVATE GOREECLOUD_ENABLE_CHROMIUM=1)
else()
  target_compile_definitions(goreecloud_browser PRIVATE GOREECLOUD_ENABLE_CHROMIUM=0)
endif()

goreecloud_apply_warnings(goreecloud_browser)

add_executable(goreecloud_browser_runtime_smoke
  apps/browser/runtime_smoke.cpp
)

target_link_libraries(goreecloud_browser_runtime_smoke
  PRIVATE
    goreecloud_browser_core
)

goreecloud_apply_warnings(goreecloud_browser_runtime_smoke)

if(GOREECLOUD_ENABLE_CHROMIUM)
  add_executable(goreecloud_browser_chromium_smoke
    apps/browser/chromium_adapter_smoke.cpp
  )
  target_link_libraries(goreecloud_browser_chromium_smoke
    PRIVATE
      goreecloud_browser_core
      goreecloud_browser_chromium
  )
  goreecloud_apply_warnings(goreecloud_browser_chromium_smoke)
endif()

if(BUILD_TESTING)
  add_test(NAME goreecloud_browser_runtime_smoke
           COMMAND goreecloud_browser_runtime_smoke)
  if(GOREECLOUD_ENABLE_CHROMIUM)
    add_test(NAME goreecloud_browser_chromium_smoke
             COMMAND goreecloud_browser_chromium_smoke)
  endif()
endif()
