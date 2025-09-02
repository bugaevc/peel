cmake_file = configure_file(
  input: 'peel-config.cmake.in',
  output: 'peel-config.cmake',
  configuration: {
    'version': meson.project_version(),
    'includedir': get_option('prefix') / get_option('includedir'),
    'peel_gen_path': get_option('prefix') / get_option('bindir') / 'peel-gen',
  },
)
install_data(cmake_file,
  install_dir: get_option('libdir') / 'cmake' / 'peel',
)
