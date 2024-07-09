cmake_minimum_required (VERSION 3.16)
project (cmake-example CXX)

add_compile_options ("-Wall" "-Wextra")
# Enable colored compiler diagnostics, even when using Ninja.
if (${CMAKE_VERSION} VERSION_LESS "3.24")
  add_compile_options ("-fdiagnostics-color=always")
else ()
  set (CMAKE_COLOR_DIAGNOSTICS ON)
endif ()

find_package (PkgConfig REQUIRED)
pkg_check_modules (gtk4 REQUIRED IMPORTED_TARGET gtk4)
# FIXME: This should not be required
pkg_check_modules (gio-unix REQUIRED IMPORTED_TARGET gio-unix-2.0)

find_package (peel REQUIRED)
peel_generate (Gtk 4.0 RECURSIVE)

add_executable (hello hello.cpp)

target_link_libraries (hello PRIVATE
  PkgConfig::gtk4
  PkgConfig::gio-unix
  peel::Gtk)

install (TARGETS hello)
