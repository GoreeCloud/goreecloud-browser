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
option(GOREECLOUD_ENABLE_CEF
  "Build the CEF-backed Chromium runtime path"
  OFF
)

set(GOREECLOUD_CEF_ROOT "" CACHE PATH
    "Path to an unpacked CEF binary distribution")

if(GOREECLOUD_ENABLE_CEF AND NOT GOREECLOUD_ENABLE_CHROMIUM)
  message(FATAL_ERROR "GOREECLOUD_ENABLE_CEF requires GOREECLOUD_ENABLE_CHROMIUM=ON")
endif()

# GoreeCloud Browser follows the latest approved Stable Glaze UI release.
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
target_include_directories(goreecloud_browser_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
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
    src/chromium_runtime_scaffold.cpp
  )
  target_link_libraries(goreecloud_browser_chromium PUBLIC goreecloud_browser_core)
  target_compile_definitions(goreecloud_browser_chromium PUBLIC GOREECLOUD_ENABLE_CHROMIUM=1)

  if(GOREECLOUD_ENABLE_CEF)
    if(GOREECLOUD_CEF_ROOT STREQUAL "")
      message(FATAL_ERROR "GOREECLOUD_ENABLE_CEF requires GOREECLOUD_CEF_ROOT")
    endif()
    if(NOT EXISTS "${GOREECLOUD_CEF_ROOT}/include/cef_app.h")
      message(FATAL_ERROR "GOREECLOUD_CEF_ROOT does not contain include/cef_app.h")
    endif()

    target_sources(goreecloud_browser_chromium PRIVATE src/cef_runtime.cpp)
    target_include_directories(goreecloud_browser_chromium PRIVATE ${GOREECLOUD_CEF_ROOT})
    target_compile_definitions(goreecloud_browser_chromium PUBLIC GOREECLOUD_ENABLE_CEF=1)
  else()
    target_compile_definitions(goreecloud_browser_chromium PUBLIC GOREECLOUD_ENABLE_CEF=0)
  endif()

  goreecloud_apply_warnings(goreecloud_browser_chromium)
endif()

add_executable(goreecloud_browser
  apps/browser/main.cpp
  src/cef_process.cpp
)
target_link_libraries(goreecloud_browser PRIVATE goreecloud_browser_core)

if(GOREECLOUD_ENABLE_CHROMIUM)
  target_link_libraries(goreecloud_browser PRIVATE goreecloud_browser_chromium)
  target_compile_definitions(goreecloud_browser PRIVATE GOREECLOUD_ENABLE_CHROMIUM=1)
  if(GOREECLOUD_ENABLE_CEF)
    target_include_directories(goreecloud_browser PRIVATE ${GOREECLOUD_CEF_ROOT})
  endif()
else()
  target_compile_definitions(goreecloud_browser PRIVATE GOREECLOUD_ENABLE_CHROMIUM=0 GOREECLOUD_ENABLE_CEF=0)
endif()

goreecloud_apply_warnings(goreecloud_browser)

add_executable(goreecloud_browser_runtime_smoke apps/browser/runtime_smoke.cpp)
target_link_libraries(goreecloud_browser_runtime_smoke PRIVATE goreecloud_browser_core)
goreecloud_apply_warnings(goreecloud_browser_runtime_smoke)

if(GOREECLOUD_ENABLE_CHROMIUM)
  add_executable(goreecloud_browser_chromium_smoke apps/browser/chromium_smoke.cpp)
  target_link_libraries(goreecloud_browser_chromium_smoke PRIVATE goreecloud_browser_core goreecloud_browser_chromium)
  goreecloud_apply_warnings(goreecloud_browser_chromium_smoke)
endif()

if(BUILD_TESTING)
  add_test(NAME goreecloud_browser_runtime_smoke COMMAND goreecloud_browser_runtime_smoke)
  if(GOREECLOUD_ENABLE_CHROMIUM)
    add_test(NAME goreecloud_browser_chromium_smoke COMMAND goreecloud_browser_chromium_smoke)
  endif()
endif()
