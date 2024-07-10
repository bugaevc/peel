cmake_minimum_required (VERSION 3.16)
project (cmake-example CXX)

add_compile_options ("-Wall" "-Wextra")

find_package (PkgConfig REQUIRED)
pkg_check_modules (gtk4 REQUIRED IMPORTED_TARGET gtk4)

find_package (peel REQUIRED)
peel_generate (Gtk 4.0 RECURSIVE)

add_executable (hello hello.cpp)

target_link_libraries (hello PRIVATE
  PkgConfig::gtk4
  peel::Gtk)

install (TARGETS hello)
