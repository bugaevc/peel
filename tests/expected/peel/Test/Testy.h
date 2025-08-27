#pragma once

/* Auto-generated, do not modify */
/* Package peel-test */

#include <peel/GObject/Type.h>
#include <peel/RefPtr.h>
#include <peel/FloatPtr.h>
#include <peel/UniquePtr.h>
#include <peel/ArrayRef.h>
#include <peel/String.h>
#include <peel/signal.h>
#include <peel/callback.h>
#include <peel/property.h>
#include <peel/lang.h>
#include <cstdint>
#include <utility>
#include <peel-test/peel-test.h>
#include <peel/GObject/Object.h>

peel_begin_header

namespace peel
{
namespace GLib
{
struct Error;
} /* namespace GLib */

namespace GObject
{
class Binding;
enum class BindingFlags : std::underlying_type<::GBindingFlags>::type;
class InitiallyUnowned;
class Object;
struct Value;
} /* namespace GObject */

namespace Test
{
class Testy;
} /* namespace Test */

template<>
inline GObject::Type
GObject::Type::of<Test::Testy> ()
{
  return test_testy_get_type ();
}


namespace Test
{
class Testy : public GObject::Object
/* non-derivable */
{
private:
  using GObject::Object::bind_property;
  friend class GObject::Type;
  friend class GObject::TypeInstance;
  friend class GObject::TypeClass;

  Testy () = delete;
  Testy (const Testy &) = delete;
  Testy (Testy &&) = delete;
  Testy &
  operator = (const Testy &) = delete;
  Testy &
  operator = (Testy &&) = delete;
  ~Testy () = delete;
public:

  static peel::RefPtr<Testy>
  create () noexcept
  {
    ::TestTesty * _peel_return = test_testy_new ();
    peel_assume (_peel_return);
    return peel::RefPtr<Testy>::adopt_ref (reinterpret_cast<Testy *> (_peel_return));
  }

  int
  get_int () noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    return test_testy_get_int (_peel_this);
  }

  void
  pass_bool (bool b) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gboolean _peel_b = static_cast<gboolean> (b);
    test_testy_pass_bool (_peel_this, _peel_b);
  }

  void
  pass_gtype (GObject::Type gt) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    test_testy_pass_gtype (_peel_this, gt);
  }

  peel_arg_out (2) peel_arg_out (3) peel_nonnull_args (2, 3)
  void
  get_out_int (int *int_ptr, int *int_ptr2) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    test_testy_get_out_int (_peel_this, int_ptr, int_ptr2);
  }

  peel_arg_out (2) peel_nonnull_args (2)
  void
  get_out_bool (bool *b) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gboolean _peel_b;
    test_testy_get_out_bool (_peel_this, &_peel_b);
    *b = !!_peel_b;
  }

  peel_arg_out (2) peel_nonnull_args (2)
  void
  get_out_bool_throws (bool *b, peel::UniquePtr<GLib::Error> *error) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gboolean _peel_b;
    ::GError *_peel_error = nullptr;
    test_testy_get_out_bool_throws (_peel_this, &_peel_b, &_peel_error);
    if (_peel_error)
      {
        if (error)
          *error = peel::UniquePtr<GLib::Error>::adopt_ref (reinterpret_cast<GLib::Error *> (_peel_error));
        else
          g_error_free (_peel_error);
      }
    else
      {
        if (error)
          *error = nullptr;
        *b = !!_peel_b;
      }
  }

  peel_arg_out (2) peel_nonnull_args (2)
  void
  get_out_gtype (GObject::Type *gt) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    GType * _peel_gt = reinterpret_cast<GType *> (gt);
    test_testy_get_out_gtype (_peel_this, _peel_gt);
  }

  peel_arg_out (2) peel_nonnull_args (2)
  void
  get_out_object (GObject::Object **obj_ptr) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_obj_ptr = reinterpret_cast<::GObject **> (obj_ptr);
    test_testy_get_out_object (_peel_this, _peel_obj_ptr);
  }

  peel_arg_out (2)
  void
  get_out_owned_object (peel::RefPtr<GObject::Object> *obj_ptr) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject * _peel_obj_ptr;
    test_testy_get_out_owned_object (_peel_this, &_peel_obj_ptr);
    *obj_ptr = peel::RefPtr<GObject::Object>::adopt_ref (reinterpret_cast<GObject::Object *> (_peel_obj_ptr));
  }

  peel_arg_out (2)
  void
  get_out_owned_optional_object (peel::RefPtr<GObject::Object> *obj_ptr) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject * _peel_obj_ptr;
    test_testy_get_out_owned_optional_object (_peel_this, obj_ptr ? &_peel_obj_ptr : nullptr);
    if (obj_ptr)
      *obj_ptr = peel::RefPtr<GObject::Object>::adopt_ref (reinterpret_cast<GObject::Object *> (_peel_obj_ptr));
  }

  peel_arg_inout (2)
  void
  inout_owned_optional_object (peel::RefPtr<GObject::Object> *obj_ptr) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject * _peel_obj_ptr;
    if (obj_ptr)
      _peel_obj_ptr = reinterpret_cast<::GObject *> (std::move (*obj_ptr).release_ref ());
    test_testy_inout_owned_optional_object (_peel_this, obj_ptr ? &_peel_obj_ptr : nullptr);
    if (obj_ptr)
      *obj_ptr = peel::RefPtr<GObject::Object>::adopt_ref (reinterpret_cast<GObject::Object *> (_peel_obj_ptr));
  }

  void
  inout_owned_optional_array (peel::UniquePtr<const uint8_t[]> *data) noexcept
  {
    gsize _peel_data_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gconstpointer _peel_data;
    if (data)
      _peel_data = (_peel_data_size = data->size (), reinterpret_cast<gconstpointer> (std::move (*data).release_ref ()));
    test_testy_inout_owned_optional_array (_peel_this, data ? &_peel_data : nullptr, &_peel_data_size);
    if (data)
      *data = peel::UniquePtr<const uint8_t[]>::adopt_ref (reinterpret_cast<const uint8_t *> (_peel_data), _peel_data_size);
  }

  peel_arg_in (2) peel_nonnull_args (2)
  void
  pass_value (const GObject::Value *v) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    const ::GValue * _peel_v = reinterpret_cast<const ::GValue *> (v);
    test_testy_pass_value (_peel_this, _peel_v);
  }

  peel_arg_out (2) peel_nonnull_args (2)
  void
  get_out_value (GObject::Value *value_ptr) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GValue * _peel_value_ptr = reinterpret_cast<::GValue *> (value_ptr);
    test_testy_get_out_value (_peel_this, _peel_value_ptr);
  }

  static void
  takes_self_by_ref (peel::RefPtr<Testy> self) noexcept
  {
    ::TestTesty * _peel_self = reinterpret_cast<::TestTesty *> (std::move (self).release_ref ());
    test_testy_takes_self_by_ref (_peel_self);
  }

  peel_arg_in (2) peel_nonnull_args (2)
  void
  nested_types (GObject::Object::ConstructParam *construct_param, GObject::Binding::Flags binding_flags) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObjectConstructParam * _peel_construct_param = reinterpret_cast<::GObjectConstructParam *> (construct_param);
    ::GBindingFlags _peel_binding_flags = static_cast<::GBindingFlags> (binding_flags);
    test_testy_nested_types (_peel_this, _peel_construct_param, _peel_binding_flags);
  }

  template<typename CoolCallback>
  int
  add_cool_callback (peel::RefPtr<GObject::Binding> binding, peel::String s, CoolCallback &&callback) noexcept
  {
    gpointer _peel_user_data;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GBinding * _peel_binding = reinterpret_cast<::GBinding *> (std::move (binding).release_ref ());
    char * _peel_s = std::move (s).release_string ();
    ::TestCoolCallback _peel_callback = peel::internals::CallbackHelper<::TestTesty *, gboolean *>::wrap_async_callback (
      static_cast<CoolCallback &&> (callback),
      [] (gboolean *b, gpointer user_data) -> ::TestTesty *
      {
        CoolCallback &_peel_captured_callback = *reinterpret_cast<typename std::remove_reference<CoolCallback>::type *> (user_data);
        bool _peel_b;
        peel::RefPtr<Testy> _peel_return = static_cast<CoolCallback &&> (_peel_captured_callback) (&_peel_b);
        *b = static_cast<gboolean> (_peel_b);
        return reinterpret_cast<::TestTesty *> (std::move (_peel_return).release_ref ());
      },
      &_peel_user_data);
    return test_testy_add_cool_callback (_peel_this, _peel_binding, _peel_s, _peel_callback, _peel_user_data);
  }

  /* Unsupported for now: get_cool_callback: casting callback from C to C++ */
  static void
  get_cool_callback (UnsupportedForNowToken);

  peel::ArrayRef<const uint8_t>
  return_byte_array () noexcept
  {
    gsize _peel_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gconstpointer _peel_return = test_testy_return_byte_array (_peel_this, &_peel_size);
    peel_assume (_peel_return);
    return peel::ArrayRef<const uint8_t> (reinterpret_cast<const uint8_t *> (_peel_return), _peel_size);
  }

  peel::UniquePtr<uint8_t[]>
  return_owned_byte_array () noexcept
  {
    gsize _peel_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gpointer _peel_return = test_testy_return_owned_byte_array (_peel_this, &_peel_size);
    peel_assume (_peel_return);
    return peel::UniquePtr<uint8_t[]>::adopt_ref (reinterpret_cast<uint8_t *> (_peel_return), _peel_size);
  }

  peel::ArrayRef<const uint8_t>
  return_byte_array_of_size (size_t size) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gconstpointer _peel_return = test_testy_return_byte_array_of_size (_peel_this, size);
    peel_assume (_peel_return);
    return peel::ArrayRef<const uint8_t> (reinterpret_cast<const uint8_t *> (_peel_return), size);
  }

  peel::UniquePtr<uint8_t[]>
  return_owned_byte_array_of_size (size_t size) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gpointer _peel_return = test_testy_return_owned_byte_array_of_size (_peel_this, size);
    peel_assume (_peel_return);
    return peel::UniquePtr<uint8_t[]>::adopt_ref (reinterpret_cast<uint8_t *> (_peel_return), size);
  }

  void
  first_n_primes (peel::UniquePtr<int[]> *primes, int n) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gint * _peel_primes;
    test_testy_first_n_primes (_peel_this, &_peel_primes, n);
    *primes = peel::UniquePtr<int[]>::adopt_ref (reinterpret_cast<int *> (_peel_primes), n);
  }

  const float
  (&return_fixed_size_array () noexcept)[4]
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    const float * _peel_return = test_testy_return_fixed_size_array (_peel_this);
    peel_assume (_peel_return);
    return reinterpret_cast<const float (&)[4]> (*_peel_return);
  }

  peel::ArrayRef<const GObject::Binding::Flags>
  return_flags_array () noexcept
  {
    gsize _peel_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    const ::GBindingFlags * _peel_return = test_testy_return_flags_array (_peel_this, &_peel_size);
    peel_assume (_peel_return);
    return peel::ArrayRef<const GObject::Binding::Flags> (reinterpret_cast<const GObject::Binding::Flags *> (_peel_return), _peel_size);
  }

  size_t
  read_in (peel::ArrayRef<uint8_t> buffer) noexcept
  {
    gsize _peel_buffer_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    void * _peel_buffer = (_peel_buffer_size = buffer.size (), reinterpret_cast<void *> (buffer.data ()));
    return test_testy_read_in (_peel_this, _peel_buffer, _peel_buffer_size);
  }

  void
  read_out (peel::ArrayRef<uint8_t> *buffer) noexcept
  {
    gsize _peel_buffer_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    void * _peel_buffer;
    test_testy_read_out (_peel_this, &_peel_buffer, &_peel_buffer_size);
    *buffer = peel::ArrayRef<uint8_t> (reinterpret_cast<uint8_t *> (_peel_buffer), _peel_buffer_size);
  }

  void
  pass_array_transfer_full (peel::UniquePtr<peel::RefPtr<GObject::Object>[]> objects) noexcept
  {
    gsize _peel_count;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_objects = (_peel_count = objects.size (), reinterpret_cast<::GObject **> (std::move (objects).release_ref ()));
    test_testy_pass_array_transfer_full (_peel_this, _peel_objects, _peel_count);
  }

  void
  pass_array_transfer_container (peel::UniquePtr<GObject::Object *[]> objects) noexcept
  {
    gsize _peel_count;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_objects = (_peel_count = objects.size (), reinterpret_cast<::GObject **> (std::move (objects).release_ref ()));
    test_testy_pass_array_transfer_container (_peel_this, _peel_objects, _peel_count);
  }

  void
  pass_array_transfer_none (peel::ArrayRef<GObject::Object *> objects) noexcept
  {
    gsize _peel_count;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_objects = (_peel_count = objects.size (), reinterpret_cast<::GObject **> (objects.data ()));
    test_testy_pass_array_transfer_none (_peel_this, _peel_objects, _peel_count);
  }

  peel::UniquePtr<peel::RefPtr<GObject::Object>[]>
  return_array_transfer_full () noexcept
  {
    gsize _peel_count;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_return = test_testy_return_array_transfer_full (_peel_this, &_peel_count);
    peel_assume (_peel_return);
    return peel::UniquePtr<peel::RefPtr<GObject::Object>[]>::adopt_ref (reinterpret_cast<peel::RefPtr<GObject::Object> *> (_peel_return), _peel_count);
  }

  peel::UniquePtr<GObject::Object *[]>
  return_array_transfer_container () noexcept
  {
    gsize _peel_count;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_return = test_testy_return_array_transfer_container (_peel_this, &_peel_count);
    peel_assume (_peel_return);
    return peel::UniquePtr<GObject::Object *[]>::adopt_ref (reinterpret_cast<GObject::Object **> (_peel_return), _peel_count);
  }

  peel::ArrayRef<GObject::Object *>
  return_array_transfer_none () noexcept
  {
    gsize _peel_count;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_return = test_testy_return_array_transfer_none (_peel_this, &_peel_count);
    peel_assume (_peel_return);
    return peel::ArrayRef<GObject::Object *> (reinterpret_cast<GObject::Object **> (_peel_return), _peel_count);
  }

  void
  inout_value_array (peel::UniquePtr<GObject::Value[]> *values) noexcept
  {
    gsize _peel_count;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GValue * _peel_values = (_peel_count = values->size (), reinterpret_cast<::GValue *> (std::move (*values).release_ref ()));
    test_testy_inout_value_array (_peel_this, &_peel_values, &_peel_count);
    *values = peel::UniquePtr<GObject::Value[]>::adopt_ref (reinterpret_cast<GObject::Value *> (_peel_values), _peel_count);
  }

  peel_arg_out (2) peel_nonnull_args (2)
  void
  out_gconstpointer (const void **ptr) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gconstpointer * _peel_ptr = reinterpret_cast<gconstpointer *> (ptr);
    test_testy_out_gconstpointer (_peel_this, _peel_ptr);
  }

  peel_nonnull_args (2) peel_returns_nonnull
  VoidAlias *
  void_alias (VoidAlias *arg) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    TestVoidAlias * _peel_arg = reinterpret_cast<TestVoidAlias *> (arg);
    TestVoidAlias * _peel_return = test_testy_void_alias (_peel_this, _peel_arg);
    peel_assume (_peel_return);
    return reinterpret_cast<VoidAlias *> (_peel_return);
  }

  template<typename... Args>
  peel_nonnull_args (2)
  void
  argv_transfer_none (GObject::Object *first_object, Args &&...args) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject * _peel_first_object = reinterpret_cast<::GObject *> (first_object);
    test_testy_argv_transfer_none (_peel_this, _peel_first_object, reinterpret_cast<::GObject *> (static_cast<GObject::Object *> (std::forward<Args> (args)))..., nullptr);
  }

  template<typename... Args>
  void
  argv_transfer_floating (peel::FloatPtr<GObject::InitiallyUnowned> first_object, Args &&...args) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GInitiallyUnowned * _peel_first_object = reinterpret_cast<::GInitiallyUnowned *> (std::move (first_object).release_floating_ptr ());
    test_testy_argv_transfer_floating (_peel_this, _peel_first_object, reinterpret_cast<::GInitiallyUnowned *> (std::move (static_cast<peel::FloatPtr<GObject::InitiallyUnowned>> (std::forward<Args> (args))).release_floating_ptr ())..., nullptr);
  }

  template<typename... Args>
  void
  argv_transfer_full (peel::RefPtr<GObject::Object> first_object, Args &&...args) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject * _peel_first_object = reinterpret_cast<::GObject *> (std::move (first_object).release_ref ());
    test_testy_argv_transfer_full (_peel_this, _peel_first_object, reinterpret_cast<::GObject *> (std::move (static_cast<peel::RefPtr<GObject::Object>> (std::forward<Args> (args))).release_ref ())..., nullptr);
  }

  template<typename... Args>
  void
  argv_transfer_none_str (const char *first_str, Args &&...args) noexcept
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    test_testy_argv_transfer_none_str (_peel_this, first_str, args..., nullptr);
  }

  static peel::Property<void *>
  prop_smuggled_pointer ()
  {
    return peel::Property<void *> { "smuggled-pointer" };
  }

protected:
  template<typename DerivedClass>
  int
  parent_vfunc_get_int () noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    return _peel_class->get_int (_peel_this);
  }

  template<typename DerivedClass>
  void
  parent_vfunc_pass_bool (bool b) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gboolean _peel_b = static_cast<gboolean> (b);
    _peel_class->pass_bool (_peel_this, _peel_b);
  }

  template<typename DerivedClass>
  peel_arg_out (2) peel_arg_out (3) peel_nonnull_args (2, 3)
  void
  parent_vfunc_get_out_int (int *int_ptr, int *int_ptr2) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    _peel_class->get_out_int (_peel_this, int_ptr, int_ptr2);
  }

  template<typename DerivedClass>
  peel_arg_out (2) peel_nonnull_args (2)
  void
  parent_vfunc_get_out_bool (bool *b) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gboolean _peel_b;
    _peel_class->get_out_bool (_peel_this, &_peel_b);
    *b = !!_peel_b;
  }

  template<typename DerivedClass>
  peel_arg_out (2) peel_nonnull_args (2)
  void
  parent_vfunc_get_out_bool_throws (bool *b, peel::UniquePtr<GLib::Error> *error) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gboolean _peel_b;
    ::GError *_peel_error = nullptr;
    _peel_class->get_out_bool_throws (_peel_this, &_peel_b, &_peel_error);
    if (_peel_error)
      {
        if (error)
          *error = peel::UniquePtr<GLib::Error>::adopt_ref (reinterpret_cast<GLib::Error *> (_peel_error));
        else
          g_error_free (_peel_error);
      }
    else
      {
        if (error)
          *error = nullptr;
        *b = !!_peel_b;
      }
  }

  template<typename DerivedClass>
  peel_arg_out (2) peel_nonnull_args (2)
  void
  parent_vfunc_get_out_object (GObject::Object **obj_ptr) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_obj_ptr = reinterpret_cast<::GObject **> (obj_ptr);
    _peel_class->get_out_object (_peel_this, _peel_obj_ptr);
  }

  template<typename DerivedClass>
  peel_arg_out (2)
  void
  parent_vfunc_get_out_owned_object (peel::RefPtr<GObject::Object> *obj_ptr) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject * _peel_obj_ptr;
    _peel_class->get_out_owned_object (_peel_this, &_peel_obj_ptr);
    *obj_ptr = peel::RefPtr<GObject::Object>::adopt_ref (reinterpret_cast<GObject::Object *> (_peel_obj_ptr));
  }

  template<typename DerivedClass>
  peel_arg_out (2)
  void
  parent_vfunc_get_out_owned_optional_object (peel::RefPtr<GObject::Object> *obj_ptr) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject * _peel_obj_ptr;
    _peel_class->get_out_owned_optional_object (_peel_this, obj_ptr ? &_peel_obj_ptr : nullptr);
    if (obj_ptr)
      *obj_ptr = peel::RefPtr<GObject::Object>::adopt_ref (reinterpret_cast<GObject::Object *> (_peel_obj_ptr));
  }

  template<typename DerivedClass>
  peel_arg_inout (2)
  void
  parent_vfunc_inout_owned_optional_object (peel::RefPtr<GObject::Object> *obj_ptr) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject * _peel_obj_ptr;
    if (obj_ptr)
      _peel_obj_ptr = reinterpret_cast<::GObject *> (std::move (*obj_ptr).release_ref ());
    _peel_class->inout_owned_optional_object (_peel_this, obj_ptr ? &_peel_obj_ptr : nullptr);
    if (obj_ptr)
      *obj_ptr = peel::RefPtr<GObject::Object>::adopt_ref (reinterpret_cast<GObject::Object *> (_peel_obj_ptr));
  }

  template<typename DerivedClass>
  void
  parent_vfunc_inout_owned_optional_array (peel::UniquePtr<const uint8_t[]> *data) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_data_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gconstpointer _peel_data;
    if (data)
      _peel_data = (_peel_data_size = data->size (), reinterpret_cast<gconstpointer> (std::move (*data).release_ref ()));
    _peel_class->inout_owned_optional_array (_peel_this, data ? &_peel_data : nullptr, &_peel_data_size);
    if (data)
      *data = peel::UniquePtr<const uint8_t[]>::adopt_ref (reinterpret_cast<const uint8_t *> (_peel_data), _peel_data_size);
  }

  template<typename DerivedClass>
  peel_arg_out (2) peel_nonnull_args (2)
  void
  parent_vfunc_get_out_value (GObject::Value *value_ptr) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GValue * _peel_value_ptr = reinterpret_cast<::GValue *> (value_ptr);
    _peel_class->get_out_value (_peel_this, _peel_value_ptr);
  }

  template<typename DerivedClass>
  static void
  parent_vfunc_takes_self_by_ref (peel::RefPtr<Testy> self) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_self = reinterpret_cast<::TestTesty *> (std::move (self).release_ref ());
    _peel_class->takes_self_by_ref (_peel_self);
  }

  template<typename DerivedClass, typename CoolCallback>
  int
  parent_vfunc_add_cool_callback (peel::RefPtr<GObject::Binding> binding, peel::String s, CoolCallback &&callback) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    gpointer _peel_user_data;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GBinding * _peel_binding = reinterpret_cast<::GBinding *> (std::move (binding).release_ref ());
    char * _peel_s = std::move (s).release_string ();
    ::TestCoolCallback _peel_callback = peel::internals::CallbackHelper<::TestTesty *, gboolean *>::wrap_async_callback (
      static_cast<CoolCallback &&> (callback),
      [] (gboolean *b, gpointer user_data) -> ::TestTesty *
      {
        CoolCallback &_peel_captured_callback = *reinterpret_cast<typename std::remove_reference<CoolCallback>::type *> (user_data);
        bool _peel_b;
        peel::RefPtr<Testy> _peel_return = static_cast<CoolCallback &&> (_peel_captured_callback) (&_peel_b);
        *b = static_cast<gboolean> (_peel_b);
        return reinterpret_cast<::TestTesty *> (std::move (_peel_return).release_ref ());
      },
      &_peel_user_data);
    return _peel_class->add_cool_callback (_peel_this, _peel_binding, _peel_s, _peel_callback, _peel_user_data);
  }

  /* Unsupported for now: get_cool_callback: casting callback from C to C++ */

  template<typename DerivedClass>
  peel::ArrayRef<const uint8_t>
  parent_vfunc_return_byte_array () noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gconstpointer _peel_return = _peel_class->return_byte_array (_peel_this, &_peel_size);
    peel_assume (_peel_return);
    return peel::ArrayRef<const uint8_t> (reinterpret_cast<const uint8_t *> (_peel_return), _peel_size);
  }

  template<typename DerivedClass>
  peel::UniquePtr<uint8_t[]>
  parent_vfunc_return_owned_byte_array () noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gpointer _peel_return = _peel_class->return_owned_byte_array (_peel_this, &_peel_size);
    peel_assume (_peel_return);
    return peel::UniquePtr<uint8_t[]>::adopt_ref (reinterpret_cast<uint8_t *> (_peel_return), _peel_size);
  }

  template<typename DerivedClass>
  peel::ArrayRef<const uint8_t>
  parent_vfunc_return_byte_array_of_size (size_t size) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gconstpointer _peel_return = _peel_class->return_byte_array_of_size (_peel_this, size);
    peel_assume (_peel_return);
    return peel::ArrayRef<const uint8_t> (reinterpret_cast<const uint8_t *> (_peel_return), size);
  }

  template<typename DerivedClass>
  peel::UniquePtr<uint8_t[]>
  parent_vfunc_return_owned_byte_array_of_size (size_t size) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gpointer _peel_return = _peel_class->return_owned_byte_array_of_size (_peel_this, size);
    peel_assume (_peel_return);
    return peel::UniquePtr<uint8_t[]>::adopt_ref (reinterpret_cast<uint8_t *> (_peel_return), size);
  }

  template<typename DerivedClass>
  void
  parent_vfunc_first_n_primes (peel::UniquePtr<int[]> *primes, int n) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gint * _peel_primes;
    _peel_class->first_n_primes (_peel_this, &_peel_primes, n);
    *primes = peel::UniquePtr<int[]>::adopt_ref (reinterpret_cast<int *> (_peel_primes), n);
  }

  template<typename DerivedClass>
  const float
  (&parent_vfunc_return_fixed_size_array () noexcept)[4]
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    const float * _peel_return = _peel_class->return_fixed_size_array (_peel_this);
    peel_assume (_peel_return);
    return reinterpret_cast<const float (&)[4]> (*_peel_return);
  }

  template<typename DerivedClass>
  peel::ArrayRef<const GObject::Binding::Flags>
  parent_vfunc_return_flags_array () noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    const ::GBindingFlags * _peel_return = _peel_class->return_flags_array (_peel_this, &_peel_size);
    peel_assume (_peel_return);
    return peel::ArrayRef<const GObject::Binding::Flags> (reinterpret_cast<const GObject::Binding::Flags *> (_peel_return), _peel_size);
  }

  template<typename DerivedClass>
  size_t
  parent_vfunc_read_in (peel::ArrayRef<uint8_t> buffer) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_buffer_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    void * _peel_buffer = (_peel_buffer_size = buffer.size (), reinterpret_cast<void *> (buffer.data ()));
    return _peel_class->read_in (_peel_this, _peel_buffer, _peel_buffer_size);
  }

  template<typename DerivedClass>
  void
  parent_vfunc_read_out (peel::ArrayRef<uint8_t> *buffer) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_buffer_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    void * _peel_buffer;
    _peel_class->read_out (_peel_this, &_peel_buffer, &_peel_buffer_size);
    *buffer = peel::ArrayRef<uint8_t> (reinterpret_cast<uint8_t *> (_peel_buffer), _peel_buffer_size);
  }

  template<typename DerivedClass>
  void
  parent_vfunc_pass_array_transfer_full (peel::UniquePtr<peel::RefPtr<GObject::Object>[]> objects) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_count;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_objects = (_peel_count = objects.size (), reinterpret_cast<::GObject **> (std::move (objects).release_ref ()));
    _peel_class->pass_array_transfer_full (_peel_this, _peel_objects, _peel_count);
  }

  template<typename DerivedClass>
  void
  parent_vfunc_pass_array_transfer_container (peel::UniquePtr<GObject::Object *[]> objects) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_count;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_objects = (_peel_count = objects.size (), reinterpret_cast<::GObject **> (std::move (objects).release_ref ()));
    _peel_class->pass_array_transfer_container (_peel_this, _peel_objects, _peel_count);
  }

  template<typename DerivedClass>
  void
  parent_vfunc_pass_array_transfer_none (peel::ArrayRef<GObject::Object *> objects) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_count;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_objects = (_peel_count = objects.size (), reinterpret_cast<::GObject **> (objects.data ()));
    _peel_class->pass_array_transfer_none (_peel_this, _peel_objects, _peel_count);
  }

  template<typename DerivedClass>
  peel::UniquePtr<peel::RefPtr<GObject::Object>[]>
  parent_vfunc_return_array_transfer_full () noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_count;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_return = _peel_class->return_array_transfer_full (_peel_this, &_peel_count);
    peel_assume (_peel_return);
    return peel::UniquePtr<peel::RefPtr<GObject::Object>[]>::adopt_ref (reinterpret_cast<peel::RefPtr<GObject::Object> *> (_peel_return), _peel_count);
  }

  template<typename DerivedClass>
  peel::UniquePtr<peel::RefPtr<GObject::Object>[]>
  parent_vfunc_return_array_transfer_container () noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_count;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_return = _peel_class->return_array_transfer_container (_peel_this, &_peel_count);
    peel_assume (_peel_return);
    return peel::UniquePtr<peel::RefPtr<GObject::Object>[]>::adopt_ref (reinterpret_cast<peel::RefPtr<GObject::Object> *> (_peel_return), _peel_count);
  }

  template<typename DerivedClass>
  peel::ArrayRef<GObject::Object *>
  parent_vfunc_return_array_transfer_none () noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_count;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_return = _peel_class->return_array_transfer_none (_peel_this, &_peel_count);
    peel_assume (_peel_return);
    return peel::ArrayRef<GObject::Object *> (reinterpret_cast<GObject::Object **> (_peel_return), _peel_count);
  }

  template<typename DerivedClass>
  void
  parent_vfunc_inout_value_array (peel::UniquePtr<GObject::Value[]> *values) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_count;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GValue * _peel_values = (_peel_count = values->size (), reinterpret_cast<::GValue *> (std::move (*values).release_ref ()));
    _peel_class->inout_value_array (_peel_this, &_peel_values, &_peel_count);
    *values = peel::UniquePtr<GObject::Value[]>::adopt_ref (reinterpret_cast<GObject::Value *> (_peel_values), _peel_count);
  }

  template<typename DerivedClass>
  static peel::RefPtr<Testy>
  parent_vfunc_make_instance (int i) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_return = _peel_class->make_instance (i);
    peel_assume (_peel_return);
    return peel::RefPtr<Testy>::adopt_ref (reinterpret_cast<Testy *> (_peel_return));
  }

  template<typename DerivedClass>
  peel_arg_out (2) peel_nonnull_args (2)
  void
  parent_vfunc_out_gconstpointer (const void **ptr) noexcept
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (GObject::TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gconstpointer * _peel_ptr = reinterpret_cast<gconstpointer *> (ptr);
    _peel_class->out_gconstpointer (_peel_this, _peel_ptr);
  }

public:
  class Class : public GObject::Object::Class
  {
  private:
    Class () = delete;
    Class (const Class &) = delete;
    Class (Class &&) = delete;

    unsigned char _placeholder[sizeof (::TestTestyClass) - sizeof (GObject::Object::Class)] peel_no_warn_unused;
  protected:
    template<typename DerivedClass>
    void
    override_vfunc_get_int ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->get_int = +[] (::TestTesty *self) -> int
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        return _peel_this->DerivedClass::vfunc_get_int ();
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_pass_bool ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->pass_bool = +[] (::TestTesty *self, gboolean b) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        bool _peel_b = !!b;
        _peel_this->DerivedClass::vfunc_pass_bool (_peel_b);
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_get_out_int ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->get_out_int = +[] (::TestTesty *self, int *int_ptr, int *int_ptr2) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        _peel_this->DerivedClass::vfunc_get_out_int (int_ptr, int_ptr2);
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_get_out_bool ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->get_out_bool = +[] (::TestTesty *self, gboolean *b) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        bool _peel_b;
        _peel_this->DerivedClass::vfunc_get_out_bool (&_peel_b);
        *b = static_cast<gboolean> (_peel_b);
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_get_out_bool_throws ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->get_out_bool_throws = +[] (::TestTesty *self, gboolean *b, ::GError **error) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        bool _peel_b;
        peel::UniquePtr<GLib::Error> _peel_error;
        _peel_this->DerivedClass::vfunc_get_out_bool_throws (&_peel_b, &_peel_error);
        if (_peel_error)
          {
            if (error)
              *error = reinterpret_cast<::GError *> (std::move (_peel_error).release_ref ());
          }
        else
          {
            *b = static_cast<gboolean> (_peel_b);
          }
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_get_out_object ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->get_out_object = +[] (::TestTesty *self, ::GObject **obj_ptr) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        GObject::Object **_peel_obj_ptr = reinterpret_cast<GObject::Object **> (obj_ptr);
        _peel_this->DerivedClass::vfunc_get_out_object (_peel_obj_ptr);
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_get_out_owned_object ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->get_out_owned_object = +[] (::TestTesty *self, ::GObject **obj_ptr) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::RefPtr<GObject::Object> _peel_obj_ptr;
        _peel_this->DerivedClass::vfunc_get_out_owned_object (&_peel_obj_ptr);
        *obj_ptr = reinterpret_cast<::GObject *> (std::move (_peel_obj_ptr).release_ref ());
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_get_out_owned_optional_object ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->get_out_owned_optional_object = +[] (::TestTesty *self, ::GObject **obj_ptr) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::RefPtr<GObject::Object> _peel_obj_ptr;
        _peel_this->DerivedClass::vfunc_get_out_owned_optional_object (obj_ptr ? &_peel_obj_ptr : nullptr);
        if (obj_ptr)
          *obj_ptr = reinterpret_cast<::GObject *> (std::move (_peel_obj_ptr).release_ref ());
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_inout_owned_optional_object ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->inout_owned_optional_object = +[] (::TestTesty *self, ::GObject **obj_ptr) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::RefPtr<GObject::Object> _peel_obj_ptr;
        if (obj_ptr)
          _peel_obj_ptr = peel::RefPtr<GObject::Object>::adopt_ref (reinterpret_cast<GObject::Object *> (*obj_ptr));
        _peel_this->DerivedClass::vfunc_inout_owned_optional_object (obj_ptr ? &_peel_obj_ptr : nullptr);
        if (obj_ptr)
          *obj_ptr = reinterpret_cast<::GObject *> (std::move (_peel_obj_ptr).release_ref ());
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_inout_owned_optional_array ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->inout_owned_optional_array = +[] (::TestTesty *self, gconstpointer *data, gsize *data_size) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::UniquePtr<const uint8_t[]> _peel_data;
        if (data)
          _peel_data = peel::UniquePtr<const uint8_t[]>::adopt_ref (reinterpret_cast<const uint8_t *> (*data), *data_size);
        _peel_this->DerivedClass::vfunc_inout_owned_optional_array (data ? &_peel_data : nullptr);
        if (data)
          *data = ((data_size ? (*data_size = _peel_data.size ()) : 0), reinterpret_cast<gconstpointer> (std::move (_peel_data).release_ref ()));
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_get_out_value ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->get_out_value = +[] (::TestTesty *self, ::GValue *value_ptr) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        GObject::Value *_peel_value_ptr = reinterpret_cast<GObject::Value *> (value_ptr);
        _peel_this->DerivedClass::vfunc_get_out_value (_peel_value_ptr);
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_takes_self_by_ref ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->takes_self_by_ref = +[] (::TestTesty *self) -> void
      {
        peel::RefPtr<DerivedClass> _peel_self = peel::RefPtr<DerivedClass>::adopt_ref (reinterpret_cast<DerivedClass *> (self));
        DerivedClass::vfunc_takes_self_by_ref (std::move (_peel_self));
      };
    }

    /* Unsupported for now: add_cool_callback: casting callback from C to C++ */

    template<typename DerivedClass>
    void
    override_vfunc_get_cool_callback ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->get_cool_callback = +[] (::TestTesty *self, ::GBinding *binding, char *s, gpointer *user_data) -> ::TestCoolCallback
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::RefPtr<GObject::Binding> _peel_binding = peel::RefPtr<GObject::Binding>::adopt_ref (reinterpret_cast<GObject::Binding *> (binding));
        peel::String _peel_s = peel::String::adopt_string (s);
        auto _peel_return = _peel_this->DerivedClass::vfunc_get_cool_callback (std::move (_peel_binding), std::move (_peel_s));
        return peel::internals::CallbackHelper<::TestTesty *, gboolean *>::wrap_async_callback (
      static_cast<decltype (_peel_return) &&> (_peel_return),
      [] (gboolean *b, gpointer user_data) -> ::TestTesty *
      {
        decltype (_peel_return) &_peel_captured__peel_return = *reinterpret_cast<typename std::remove_reference<decltype (_peel_return)>::type *> (user_data);
        bool _peel_b;
        peel::RefPtr<Testy> _peel_return = static_cast<decltype (_peel_return) &&> (_peel_captured__peel_return) (&_peel_b);
        *b = static_cast<gboolean> (_peel_b);
        return reinterpret_cast<::TestTesty *> (std::move (_peel_return).release_ref ());
      },
      &*user_data);
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_return_byte_array ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->return_byte_array = +[] (::TestTesty *self, gsize *size) -> gconstpointer
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::ArrayRef<const uint8_t> _peel_return = _peel_this->DerivedClass::vfunc_return_byte_array ();
        return ((size ? (*size = _peel_return.size ()) : 0), reinterpret_cast<gconstpointer> (_peel_return.data ()));
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_return_owned_byte_array ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->return_owned_byte_array = +[] (::TestTesty *self, gsize *size) -> gpointer
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::UniquePtr<uint8_t[]> _peel_return = _peel_this->DerivedClass::vfunc_return_owned_byte_array ();
        return ((size ? (*size = _peel_return.size ()) : 0), reinterpret_cast<gpointer> (std::move (_peel_return).release_ref ()));
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_return_byte_array_of_size ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->return_byte_array_of_size = +[] (::TestTesty *self, gsize size) -> gconstpointer
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::ArrayRef<const uint8_t> _peel_return = _peel_this->DerivedClass::vfunc_return_byte_array_of_size (size);
        return (/* g_assert (_peel_return.size () == size) */ 0, reinterpret_cast<gconstpointer> (_peel_return.data ()));
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_return_owned_byte_array_of_size ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->return_owned_byte_array_of_size = +[] (::TestTesty *self, gsize size) -> gpointer
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::UniquePtr<uint8_t[]> _peel_return = _peel_this->DerivedClass::vfunc_return_owned_byte_array_of_size (size);
        return (/* g_assert (_peel_return.size () == size) */ 0, reinterpret_cast<gpointer> (std::move (_peel_return).release_ref ()));
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_first_n_primes ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->first_n_primes = +[] (::TestTesty *self, gint **primes, gint n) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::UniquePtr<int[]> _peel_primes;
        _peel_this->DerivedClass::vfunc_first_n_primes (&_peel_primes, n);
        *primes = (/* g_assert (_peel_primes.size () == n) */ 0, reinterpret_cast<gint *> (std::move (_peel_primes).release_ref ()));
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_return_fixed_size_array ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->return_fixed_size_array = +[] (::TestTesty *self) -> const float *
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        const float (&_peel_return)[4] = _peel_this->DerivedClass::vfunc_return_fixed_size_array ();
        return reinterpret_cast<const float *> (_peel_return);
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_return_flags_array ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->return_flags_array = +[] (::TestTesty *self, gsize *size) -> const ::GBindingFlags *
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::ArrayRef<const GObject::Binding::Flags> _peel_return = _peel_this->DerivedClass::vfunc_return_flags_array ();
        return ((size ? (*size = _peel_return.size ()) : 0), reinterpret_cast<const ::GBindingFlags *> (_peel_return.data ()));
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_read_in ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->read_in = +[] (::TestTesty *self, void *buffer, gsize buffer_size) -> gsize
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::ArrayRef<uint8_t> _peel_buffer = peel::ArrayRef<uint8_t> (reinterpret_cast<uint8_t *> (buffer), buffer_size);
        return _peel_this->DerivedClass::vfunc_read_in (_peel_buffer);
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_read_out ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->read_out = +[] (::TestTesty *self, void **buffer, gsize *buffer_size) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::ArrayRef<uint8_t> _peel_buffer;
        _peel_this->DerivedClass::vfunc_read_out (&_peel_buffer);
        *buffer = (*buffer_size = _peel_buffer.size (), reinterpret_cast<void *> (_peel_buffer.data ()));
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_pass_array_transfer_full ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->pass_array_transfer_full = +[] (::TestTesty *self, ::GObject **objects, gsize count) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::UniquePtr<peel::RefPtr<GObject::Object>[]> _peel_objects = peel::UniquePtr<peel::RefPtr<GObject::Object>[]>::adopt_ref (reinterpret_cast<peel::RefPtr<GObject::Object> *> (objects), count);
        _peel_this->DerivedClass::vfunc_pass_array_transfer_full (std::move (_peel_objects));
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_pass_array_transfer_container ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->pass_array_transfer_container = +[] (::TestTesty *self, ::GObject **objects, gsize count) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::UniquePtr<GObject::Object *[]> _peel_objects = peel::UniquePtr<GObject::Object *[]>::adopt_ref (reinterpret_cast<GObject::Object **> (objects), count);
        _peel_this->DerivedClass::vfunc_pass_array_transfer_container (std::move (_peel_objects));
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_pass_array_transfer_none ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->pass_array_transfer_none = +[] (::TestTesty *self, ::GObject **objects, gsize count) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::ArrayRef<GObject::Object *> _peel_objects = peel::ArrayRef<GObject::Object *> (reinterpret_cast<GObject::Object **> (objects), count);
        _peel_this->DerivedClass::vfunc_pass_array_transfer_none (_peel_objects);
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_return_array_transfer_full ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->return_array_transfer_full = +[] (::TestTesty *self, gsize *count) -> ::GObject **
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::UniquePtr<peel::RefPtr<GObject::Object>[]> _peel_return = _peel_this->DerivedClass::vfunc_return_array_transfer_full ();
        return ((count ? (*count = _peel_return.size ()) : 0), reinterpret_cast<::GObject **> (std::move (_peel_return).release_ref ()));
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_return_array_transfer_container ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->return_array_transfer_container = +[] (::TestTesty *self, gsize *count) -> ::GObject **
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::UniquePtr<peel::RefPtr<GObject::Object>[]> _peel_return = _peel_this->DerivedClass::vfunc_return_array_transfer_container ();
        return ((count ? (*count = _peel_return.size ()) : 0), reinterpret_cast<::GObject **> (std::move (_peel_return).release_ref ()));
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_return_array_transfer_none ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->return_array_transfer_none = +[] (::TestTesty *self, gsize *count) -> ::GObject **
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::ArrayRef<GObject::Object *> _peel_return = _peel_this->DerivedClass::vfunc_return_array_transfer_none ();
        return ((count ? (*count = _peel_return.size ()) : 0), reinterpret_cast<::GObject **> (_peel_return.data ()));
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_inout_value_array ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->inout_value_array = +[] (::TestTesty *self, ::GValue **values, gsize *count) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::UniquePtr<GObject::Value[]> _peel_values = peel::UniquePtr<GObject::Value[]>::adopt_ref (reinterpret_cast<GObject::Value *> (*values), *count);
        _peel_this->DerivedClass::vfunc_inout_value_array (&_peel_values);
        *values = (*count = _peel_values.size (), reinterpret_cast<::GValue *> (std::move (_peel_values).release_ref ()));
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_make_instance ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->make_instance = +[] (int i) -> ::TestTesty *
      {
        peel::RefPtr<Testy> _peel_return = DerivedClass::vfunc_make_instance (i);
        return reinterpret_cast<::TestTesty *> (std::move (_peel_return).release_ref ());
      };
    }

    template<typename DerivedClass>
    void
    override_vfunc_out_gconstpointer ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->out_gconstpointer = +[] (::TestTesty *self, gconstpointer *ptr) -> void
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        _peel_this->DerivedClass::vfunc_out_gconstpointer (ptr);
      };
    }
  };

  static_assert (sizeof (Class) == sizeof (::TestTestyClass),
                 "Testy::Class size mismatch");
  static_assert (alignof (Class) == alignof (::TestTestyClass),
                 "Testy::Class align mismatch");
}; /* class Testy */

} /* namespace Test */
} /* namespace peel */

peel_end_header

#include <peel/GLib/Error.h>
#include <peel/GObject/Binding.h>
#include <peel/GObject/InitiallyUnowned.h>
#include <peel/GObject/Value.h>
