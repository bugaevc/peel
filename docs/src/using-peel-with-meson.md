# Using peel with Meson

Many projects in the glib and GNOME ecosystem are built with [Meson]. peel
itself is built with Meson as well, and provides some integration to make it
easier to use peel in Meson projects.

Start by looking up the `peel` dependency:

```python
# meson.build

peel = dependency('peel')
```

The dependency can be installed system-wide, or provided by a [subproject],
perhaps coming from a [wrap file]. Specifically, to make peel just work as a
wrap-based subproject, place the following into `subprojects/peel.wrap`:

```ini
[wrap-git]
url=https://gitlab.gnome.org/bugaevc/peel.git
revision=HEAD
depth=1

[provide]
program_names=peel-gen
```

Meson famously [doesn't allow][meson-user-defined-functions] build scripts to
define custom functions[^matklad], which is why it's not possible for peel to
provide the same level of automation for Meson as it does
[for CMake](using-peel-with-cmake.md), particularly around code generation.

So to generate the bindings, we have to find the `peel-gen` program and define
a custom target manually:

```python
peel_gen = find_program('peel-gen')

peel_gtk = custom_target('peel-codegen',
  command: [peel_gen, '--recursive', 'Gtk', '4.0'],
  output: 'peel',
)
```

`output: 'peel'` here refers to the fact that the `peel-gen` invocation
generates a `peel/` directory.

You can then use the generated bindings as _sources_ in your target, while
passing the `peel` dependency object as a _dependency_:

```python
gtk = dependency('gtk4')

executable('example',
  'example.cpp',
  peel_gtk,
  dependencies: [gtk, peel],
)
```

Note that including the generated bindings as sources into your target does
not, by itself, cause your target to depend on the actual library you’re
generating bindings for; you still need to do that yourself.

A complete example:

```python
project('example', 'cpp')

gtk = dependency('gtk4')
peel = dependency('peel')

peel_gen = find_program('peel-gen')
peel_gtk = custom_target('peel-codegen',
  command: [peel_gen, '--recursive', 'Gtk', '4.0'],
  output: 'peel',
)

executable('example',
  'example.cpp',
  peel_gtk,
  dependencies: [gtk, peel],
)
```

[^matklad]: See also [Primitive Recursive Functions For A Working Programmer].

[Meson]: https://mesonbuild.com/
[subproject]: https://mesonbuild.com/Subprojects.html
[wrap file]: https://mesonbuild.com/Wrap-dependency-system-manual.html
[meson-user-defined-functions]: https://mesonbuild.com/FAQ.html#why-doesnt-meson-have-user-defined-functionsmacros
[Primitive Recursive Functions For A Working Programmer]: https://matklad.github.io/2024/08/01/primitive-recursive-functions.html
