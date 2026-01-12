# Using peel with CMake

Many C++ projects are built with [CMake]. peel provides a CMake package to make
it easier to use peel in CMake projects.

Start by importing the `peel` CMake package:

```python
# CMakeLists.txt

find_package (peel REQUIRED)
```

This will look for the file named `peel-config.cmake`, which peel installs into
`$libdir/cmake/peel/` (e.g. `/usr/lib64/cmake/peel/peel-config.cmake`). If you
have peel installed into a non-standard prefix, you can tell CMake to look
there by passing it as `CMAKE_PREFIX_PATH` value at project configuration time.
See the [CMake Using Dependencies Guide] for more details about how
`find_package` can be used, and where CMake looks for packages.

The `peel` CMake package defines the `peel::peel` [imported target], which you
can use when defining another target to add peel's own (non-generated) header
directory to the include path:

```python
find_package (peel REQUIRED)

add_executable (my-app main.cpp)
target_link_libraries (my-app PRIVATE peel::peel)
```

To run the bindings generator, use the `peel_generate` function:

```python
find_package (peel REQUIRED)
peel_generate (Gtk 4.0 RECURSIVE)
```

This function will, at build time, create a `peel-generated` directory inside
the [current build directory] and run the peel generator to generate bindings
inside of it. The function defines an imported target which is named same as
the first argument (the GIR repo name, so in this case, `peel::Gtk`); this
imported target can be linked into another target to add the `peel-generated`
directory to the include path, as well as to ensure a proper dependency edge
between generating bindings and compiling the dependent target.

Note that the peel imported target does not, by itself, link to the actual
library you're generating bindings for; you still need to do that yourself. So
a complete working example of using peel with GTK 4 might look like this:

```python
# Find GTK 4 using pkg-config:
find_package (PkgConfig)
pkg_check_modules (gtk4 IMPORTED_TARGET gtk4 REQUIRED)

# Find peel and generate bindings for GTK 4:
find_package (peel REQUIRED)
peel_generate (Gtk 4.0 RECURSIVE)

add_executable (my-app main.cpp)
# Build and link the executable with GTK 4,
# as well as peel bindings for GTK 4:
target_link_libraries (my-app PRIVATE PkgConfig::gtk4 peel::Gtk)
```

See [`examples/cmake-project`][example] in the peel repository for a small, yet
complete example of a CMake project that uses GTK 4 with peel.

[CMake]: https://cmake.org/
[CMake Using Dependencies Guide]: https://cmake.org/cmake/help/latest/guide/using-dependencies/
[imported target]: https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#imported-targets
[current build directory]: https://cmake.org/cmake/help/latest/variable/CMAKE_CURRENT_BINARY_DIR.html
[example]: https://gitlab.gnome.org/bugaevc/peel/-/tree/tmp/examples/cmake-project?ref_type=heads
