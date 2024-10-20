project('peel', version: '0.1.0')

pkg = import('pkgconfig')
py = import('python').find_installation('python3')

subdir('include')
subdir('peel_gen')
subdir('tests')
subdir('cmake')

peel_dep = declare_dependency(include_directories: inc)

peel_gen = configure_file(
  input: 'peel-gen.py',
  output: 'peel-gen',
  install: not meson.is_subproject(),
  install_dir: get_option('bindir'),
  configuration: {
    'module_path': py.get_install_dir(),
    'api_tweaks_path': get_option('prefix') / get_option('datadir') / 'peel' / 'api-tweaks.txt',
  },
)

if meson.is_subproject()
  meson.override_dependency('peel', peel_dep)
  meson.override_find_program('peel-gen', peel_gen)
else
  py.install_sources(peel_gen_files, subdir: 'peel_gen')
  install_data('api-tweaks.txt')
endif

pkg.generate(
  name: 'peel',
  version : '0.1.0',
  description: 'C++ bindings for GObject-based libraries',
  filebase: 'peel',
)
