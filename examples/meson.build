add_languages('cpp', native: false)

glib = dependency('gobject-2.0')
gtk4 = dependency('gtk4')
libadwaita = dependency('libadwaita-1', required: false)
libsoup = dependency('libsoup-3.0', required: false)
libdex = dependency('libdex-1.0', required: false)

peel_gtk = custom_target('peel-gtk-codegen',
  command: [py, meson.current_source_dir() / '..' / 'peel-gen.py', '--recursive', 'Gtk', '4.0'],
  output: 'peel',
)

gtk_examples = [
  'gtk-builder',
  'hello-gtk',
  'list-model',
  'rotation-bin',
  'simple-button',
  'simple-label',
  'type-tree',
  'bind-property',
  'color-gradient',
  'dialogs',
  'file-browser',
  'scrollable-widget',
]

foreach example : gtk_examples
  executable(
    example,
    example + '.cpp',
    peel_gtk,
    dependencies: [gtk4, peel_dep],
    install: false
  )
endforeach
