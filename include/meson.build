inc = include_directories('.')

static_headers = files(
  'peel/lang.h',
  'peel/class.h',
  'peel/enum.h',
  'peel/property.h',
  'peel/signal.h',
  'peel/callback.h',
  'peel/widget-template.h',

  'peel/UniquePtr.h',
  'peel/ArrayRef.h',
  'peel/RefPtr.h',
  'peel/FloatPtr.h',
  'peel/WeakPtr.h',
  'peel/String.h',

  'peel/coro/SimpleTask.h',
  'peel/coro/AsyncResult.h',

  'peel/GObject/EnumClass.h',
  'peel/GObject/FlagsClass.h',
  'peel/GObject/Type.h',
  'peel/GObject/TypeInstance.h',
  'peel/GObject/TypeClass.h',
  'peel/GObject/TypeInterface.h',
  'peel/GObject/Value.h',
  'peel/GObject/Object.h',
  'peel/GObject/ObjectConstructParam.h',
  'peel/GLib/Variant.h',
)

if not meson.is_subproject()
  install_headers(static_headers, preserve_path: true)
endif
