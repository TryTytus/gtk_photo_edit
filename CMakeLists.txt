cmake_minimum_required(VERSION 3.20)

project(gtk_second VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
  if(EXISTS "/opt/homebrew/bin/pkg-config")
    set(PKG_CONFIG_EXECUTABLE "/opt/homebrew/bin/pkg-config" CACHE FILEPATH
        "pkg-config executable" FORCE)
  elseif(EXISTS "/usr/local/bin/pkg-config")
    set(PKG_CONFIG_EXECUTABLE "/usr/local/bin/pkg-config" CACHE FILEPATH
        "pkg-config executable" FORCE)
  endif()

  foreach(_prefix /opt/homebrew /usr/local /opt/local)
    if(EXISTS "${_prefix}")
      list(APPEND CMAKE_PREFIX_PATH "${_prefix}")
    endif()
  endforeach()

  set(_pkg_config_paths "")
  foreach(_prefix /opt/homebrew /usr/local /opt/local)
    foreach(_suffix lib/pkgconfig share/pkgconfig)
      if(EXISTS "${_prefix}/${_suffix}")
        list(APPEND _pkg_config_paths "${_prefix}/${_suffix}")
      endif()
    endforeach()
  endforeach()

  if(_pkg_config_paths)
    list(JOIN _pkg_config_paths ":" _joined_pkg_config_paths)
    if(DEFINED ENV{PKG_CONFIG_PATH} AND NOT "$ENV{PKG_CONFIG_PATH}" STREQUAL "")
      set(ENV{PKG_CONFIG_PATH} "${_joined_pkg_config_paths}:$ENV{PKG_CONFIG_PATH}")
    else()
      set(ENV{PKG_CONFIG_PATH} "${_joined_pkg_config_paths}")
    endif()
  endif()
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)
pkg_check_modules(VIPS REQUIRED IMPORTED_TARGET vips-cpp)

add_executable(gtk_second
  src/Filters.cpp
  src/sliders_frame.cpp
  src/image_frame.cpp
  src/hello_window.cpp
  src/main.cpp
)

target_link_libraries(gtk_second PRIVATE PkgConfig::GTKMM)
target_link_libraries(gtk_second PRIVATE PkgConfig::VIPS)
target_compile_options(gtk_second PRIVATE -Wall -Wextra -Wpedantic)
