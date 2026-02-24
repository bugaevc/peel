from peel_dbus_gen.utils import camel_case_to_underscore, escape_cpp_name

class Method:
    def __init__(self, attrs, iface):
        self.iface = iface
        self.dbus_name = attrs.get('name')
        self.cpp_name = camel_case_to_underscore(self.dbus_name)
        self.cpp_name = escape_cpp_name(self.cpp_name)
        self.arguments = []
        self.annotations = dict()

    def resolve_stuff(self):
        self.in_args = ''.join(arg.generate_cpp_type() + ', ' for arg in self.arguments if arg.direction == 'in')
        self.in_args_forward = ''.join('std::move ({}), '.format(arg.cpp_name) for arg in self.arguments if arg.direction == 'in')
        self.out_args = ''.join(arg.generate_cpp_type() + ', ' for arg in self.arguments if arg.direction == 'out')
        self.out_args_forward = ''.join('{}, '.format(arg.cpp_name) for arg in self.arguments if arg.direction == 'out')

        self.no_reply = self.annotations.get('org.freedesktop.DBus.Method.NoReply', 'false') == 'true'
        if 'org.freedesktop.DBus.GLib.CSymbol' in self.annotations:
            self.cpp_name = escape_cpp_name(self.annotations['org.freedesktop.DBus.GLib.CSymbol'])
        self.unix_fd = bool(self.annotations.get('org.gtk.GDBus.C.UnixFD', None))
        if not self.unix_fd:
            self.unix_fd = any('h' in arg.type.signature for arg in self.arguments)

    def generate_header(self):
        return '\n'.join([
            '  template<typename AsyncReadyCallback>',
            '  void',
            '  {}_async ({}'.format(self.cpp_name, self.in_args.strip()),
            '    AsyncReadyCallback &&callback, ::peel::Gio::Cancellable *cancellable = nullptr,',
            '    ::peel::Gio::DBusCallFlags bus_call_flags = ::peel::Gio::DBusCallFlags::NONE, int bus_call_timeout_msec = -1) noexcept',
            '  {',
            '    gpointer _peel_user_data;',
            '    ::GAsyncReadyCallback _peel_callback = ::peel::internals::CallbackHelper<void, ::GObject *, ::GAsyncResult *>::wrap_async_callback (',
            '      static_cast<AsyncReadyCallback &&> (callback),',
            '      [] (::GObject *source_object, ::GAsyncResult *res, gpointer data) -> void',
            '      {',
            '        AsyncReadyCallback &callback = reinterpret_cast<AsyncReadyCallback &> (*reinterpret_cast<unsigned char *> (data));',
            '        ::peel::GObject::Object *_peel_source_object = reinterpret_cast<::peel::GObject::Object *> (source_object);',
            '        ::peel::Gio::AsyncResult *_peel_res = reinterpret_cast<::peel::Gio::AsyncResult *> (res);',
            '        peel::internals::invoke_if_nonnull<void> (static_cast<AsyncReadyCallback &&> (callback)) (_peel_source_object, _peel_res);',
            '      },',
            '      &_peel_user_data);',
            '    {}::Iface *_peel_iface = get_interface<{}> ();'.format(self.iface.emit_name, self.iface.emit_name),
            '    _peel_iface->_peel_vfunc_{}_async (this, {}_peel_callback, _peel_user_data, cancellable, bus_call_flags, bus_call_timeout_msec);'.format(self.cpp_name, self.in_args_forward),
            '  }',
            '',
            '  bool',
            '  {}_finish (::peel::Gio::AsyncResult *async_result, {}::peel::UniquePtr<::peel::GLib::Error> *error) noexcept'.format(self.cpp_name, self.out_args),
            '  {',
            '    {}::Iface *_peel_iface = get_interface<{}> ();'.format(self.iface.emit_name, self.iface.emit_name),
            '    return _peel_iface->_peel_vfunc_{}_finish (this, async_result, {}error);'.format(self.cpp_name, self.out_args_forward),
            '  }',
            '',
            '  bool',
            '  {}_sync ({}'.format(self.cpp_name, (self.in_args + self.out_args).strip()),
            '    ::peel::UniquePtr<::peel::GLib::Error> *error, ::peel::Gio::Cancellable *cancellable = nullptr,',
            '    ::peel::Gio::DBusCallFlags bus_call_flags = ::peel::Gio::DBusCallFlags::NONE, int bus_call_timeout_msec = -1) noexcept',
            '  {',
            '    {}::Iface *_peel_iface = get_interface<{}> ();'.format(self.iface.emit_name, self.iface.emit_name),
            '    return _peel_iface->_peel_vfunc_{}_sync (this, {}{}error, cancellable, bus_call_flags, bus_call_timeout_msec);'.format(self.cpp_name, self.in_args_forward, self.out_args_forward),
            '  }',
        ])

    def generate_vfunc_ptr(self):
        return '\n'.join([
            '    void (*_peel_vfunc_{}_async) ({} *self, {}'.format(self.cpp_name, self.iface.emit_name, self.in_args.strip()),
            '      ::GAsyncReadyCallback, gpointer user_data, ::peel::Gio::Cancellable *, ::peel::Gio::DBusCallFlags, int bus_call_timeout_msec);',
            '    bool (*_peel_vfunc_{}_finish) ({} *self, ::peel::Gio::AsyncResult *, {}'.format(self.cpp_name, self.iface.emit_name, self.out_args.strip()),
            '      ::peel::UniquePtr<::peel::GLib::Error> *);',
            '    bool (*_peel_vfunc_{}_sync) ({} *self, {}'.format(self.cpp_name, self.iface.emit_name, (self.in_args + self.out_args).strip()),
            '      ::peel::UniquePtr<::peel::GLib::Error> *, ::peel::Gio::Cancellable *, ::peel::Gio::DBusCallFlags, int bus_call_timeout_msec);',
        ])

    def generate_override_vfunc_method(self):
        return '\n'.join([
            '    template<typename DerivedClass>',
            '    void',
            '    override_vfunc_{}_async ()'.format(self.cpp_name),
            '    {',
            '      _peel_vfunc_{}_async = +[] ({} *self, {}'.format(self.cpp_name, self.iface.emit_name, self.in_args.strip()),
            '        ::GAsyncReadyCallback callback, gpointer user_data, ::peel::Gio::Cancellable *cancellable, ::peel::Gio::DBusCallFlags bus_call_flags, int bus_call_timeout_msec) -> void',
            '        {',
            '          DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);',
            '          _peel_this->DerivedClass::vfunc_{}_async ({}callback, user_data, cancellable, bus_call_flags, bus_call_timeout_msec);'.format(self.cpp_name, self.in_args_forward),
            '        };',
            '    }',
            '',
            '    template<typename DerivedClass>',
            '    void',
            '    override_vfunc_{}_finish ()'.format(self.cpp_name),
            '    {',
            '      _peel_vfunc_{}_finish = +[] ({} *self, ::peel::Gio::AsyncResult *async_result, {}'.format(self.cpp_name, self.iface.emit_name, self.out_args.strip()),
            '        ::peel::UniquePtr<::peel::GLib::Error> *error) -> bool',
            '        {',
            '          DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);',
            '          return _peel_this->DerivedClass::vfunc_{}_finish (async_result, {}error);'.format(self.cpp_name, self.out_args_forward),
            '        };',
            '    }',
            '',
            '    template<typename DerivedClass>',
            '    void',
            '    override_vfunc_{}_sync ()'.format(self.cpp_name),
            '    {',
            '      _peel_vfunc_{}_sync = +[] ({} *self, {}'.format(self.cpp_name, self.iface.emit_name, (self.in_args + self.out_args).strip()),
            '        ::peel::UniquePtr<::peel::GLib::Error> *error, ::peel::Gio::Cancellable *cancellable, ::peel::Gio::DBusCallFlags bus_call_flags, int bus_call_timeout_msec) -> bool',
            '        {',
            '          DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);',
            '          return _peel_this->DerivedClass::vfunc_{}_sync ({}{}error, cancellable, bus_call_flags, bus_call_timeout_msec);'.format(self.cpp_name, self.in_args_forward, self.out_args_forward),
            '        };',
            '    }',
        ])

    def generate_iface_init(self, iface_expr):
        return '\n'.join([
            '  {}->_peel_vfunc_{}_async = {}_default_{}_async;'.format(iface_expr, self.cpp_name, self.iface.emit_name, self.cpp_name),
            '  {}->_peel_vfunc_{}_finish = {}_default_{}_finish;'.format(iface_expr, self.cpp_name, self.iface.emit_name, self.cpp_name),
            '  {}->_peel_vfunc_{}_sync = {}_default_{}_sync;'.format(iface_expr, self.cpp_name, self.iface.emit_name, self.cpp_name),
        ])

    def generate_proxy_iface_init(self, iface_expr):
        return '\n'.join([
            '  {}->_peel_vfunc_{}_async = {}_proxy_{}_async;'.format(iface_expr, self.cpp_name, self.iface.emit_name, self.cpp_name),
            '  {}->_peel_vfunc_{}_finish = {}_proxy_{}_finish;'.format(iface_expr, self.cpp_name, self.iface.emit_name, self.cpp_name),
            '  {}->_peel_vfunc_{}_sync = {}_proxy_{}_sync;'.format(iface_expr, self.cpp_name, self.iface.emit_name, self.cpp_name),
        ])

    def generate_default_vfuncs(self):
        l = [
            'static void',
            '{}_default_{}_async ({} *self, {}'.format(self.iface.emit_name, self.cpp_name, self.iface.emit_name, self.in_args.strip()),
            '  ::GAsyncReadyCallback, gpointer user_data, ::peel::Gio::Cancellable *, ::peel::Gio::DBusCallFlags, int bus_call_timeout_msec);',
            'static bool',
            '{}_default_{}_finish ({} *self, ::peel::Gio::AsyncResult *, {}'.format(self.iface.emit_name, self.cpp_name, self.iface.emit_name, self.out_args.strip()),
            '  ::peel::UniquePtr<::peel::GLib::Error> *);',
            'static bool',
            '{}_default_{}_sync ({} *self, {}'.format(self.iface.emit_name, self.cpp_name, self.iface.emit_name, (self.in_args + self.out_args).strip()),
            '  ::peel::UniquePtr<::peel::GLib::Error> *, ::peel::Gio::Cancellable *, ::peel::Gio::DBusCallFlags, int bus_call_timeout_msec);',
            '',
            'struct {}{}Data'.format(self.iface.emit_name, self.dbus_name),
            '{',
        ]
        for arg in self.arguments:
            l.append('  {};'.format(arg.generate_cpp_type(no_out_asterisk=True)))
        l.extend([
            '  ::peel::Gio::DBusCallFlags bus_call_flags;',
            '  int bus_call_timeout_msec;',
            '};',
            '',
            'static void',
            '{}_{}_data_free (gpointer d)'.format(self.iface.emit_name, self.cpp_name),
            '{',
            '  {}{}Data *data = reinterpret_cast<{}{}Data *> (d);'.format(self.iface.emit_name, self.dbus_name, self.iface.emit_name, self.dbus_name),
            '  data->~{}{}Data ();'.format(self.iface.emit_name, self.dbus_name),
            '  g_slice_free ({}{}Data, d);'.format(self.iface.emit_name, self.dbus_name),
            '}',
            '',
            'static void',
            '{}_default_{}_async ({} *self, {}'.format(self.iface.emit_name, self.cpp_name, self.iface.emit_name, self.in_args.strip()),
            '  ::GAsyncReadyCallback callback, gpointer user_data, ::peel::Gio::Cancellable *cancellable, ::peel::Gio::DBusCallFlags bus_call_flags, int bus_call_timeout_msec)',
            '{',
            '  {}::Iface *_peel_iface = self->get_interface<{}> ();'.format(self.iface.emit_name, self.iface.emit_name),
            '  if (G_UNLIKELY (_peel_iface->_peel_vfunc_{}_sync == {}_default_{}_sync))'.format(self.cpp_name, self.iface.emit_name, self.cpp_name),
            '    {',
            '      g_critical ("{} of type \'%s\' does not implement {}::{}", self->get_type_name ());'.format(self.iface.emit_name, self.iface.emit_name, self.cpp_name),
            '      return;',
            '    }',
            '',
            '  {}{}Data *_peel_task_data = g_slice_new ({}{}Data);'.format(self.iface.emit_name, self.dbus_name, self.iface.emit_name, self.dbus_name),
            '  _peel_task_data->bus_call_flags = bus_call_flags;',
            '  _peel_task_data->bus_call_timeout_msec = bus_call_timeout_msec;',
        ])
        for arg in self.arguments:
            if arg.direction == 'in':
                l.append('  _peel_task_data->{} = std::move ({});'.format(arg.cpp_name, arg.cpp_name))
        l.extend([
            '  ::GTask *_peel_task = g_task_new (self, reinterpret_cast<::GCancellable *> (cancellable), callback, user_data);',
            '  (g_task_set_source_tag) (_peel_task, (gpointer) {}_default_{}_async);'.format(self.iface.emit_name, self.cpp_name),
            '#if GLIB_CHECK_VERSION (2, 76, 0)',
            '  g_task_set_static_name (_peel_task, "{}::{}");'.format(self.iface.emit_name, self.cpp_name),
            '#endif',
            '  g_task_set_task_data (_peel_task, _peel_task_data, {}_{}_data_free);'.format(self.iface.emit_name, self.cpp_name),
            '  g_task_run_in_thread (_peel_task, +[] (::GTask *task, gpointer source_object, gpointer d, ::GCancellable *cancellable)',
            '    {',
            '      {} *self = reinterpret_cast<{} *> (source_object);'.format(self.iface.emit_name, self.iface.emit_name),
            '      {}{}Data *task_data = reinterpret_cast<{}{}Data *> (d);'.format(self.iface.emit_name, self.dbus_name, self.iface.emit_name, self.dbus_name),
            '      ::peel::UniquePtr<::peel::GLib::Error> error;',
            '      ::peel::Gio::Cancellable *_peel_cancellable = reinterpret_cast<::peel::Gio::Cancellable *> (cancellable);',
            '      {}::Iface *iface = self->get_interface<{}> ();'.format(self.iface.emit_name, self.iface.emit_name),
            '      bool _peel_return = iface->_peel_vfunc_{}_sync (self, {}&error, _peel_cancellable, task_data->bus_call_flags, task_data->bus_call_timeout_msec);'.format(
                self.cpp_name,
                ''.join(
                    'std::move (task_data->{}), '.format(arg.cpp_name) if arg.direction == 'in' else '&task_data->{}, '.format(arg.cpp_name)
                    for arg in self.arguments
                )
            ),
            '      if (error)',
            '        g_task_return_error (task, reinterpret_cast<::GError *> (std::move (error).release_ref ()));',
            '      else',
            '        g_task_return_boolean (task, _peel_return);',
            '    });',
            '  g_object_unref (_peel_task);',
            '}',
            '',
            'static bool',
            '{}_default_{}_finish ({} *self, ::peel::Gio::AsyncResult *async_result, {}'.format(self.iface.emit_name, self.cpp_name, self.iface.emit_name, self.out_args.strip()),
            '  ::peel::UniquePtr<::peel::GLib::Error> *error)',
            '{',
            '  g_return_val_if_fail (::peel::Gio::Task::is_valid (async_result, self), false);',
            '  ::peel::Gio::Task *_peel_task = async_result->cast<::peel::Gio::Task> ();',
            '  g_return_val_if_fail (_peel_task->get_source_tag () == {}_default_{}_async, false);'.format(self.iface.emit_name, self.cpp_name),
        ])
        if any(arg.direction == 'out' for arg in self.arguments):
            l.extend([
                '  if (!_peel_task->had_error ())',
                '    {',
                '      {}{}Data *_peel_task_data = reinterpret_cast<{}{}Data *> (_peel_task->get_task_data ());'.format(
                    self.iface.emit_name,
                    self.dbus_name,
                    self.iface.emit_name,
                    self.dbus_name,
                ),
            ])
            for arg in self.arguments:
                if arg.direction == 'out':
                    l.append('      *{} = std::move (_peel_task_data->{});'.format(arg.cpp_name, arg.cpp_name))
            l.append('    }'),
        l.extend([
            '  return _peel_task->propagate_boolean (error);',
            '}',
            '',
            'static bool',
            '{}_default_{}_sync ({} *self, {}'.format(self.iface.emit_name, self.cpp_name, self.iface.emit_name, (self.in_args + self.out_args).strip()),
            '  ::peel::UniquePtr<::peel::GLib::Error> *error, ::peel::Gio::Cancellable *cancellable, ::peel::Gio::DBusCallFlags bus_call_flags, int bus_call_timeout_msec)',
            '{',
            '  {}::Iface *iface = self->get_interface<{}> ();'.format(self.iface.emit_name, self.iface.emit_name),
            '  if (G_UNLIKELY (iface->_peel_vfunc_{}_async == {}_default_{}_async))'.format(self.cpp_name, self.iface.emit_name, self.cpp_name),
            '    {',
            '      g_critical ("{} of type \'%s\' does not implement {}::{}", self->get_type_name ());'.format(self.iface.emit_name, self.iface.emit_name, self.cpp_name),
            '      return false;',
            '    }',
            '',
            '  bool _peel_completed = false;',
            '  bool _peel_result;',
            '  ::peel::RefPtr<::peel::GLib::MainContext> _peel_main_context = ::peel::GLib::MainContext::create ();',
            '  _peel_main_context->push_thread_default ();',
            '  self->{}_async ({}[{}error, &_peel_completed, &_peel_result] (::peel::GObject::Object *_peel_source_object, ::peel::Gio::AsyncResult *_peel_res)'.format(self.cpp_name, self.in_args_forward, self.out_args_forward),
            '    {',
            '      _peel_result = _peel_source_object->cast<{}> ()->{}_finish (_peel_res, {}error);'.format(self.iface.emit_name, self.cpp_name, self.out_args_forward),
            '      _peel_completed = true;',
            '    }, cancellable, bus_call_flags, bus_call_timeout_msec);',
            '  while (!_peel_completed)',
            '    _peel_main_context->iteration (true);',
            '  _peel_main_context->pop_thread_default ();',
            '  return _peel_result;',
            '}',
        ])
        return '\n'.join(l)

    def generate_proxy_vfuncs(self):
        num_in_args = len([arg for arg in self.arguments if arg.direction == 'in'])
        in_args = ',\n'.join('      ' + arg.generate_make_variant() for arg in self.arguments if arg.direction == 'in')

        out_args = []
        index = 0
        for arg in self.arguments:
            if arg.direction != 'out':
                continue
            out_args.extend([
              '  _peel_out_arg = g_variant_get_child_value (_peel_return, {});'.format(index),
              '  ' + arg.generate_set_from_variant('_peel_out_arg') + ';',
              '  g_variant_unref (_peel_out_arg);',
            ])
            index += 1
        out_args = '\n'.join(out_args)
        l = [
            'static void',
            '{}_proxy_{}_async ({} *self, {}'.format(self.iface.emit_name, self.cpp_name, self.iface.emit_name, self.in_args.strip()),
            '  ::GAsyncReadyCallback callback, gpointer user_data, ::peel::Gio::Cancellable *cancellable, ::peel::Gio::DBusCallFlags bus_call_flags, int bus_call_timeout_msec)',
            '{',
            '  ::GDBusProxy *proxy = G_DBUS_PROXY (self);',
            '  ::GVariant *_peel_args[{}]'.format(num_in_args),
            '    {',
            in_args,
            '    };',
            '  g_dbus_proxy_call (proxy, "{}", g_variant_new_tuple (_peel_args, {}),'.format(self.dbus_name, num_in_args),
            '    static_cast<::GDBusCallFlags> (bus_call_flags), bus_call_timeout_msec, reinterpret_cast<::GCancellable *> (cancellable),',
        ]
        if not self.no_reply:
            l.append('    callback, user_data);')
        else:
            l.extend([
                '    nullptr, nullptr);',
                '  /* Complete immediately */',
                '  ::GTask *_peel_task = g_task_new (self, reinterpret_cast<::GCancellable *> (cancellable), callback, user_data);',
                '  (g_task_set_source_tag) (_peel_task, (gpointer) {}_proxy_{}_async);'.format(self.iface.emit_name, self.cpp_name),
                '#if GLIB_CHECK_VERSION (2, 76, 0)',
                '  g_task_set_static_name (_peel_task, "{}::Proxy::{}");'.format(self.iface.emit_name, self.cpp_name),
                '#endif',
                '  g_task_return_boolean (_peel_task, true);',
                '  g_object_unref (_peel_task);',
            ])
        l.extend([
            '}',
            '',
            'static bool',
            '{}_proxy_{}_finish ({} *self, ::peel::Gio::AsyncResult *async_result, {}'.format(self.iface.emit_name, self.cpp_name, self.iface.emit_name, self.out_args.strip()),
            '  ::peel::UniquePtr<::peel::GLib::Error> *error)',
            '{',
        ])
        if not self.no_reply:
            l.extend([
                '  ::GDBusProxy *proxy = G_DBUS_PROXY (self);',
                '  ::GError *_peel_error = nullptr;',
                '  ::GVariant *_peel_return = g_dbus_proxy_call_finish (proxy, reinterpret_cast<::GAsyncResult *> (async_result), error ? &_peel_error : nullptr);',
                '  if (!_peel_return)',
                '    {',
                '      if (error)',
                '        *error = ::peel::UniquePtr<::peel::GLib::Error>::adopt_ref (reinterpret_cast<::peel::GLib::Error *> (_peel_error));',
                '      return false;',
                '    }',
                '  if (error)',
                '    *error = nullptr;',
                '  ::GVariant *_peel_out_arg;',
                out_args,
                '  g_variant_unref (_peel_return);',
                '  return true;',
            ])
        else:
            l.extend([
                '  g_return_val_if_fail (::peel::Gio::Task::is_valid (async_result, self), false);',
                '  ::peel::Gio::Task *_peel_task = async_result->cast<::peel::Gio::Task> ();',
                '  g_return_val_if_fail (_peel_task->get_source_tag () == {}_proxy_{}_async, false);'.format(self.iface.emit_name, self.cpp_name),
                '  return _peel_task->propagate_boolean (error);',
            ])
        l.extend([
            '}',
            '',
            'static bool',
            '{}_proxy_{}_sync ({} *self, {}'.format(self.iface.emit_name, self.cpp_name, self.iface.emit_name, (self.in_args + self.out_args).strip()),
            '  ::peel::UniquePtr<::peel::GLib::Error> *error, ::peel::Gio::Cancellable *cancellable, ::peel::Gio::DBusCallFlags bus_call_flags, int bus_call_timeout_msec)',
            '{',
            '  ::GDBusProxy *proxy = G_DBUS_PROXY (self);',
            '  ::GVariant *_peel_args[{}]'.format(num_in_args),
            '    {',
            in_args,
            '    };',
            '  ::GError *_peel_error = nullptr;',
            '  ::GVariant *_peel_return = g_dbus_proxy_call_sync (proxy, "{}", g_variant_new_tuple (_peel_args, {}),'.format(self.dbus_name, num_in_args),
            '    static_cast<::GDBusCallFlags> (bus_call_flags), bus_call_timeout_msec, reinterpret_cast<::GCancellable *> (cancellable), error ? &_peel_error : nullptr);',
            '  if (!_peel_return)',
            '    {',
            '      if (error)',
            '        *error = ::peel::UniquePtr<::peel::GLib::Error>::adopt_ref (reinterpret_cast<::peel::GLib::Error *> (_peel_error));',
            '      return false;',
            '    }',
            '  if (error)',
            '    *error = nullptr;',
            '  ::GVariant *_peel_out_arg;',
            out_args,
            '  g_variant_unref (_peel_return);',
            '  return true;',
            '}',
        ])
        return '\n'.join(l)

    def generate_method_info(self):
        in_arg_infos = []
        out_arg_infos = []
        l = []
        for arg in self.arguments:
            name, s = arg.generate_arg_info()
            l.append(s)
            if arg.direction == 'in':
                in_arg_infos.append('  &{},'.format(name))
            else:
                out_arg_infos.append('  &{},'.format(name))
        name = '{}_{}_method_info'.format(self.iface.emit_name, self.cpp_name)
        l.extend([
            'static const ::GDBusArgInfo * const',
            '{}_{}_in_arg_infos[] ='.format(self.iface.emit_name, self.cpp_name),
            '{',
            '\n'.join(in_arg_infos),
            '  nullptr',
            '};',
            ''
            'static const ::GDBusArgInfo * const',
            '{}_{}_out_arg_infos[] ='.format(self.iface.emit_name, self.cpp_name),
            '{',
            '\n'.join(out_arg_infos),
            '  nullptr',
            '};',
            ''
            'static const ::GDBusMethodInfo',
            '{} ='.format(name),
            '{',
            '  -1, /* ref_count */',
            '  const_cast<char *> ("{}"),'.format(self.dbus_name),
            '  const_cast<::GDBusArgInfo **> ({}_{}_in_arg_infos),'.format(self.iface.emit_name, self.cpp_name),
            '  const_cast<::GDBusArgInfo **> ({}_{}_out_arg_infos),'.format(self.iface.emit_name, self.cpp_name),
            '  nullptr /* annotations */',
            '};',
        ])
        return name, '\n'.join(l)

    def generate_vtable_method(self):
        in_args = [arg for arg in self.arguments if arg.direction == 'in']
        num_in_args = len(in_args)
        out_args = [arg for arg in self.arguments if arg.direction == 'out']
        num_out_args = len(out_args)
        l = [
            'static void',
            '{}_vtable_method_{}_ready_cb (::GObject *_peel_source_object, ::GAsyncResult *_peel_res, gpointer _peel_user_data)'.format(
                self.iface.emit_name,
                self.cpp_name,
            ),
            '{',
            '  g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (_peel_source_object, ::peel::Type::of<{}> ()));'.format(self.iface.emit_name),
            '  {} *self = reinterpret_cast<{} *> (_peel_source_object);'.format(
                self.iface.emit_name,
                self.iface.emit_name,
            ),
            '  {}::Iface *_peel_iface = G_TYPE_INSTANCE_GET_INTERFACE (self, ::peel::Type::of<{}> (), {}::Iface);'.format(
                self.iface.emit_name,
                self.iface.emit_name,
                self.iface.emit_name,
            ),
            '  ::GDBusMethodInvocation *_peel_invocation = G_DBUS_METHOD_INVOCATION (_peel_user_data);',
        ]
        for arg in out_args:
            l.append('  {};'.format(arg.generate_cpp_type(no_out_asterisk=True)))
        l.extend([
            '  ::peel::UniquePtr<::peel::GLib::Error> _peel_error;',
            '  _peel_iface->_peel_vfunc_{}_finish (self, reinterpret_cast<::peel::Gio::AsyncResult *> (_peel_res), {}&_peel_error);'.format(
                self.cpp_name,
                ''.join('&{}, '.format(arg.cpp_name) for arg in out_args),
            ),
            '  if (_peel_error)',
            '    {',
            '#if GLIB_CHECK_VERSION (2, 30, 0)',
            '      g_dbus_method_invocation_take_error (_peel_invocation, reinterpret_cast<::GError *> (std::move (_peel_error).release_ref ()));',
            '#else',
            '      g_dbus_method_invocation_return_gerror (_peel_invocation, reinterpret_cast<::GError *> (static_cast<::peel::GLib::Error *> (_peel_error)));',
            '#endif',
            '      return;',
            '    }',
            '  ::GVariant *_peel_out_args[{}];'.format(num_out_args),
        ])
        for i, arg in enumerate(out_args):
            l.append('  _peel_out_args[{}] = {};'.format(i, arg.generate_make_variant()))
        l.extend([
            '  g_dbus_method_invocation_return_value (_peel_invocation, g_variant_new_tuple (_peel_out_args, {}));'.format(num_out_args),
            '}',
            '',
            'static void',
            '{}_vtable_method_{} ({} *self, ::GVariant *_peel_parameters, ::GDBusMethodInvocation *_peel_invocation)'.format(
                self.iface.emit_name,
                self.cpp_name,
                self.iface.emit_name,
            ),
            '{',
            '  ::GVariant *_peel_in_args[{}];'.format(num_in_args),
        ])
        for i, arg in enumerate(in_args):
            arg_expr = '_peel_in_args[{}]'.format(i)
            l.extend([
                '  {} = g_variant_get_child_value (_peel_parameters, {});'.format(arg_expr, i),
                '  {} = {};'.format(arg.generate_cpp_type(), arg.type.generate_variant_get(arg_expr)),
            ])
        l.extend([
            '  ::GDBusMessageFlags _peel_message_flags = g_dbus_message_get_flags (g_dbus_method_invocation_get_message (_peel_invocation));',
            '  bool _peel_no_reply = _peel_message_flags & G_DBUS_MESSAGE_FLAGS_NO_REPLY_EXPECTED;',
            '  ::peel::Gio::DBusCallFlags _peel_call_flags = ::peel::Gio::DBusCallFlags::NONE;',
            '#if GLIB_CHECK_VERSION (2, 46, 0)',
            '  if (_peel_message_flags & G_DBUS_MESSAGE_FLAGS_ALLOW_INTERACTIVE_AUTHORIZATION)',
            '    _peel_call_flags |= ::peel::Gio::DBusCallFlags::ALLOW_INTERACTIVE_AUTHORIZATION;',
            '#endif',
            '  {}::Iface *_peel_iface = G_TYPE_INSTANCE_GET_INTERFACE (self, ::peel::Type::of<{}> (), {}::Iface);'.format(
                self.iface.emit_name,
                self.iface.emit_name,
                self.iface.emit_name,
            ),
            '  _peel_iface->_peel_vfunc_{}_async (self, {}_peel_no_reply ? nullptr : {}_vtable_method_{}_ready_cb, _peel_invocation, nullptr, _peel_call_flags, -1);'.format(
                self.cpp_name,
                self.in_args_forward,
                self.iface.emit_name,
                self.cpp_name,
            ),
            '  for (::GVariant *_peel_in_arg : _peel_in_args)',
            '    g_variant_unref (_peel_in_arg);',
            '  if (_peel_no_reply)',
            '    g_object_unref (_peel_invocation);',
            '}',
        ])
        return '\n'.join(l)
