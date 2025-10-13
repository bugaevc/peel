add_languages('cpp', native: false)

glib = dependency('gobject-2.0')
gtk4 = dependency('gtk4')
libadwaita = dependency('libadwaita-1', version: '>= 1.5', required: false)
libsoup = dependency('libsoup-3.0', required: false)
libdex = dependency('libdex-1.0', required: false)

peel_gen_command = [
  py, meson.project_source_root() / 'peel-gen.py',
  '--recursive',
  '--out-dir', '@OUTDIR@',
  'Gtk', '4.0',
]

if libadwaita.found()
  peel_gen_command += ['Adw', '1']
endif

if libsoup.found()
  peel_gen_command += ['Soup', '3.0']
endif

if libdex.found()
  peel_gen_command += ['Dex', '1']
endif

peel_headers = custom_target('peel-codegen',
  command: peel_gen_command,
  output: 'peel',
)

glib_examples = [
  'simple-class',
  'properties-and-signals',
]
# TODO: add coro/async-result.cpp with C++20 / __cpp_impl_coroutine.

gtk_examples = [
  'gtk-builder',
  'hello-gtk',
  'list-model',
  'rotation-bin',
  'simple-button',
  'simple-label',
  'type-tree',
  'bind-property',
  'file-browser',
  'scrollable-widget',
]

adw_examples = ['adwaita']

soup_examples = ['soup']

dex_examples = ['libdex']

if gtk4.version().version_compare('>= 4.10')
  gtk_examples += [
    'color-gradient',
    'dialogs',
  ]
endif

foreach example : glib_examples
  executable(example,
    example + '.cpp',
    peel_headers,
    dependencies: [glib, peel_dep],
    install: false,
  )
endforeach

foreach example : gtk_examples
  executable(example,
    example + '.cpp',
    peel_headers,
    dependencies: [gtk4, peel_dep],
    install: false,
  )
endforeach

if libadwaita.found()
  foreach example : adw_examples
    executable(example,
      example + '.cpp',
      peel_headers,
      dependencies: [libadwaita, gtk4, peel_dep],
      install: false,
    )
  endforeach
endif

if libsoup.found()
  foreach example : soup_examples
    executable(example,
      example + '.cpp',
      peel_headers,
      dependencies: [libsoup, peel_dep],
      install: false,
    )
  endforeach
endif

if libdex.found()
  foreach example : dex_examples
    executable(example,
      example + '.cpp',
      peel_headers,
      dependencies: [libdex, peel_dep],
      install: false,
    )
  endforeach
endif
