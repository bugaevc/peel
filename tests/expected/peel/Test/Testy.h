#pragma once

/* Auto-generated, do not modify */
/* Package peel-test */

#include <peel/GObject/Type.h>
#include <peel/RefPtr.h>
#include <peel/FloatPtr.h>
#include <peel/UniquePtr.h>
#include <peel/ArrayRef.h>
#include <peel/signal.h>
#include <peel/callback.h>
#include <peel/property.h>
#include <peel/lang.h>
#include <cstdint>
#include <utility>
#include <peel-test/peel-test.h>
#include <peel/GObject/BindingFlags.h>
#include <peel/GObject/Object.h>
#include <peel/GObject/ObjectConstructParam.h>

G_GNUC_BEGIN_IGNORE_DEPRECATIONS

namespace peel
{
namespace GObject
{
class Binding;
enum class BindingFlags : std::underlying_type<::GBindingFlags>::type;
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

protected:
  ~Testy () = default;

public:

  peel_nothrow
  static peel::RefPtr<Testy>
  create ()
  {
    ::TestTesty * _peel_return = test_testy_new ();
    peel_assume (_peel_return);
    return peel::RefPtr<Testy>::adopt_ref (reinterpret_cast<Testy *> (_peel_return));
  }

  peel_nothrow
  int
  get_int ()
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    return test_testy_get_int (_peel_this);
  }

  peel_nothrow
  void
  pass_bool (bool b)
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gboolean _peel_b = static_cast<gboolean> (b);
    test_testy_pass_bool (_peel_this, _peel_b);
  }

  peel_nothrow
  void
  pass_gtype (GObject::Type gt)
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    test_testy_pass_gtype (_peel_this, gt);
  }

  peel_nothrow peel_arg_out (2) peel_arg_out (3) peel_nonnull_args (2, 3)
  void
  get_out_int (int *int_ptr, int *int_ptr2)
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    test_testy_get_out_int (_peel_this, int_ptr, int_ptr2);
  }

  peel_nothrow peel_arg_out (2) peel_nonnull_args (2)
  void
  get_out_bool (bool *b)
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gboolean _peel_b;
    test_testy_get_out_bool (_peel_this, &_peel_b);
    *b = !!_peel_b;
  }

  peel_nothrow peel_arg_out (2) peel_nonnull_args (2)
  void
  get_out_gtype (GObject::Type *gt)
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    GType * _peel_gt = reinterpret_cast<GType *> (gt);
    test_testy_get_out_gtype (_peel_this, _peel_gt);
  }

  peel_nothrow peel_arg_out (2) peel_nonnull_args (2)
  void
  get_out_object (GObject::Object **obj_ptr)
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_obj_ptr = reinterpret_cast<::GObject **> (obj_ptr);
    test_testy_get_out_object (_peel_this, _peel_obj_ptr);
  }

  peel_nothrow peel_arg_out (2)
  void
  get_out_owned_object (peel::RefPtr<GObject::Object> *obj_ptr)
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject * _peel_obj_ptr;
    test_testy_get_out_owned_object (_peel_this, &_peel_obj_ptr);
    *obj_ptr = peel::RefPtr<GObject::Object>::adopt_ref (reinterpret_cast<GObject::Object *> (_peel_obj_ptr));
  }

  peel_nothrow peel_arg_in (2) peel_nonnull_args (2)
  void
  pass_value (const GObject::Value *v)
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    const ::GValue * _peel_v = reinterpret_cast<const ::GValue *> (v);
    test_testy_pass_value (_peel_this, _peel_v);
  }

  peel_nothrow peel_arg_out (2) peel_nonnull_args (2)
  void
  get_out_value (GObject::Value *value_ptr)
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GValue * _peel_value_ptr = reinterpret_cast<::GValue *> (value_ptr);
    test_testy_get_out_value (_peel_this, _peel_value_ptr);
  }

  peel_nothrow peel_arg_in (2) peel_nonnull_args (2)
  void
  nested_types (GObject::Object::ConstructParam *construct_param, GObject::Binding::Flags binding_flags)
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObjectConstructParam * _peel_construct_param = reinterpret_cast<::GObjectConstructParam *> (construct_param);
    ::GObjectBindingFlags _peel_binding_flags = static_cast<::GObjectBindingFlags> (binding_flags);
    test_testy_nested_types (_peel_this, _peel_construct_param, _peel_binding_flags);
  }

  template<typename CoolCallback>
  peel_nothrow
  int
  add_cool_callback (peel::RefPtr<GObject::Binding> binding, /* owned */ char *s, CoolCallback &&callback)
  {
    gpointer _peel_user_data;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GBinding * _peel_binding = reinterpret_cast<::GBinding *> (std::move (binding).release_ref ());
    ::TestCoolCallback _peel_callback = peel::internals::CallbackHelper<::TestTesty *, gboolean *>::wrap_async_callback (
      static_cast<CoolCallback &&> (callback),
      [] (gboolean *b, gpointer user_data) -> ::TestTesty *
      {
        CoolCallback &callback = reinterpret_cast<CoolCallback &> (*reinterpret_cast<unsigned char *> (user_data));
        bool _peel_b;
        peel::RefPtr<Testy> _peel_return = static_cast<CoolCallback &&> (callback) (&_peel_b);
        *b = static_cast<gboolean> (_peel_b);
        return reinterpret_cast<::TestTesty *> (std::move (_peel_return).release_ref ());
      },
      &_peel_user_data);
    return test_testy_add_cool_callback (_peel_this, _peel_binding, s, _peel_callback, _peel_user_data);
  }

  peel_nothrow
  peel::ArrayRef<const uint8_t>
  return_byte_array ()
  {
    gsize _peel_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gconstpointer _peel_return = test_testy_return_byte_array (_peel_this, &_peel_size);
    peel_assume (_peel_return);
    return peel::ArrayRef<const uint8_t> (reinterpret_cast<const uint8_t *> (_peel_return), _peel_size);
  }

  peel_nothrow
  peel::UniquePtr<uint8_t[]>
  return_owned_byte_array ()
  {
    gsize _peel_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gpointer _peel_return = test_testy_return_owned_byte_array (_peel_this, &_peel_size);
    peel_assume (_peel_return);
    return peel::UniquePtr<uint8_t[]>::adopt_ref (reinterpret_cast<uint8_t *> (_peel_return), _peel_size);
  }

  peel_nothrow
  const float
  (&return_fixed_size_array ())[4]
  {
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    const float * _peel_return = test_testy_return_fixed_size_array (_peel_this);
    peel_assume (_peel_return);
    return reinterpret_cast<const float (&)[4]> (*_peel_return);
  }

  peel_nothrow
  peel::ArrayRef<const GObject::Binding::Flags>
  return_flags_array ()
  {
    gsize _peel_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    const ::GBindingFlags * _peel_return = test_testy_return_flags_array (_peel_this, &_peel_size);
    peel_assume (_peel_return);
    return peel::ArrayRef<const GObject::Binding::Flags> (reinterpret_cast<const GObject::Binding::Flags *> (_peel_return), _peel_size);
  }

  peel_nothrow
  size_t
  read_in (peel::ArrayRef<uint8_t> buffer)
  {
    gsize _peel_buffer_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    void * _peel_buffer = (_peel_buffer_size = buffer.count (), reinterpret_cast<void *> (buffer.ptr ()));
    return test_testy_read_in (_peel_this, _peel_buffer, _peel_buffer_size);
  }

  peel_nothrow
  void
  read_out (peel::ArrayRef<uint8_t> *buffer)
  {
    gsize _peel_buffer_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    void * _peel_buffer;
    test_testy_read_out (_peel_this, &_peel_buffer, &_peel_buffer_size);
    *buffer = peel::ArrayRef<uint8_t> (reinterpret_cast<uint8_t *> (_peel_buffer), _peel_buffer_size);
  }

protected:
  template<typename DerivedClass>
  peel_nothrow
  int
  parent_vfunc_get_int ()
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (Class::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    return _peel_class->get_int (_peel_this);
  }

  template<typename DerivedClass>
  peel_nothrow
  void
  parent_vfunc_pass_bool (bool b)
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (Class::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gboolean _peel_b = static_cast<gboolean> (b);
    _peel_class->pass_bool (_peel_this, _peel_b);
  }

  template<typename DerivedClass>
  peel_nothrow peel_arg_out (2) peel_arg_out (3) peel_nonnull_args (2, 3)
  void
  parent_vfunc_get_out_int (int *int_ptr, int *int_ptr2)
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (Class::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    _peel_class->get_out_int (_peel_this, int_ptr, int_ptr2);
  }

  template<typename DerivedClass>
  peel_nothrow peel_arg_out (2) peel_nonnull_args (2)
  void
  parent_vfunc_get_out_bool (bool *b)
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (Class::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gboolean _peel_b;
    _peel_class->get_out_bool (_peel_this, &_peel_b);
    *b = !!_peel_b;
  }

  template<typename DerivedClass>
  peel_nothrow peel_arg_out (2) peel_nonnull_args (2)
  void
  parent_vfunc_get_out_object (GObject::Object **obj_ptr)
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (Class::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject ** _peel_obj_ptr = reinterpret_cast<::GObject **> (obj_ptr);
    _peel_class->get_out_object (_peel_this, _peel_obj_ptr);
  }

  template<typename DerivedClass>
  peel_nothrow peel_arg_out (2)
  void
  parent_vfunc_get_out_owned_object (peel::RefPtr<GObject::Object> *obj_ptr)
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (Class::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GObject * _peel_obj_ptr;
    _peel_class->get_out_owned_object (_peel_this, &_peel_obj_ptr);
    *obj_ptr = peel::RefPtr<GObject::Object>::adopt_ref (reinterpret_cast<GObject::Object *> (_peel_obj_ptr));
  }

  template<typename DerivedClass>
  peel_nothrow peel_arg_out (2) peel_nonnull_args (2)
  void
  parent_vfunc_get_out_value (GObject::Value *value_ptr)
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (Class::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GValue * _peel_value_ptr = reinterpret_cast<::GValue *> (value_ptr);
    _peel_class->get_out_value (_peel_this, _peel_value_ptr);
  }

  template<typename DerivedClass, typename CoolCallback>
  peel_nothrow
  int
  parent_vfunc_add_cool_callback (peel::RefPtr<GObject::Binding> binding, /* owned */ char *s, CoolCallback &&callback)
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (Class::peek<DerivedClass> ()->peek_parent ());
    gpointer _peel_user_data;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    ::GBinding * _peel_binding = reinterpret_cast<::GBinding *> (std::move (binding).release_ref ());
    ::TestCoolCallback _peel_callback = peel::internals::CallbackHelper<::TestTesty *, gboolean *>::wrap_async_callback (
      static_cast<CoolCallback &&> (callback),
      [] (gboolean *b, gpointer user_data) -> ::TestTesty *
      {
        CoolCallback &callback = reinterpret_cast<CoolCallback &> (*reinterpret_cast<unsigned char *> (user_data));
        bool _peel_b;
        peel::RefPtr<Testy> _peel_return = static_cast<CoolCallback &&> (callback) (&_peel_b);
        *b = static_cast<gboolean> (_peel_b);
        return reinterpret_cast<::TestTesty *> (std::move (_peel_return).release_ref ());
      },
      &_peel_user_data);
    return _peel_class->add_cool_callback (_peel_this, _peel_binding, s, _peel_callback, _peel_user_data);
  }

  template<typename DerivedClass>
  peel_nothrow
  peel::ArrayRef<const uint8_t>
  parent_vfunc_return_byte_array ()
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (Class::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gconstpointer _peel_return = _peel_class->return_byte_array (_peel_this, &_peel_size);
    peel_assume (_peel_return);
    return peel::ArrayRef<const uint8_t> (reinterpret_cast<const uint8_t *> (_peel_return), _peel_size);
  }

  template<typename DerivedClass>
  peel_nothrow
  peel::UniquePtr<uint8_t[]>
  parent_vfunc_return_owned_byte_array ()
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (Class::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    gpointer _peel_return = _peel_class->return_owned_byte_array (_peel_this, &_peel_size);
    peel_assume (_peel_return);
    return peel::UniquePtr<uint8_t[]>::adopt_ref (reinterpret_cast<uint8_t *> (_peel_return), _peel_size);
  }

  template<typename DerivedClass>
  peel_nothrow
  const float
  (&parent_vfunc_return_fixed_size_array ())[4]
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (Class::peek<DerivedClass> ()->peek_parent ());
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    const float * _peel_return = _peel_class->return_fixed_size_array (_peel_this);
    peel_assume (_peel_return);
    return reinterpret_cast<const float (&)[4]> (*_peel_return);
  }

  template<typename DerivedClass>
  peel_nothrow
  peel::ArrayRef<const GObject::Binding::Flags>
  parent_vfunc_return_flags_array ()
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (Class::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    const ::GBindingFlags * _peel_return = _peel_class->return_flags_array (_peel_this, &_peel_size);
    peel_assume (_peel_return);
    return peel::ArrayRef<const GObject::Binding::Flags> (reinterpret_cast<const GObject::Binding::Flags *> (_peel_return), _peel_size);
  }

  template<typename DerivedClass>
  peel_nothrow
  size_t
  parent_vfunc_read_in (peel::ArrayRef<uint8_t> buffer)
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (Class::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_buffer_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    void * _peel_buffer = (_peel_buffer_size = buffer.count (), reinterpret_cast<void *> (buffer.ptr ()));
    return _peel_class->read_in (_peel_this, _peel_buffer, _peel_buffer_size);
  }

  template<typename DerivedClass>
  peel_nothrow
  void
  parent_vfunc_read_out (peel::ArrayRef<uint8_t> *buffer)
  {
    ::TestTestyClass *_peel_class = reinterpret_cast<::TestTestyClass *> (Class::peek<DerivedClass> ()->peek_parent ());
    gsize _peel_buffer_size;
    ::TestTesty * _peel_this = reinterpret_cast<::TestTesty *> (this);
    void * _peel_buffer;
    _peel_class->read_out (_peel_this, &_peel_buffer, &_peel_buffer_size);
    *buffer = peel::ArrayRef<uint8_t> (reinterpret_cast<uint8_t *> (_peel_buffer), _peel_buffer_size);
  }

  class Class : public GObject::Object::Class
  {
  private:
    Class () = delete;
    Class (const Class &) = delete;
    Class (Class &&) = delete;

  public:
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

    /* Unsupported for now: add_cool_callback: casting callback from C to C++ */

    template<typename DerivedClass>
    void
    override_vfunc_return_byte_array ()
    {
      ::TestTestyClass *klass = reinterpret_cast<::TestTestyClass *> (this);
      klass->return_byte_array = +[] (::TestTesty *self, gsize *size) -> gconstpointer
      {
        DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);
        peel::ArrayRef<const uint8_t> _peel_return = _peel_this->DerivedClass::vfunc_return_byte_array ();
        return (*size = _peel_return.count (), reinterpret_cast<gconstpointer> (_peel_return.ptr ()));
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
        return (*size = _peel_return.count (), reinterpret_cast<gpointer> (std::move (_peel_return).release_ref ()));
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
        return (*size = _peel_return.count (), reinterpret_cast<const ::GBindingFlags *> (_peel_return.ptr ()));
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
        *buffer = (*buffer_size = _peel_buffer.count (), reinterpret_cast<void *> (_peel_buffer.ptr ()));
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

G_GNUC_END_IGNORE_DEPRECATIONS

#include <peel/GObject/Binding.h>
