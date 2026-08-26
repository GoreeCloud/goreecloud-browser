cmake_minimum_required(VERSION 3.24)

project(goreecloud_browser
  VERSION 0.1.0
  DESCRIPTION "GoreeCloud first-party native browser"
  LANGUAGES CXX
)

add_library(goreecloud_browser_core INTERFACE)

target_include_directories(goreecloud_browser_core
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(goreecloud_browser_core INTERFACE cxx_std_20)

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
