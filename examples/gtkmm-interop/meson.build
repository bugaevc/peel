project('gtkmm-interop', 'cpp',
  version: '0.1',
  default_options: ['warning_level=3'])

gtk = dependency('gtk4')
gtkmm = dependency('gtkmm-4.0')
peel = dependency('peel')

peel_gen = find_program('peel-gen')
peel_gtk = custom_target('peel-codegen',
  command: [peel_gen, '--recursive', 'Gtk', '4.0'],
  output: 'peel')

executable('main',
  'main.cpp',
  peel_gtk,
  dependencies: [gtk, gtkmm, peel],
  install: true)
