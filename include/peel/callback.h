#pragma once

#include <peel/lang.h>
#include <utility>
#include <glib.h>

namespace peel
{
namespace internals
{

template<typename Ret, typename F>
F &&
invoke_if_nonnull (F &&f)
{
  return static_cast<F &&> (f);
}

template<typename Ret>
Ret
(*invoke_if_nonnull (decltype (nullptr))) (...)
{
  peel_unreachable;
  return nullptr;
}

template<typename F, typename, typename... Args>
struct is_const_invocable
{
  static constexpr bool value = false;
};

template<typename F, typename... Args>
struct is_const_invocable<F, void_t<decltype (std::declval<const typename std::remove_reference<F>::type &> () (std::declval<Args> ()...))>, Args...>
{
  static constexpr bool value = true;
};

template<typename Ret, typename... Args>
class CallbackHelper
{
public:
  typedef Ret (*CallbackType) (Args..., gpointer);

  template<typename F, typename F2>
  static CallbackType
  wrap_notified_callback (F &&f, F2 &&, gpointer *out_data, GDestroyNotify *out_notify, bool const_invocable)
  {
    typedef typename std::remove_reference<F>::type C;
    if (std::is_same<C, decltype (nullptr)>::value)
      {
        if (out_notify)
          *out_notify = nullptr;
        *out_data = nullptr;
        return nullptr;
      }
    if (sizeof (C) <= sizeof (gpointer) && const_invocable)
      {
        union U
          {
            C f;
            F2 *f2;
            gpointer data;
            U (gpointer data) : data (data) { }
            ~U () { }
          } u { nullptr };
        new (&u.f) C (static_cast<F &&> (f));
        *out_data = u.data;
        if (out_notify)
          {
            if (std::is_trivially_destructible<C>::value)
              *out_notify = nullptr;
            else
              *out_notify = +[] (gpointer data)
              {
                U u { data };
                u.f.~C ();
              };
          }
        return +[] (Args... args, gpointer data) -> Ret
        {
          U u { data };
#ifdef peel_cpp_20
          F2 f2;
#else
          // Make up a fake instance of F2.
          F2 &f2 = *u.f2;
#endif
          return f2 (args..., &u.data);
        };
      }
    else
      {
        C *heap_f = new C (static_cast<F &&> (f));
        *out_data = heap_f;
        if (out_notify)
          *out_notify = +[] (gpointer data)
          {
            peel_assume (data);
            C *f = reinterpret_cast<C *> (data);
            delete f;
          };
        return +[] (Args... args, gpointer data) -> Ret
        {
          peel_assume (data);
#ifdef peel_cpp_20
          F2 f2;
#else
          // Make up a fake instance of F2.
          F2 &f2 = *reinterpret_cast<F2 *> (data);
#endif
          return f2 (args..., data);
        };
      }
  }

  template<typename F, typename F2>
  static CallbackType
  wrap_async_callback (F &&f, F2 &&, gpointer *out_data)
  {
    typedef typename std::remove_reference<F>::type C;
    if (std::is_same<C, decltype (nullptr)>::value)
      {
        *out_data = nullptr;
        return nullptr;
      }
    if (sizeof (C) <= sizeof (gpointer))
      {
        union U
          {
            C f;
            F2 *f2;
            gpointer data;
            U (gpointer data) : data (data) { }
            ~U () { }
          } u { nullptr };
        new (&u.f) C (static_cast<F &&> (f));
        *out_data = u.data;
        return +[] (Args... args, gpointer data) -> Ret
        {
          U u { data };
#ifdef peel_cpp_20
          F2 f2;
#else
          // Make up a fake instance of F2.
          F2 &f2 = *u.f2;
#endif
          Ret ret = f2 (args..., &u.data);
          u.f.~C ();
          return ret;
        };
      }
    else
      {
        C *heap_f = new C (static_cast<F &&> (f));
        *out_data = heap_f;
        return +[] (Args... args, gpointer data) -> Ret
        {
          peel_assume (data);
#ifdef peel_cpp_20
          F2 f2;
#else
          // Make up a fake instance of F2.
          F2 &f2 = *reinterpret_cast<F2 *> (data);
#endif
          Ret ret = f2 (args..., data);
          delete reinterpret_cast<C *> (data);
          return ret;
        };
      }
  }

  template<typename F, typename F2>
  static CallbackType
  wrap_gsourcefunc_callback (F &&f, F2 &&, gpointer *out_data, bool const_invocable)
  {
    typedef typename std::remove_reference<F>::type C;
    if (std::is_same<C, decltype (nullptr)>::value)
      {
        *out_data = nullptr;
        return nullptr;
      }
    if (sizeof (C) <= sizeof (gpointer) && const_invocable)
      {
        union U
          {
            C f;
            F2 *f2;
            gpointer data;
            U (gpointer data) : data (data) { }
            ~U () { }
          } u { nullptr };
        new (&u.f) C (static_cast<F &&> (f));
        *out_data = u.data;
        return +[] (Args... args, gpointer data) -> gboolean
        {
          U u { data };
#ifdef peel_cpp_20
          F2 f2;
#else
          // Make up a fake instance of F2.
          F2 &f2 = *u.f2;
#endif
          gboolean again = f2 (args..., &u.data);
          if (!again)
            u.f.~C ();
          return again;
        };
      }
    else
      {
        C *heap_f = new C (static_cast<F &&> (f));
        *out_data = heap_f;
        return +[] (Args... args, gpointer data) -> gboolean
        {
          peel_assume (data);
#ifdef peel_cpp_20
          F2 f2;
#else
          // Make up a fake instance of F2.
          F2 &f2 = *reinterpret_cast<F2 *> (data);
#endif
          gboolean again = f2 (args..., data);
          if (!again)
            delete reinterpret_cast<C *> (data);
          return again;
        };
      }
  }
};

template<typename... Args>
class CallbackHelper<void, Args...>
{
public:
  typedef void (*CallbackType) (Args..., gpointer);

  template<typename F, typename F2>
  static CallbackType
  wrap_notified_callback (F &&f, F2 &&, gpointer *out_data, GDestroyNotify *out_notify, bool const_invocable)
  {
    typedef typename std::remove_reference<F>::type C;
    if (std::is_same<C, decltype (nullptr)>::value)
      {
        if (out_notify)
          *out_notify = nullptr;
        *out_data = nullptr;
        return nullptr;
      }
    if (sizeof (C) <= sizeof (gpointer) && const_invocable)
      {
        union U
          {
            C f;
            F2 *f2;
            gpointer data;
            U (gpointer data) : data (data) { }
            ~U () { }
          } u { nullptr };
        new (&u.f) C (static_cast<F &&> (f));
        *out_data = u.data;
        if (out_notify)
          {
            if (std::is_trivially_destructible<C>::value)
              *out_notify = nullptr;
            else
              *out_notify = +[] (gpointer data)
              {
                U u { data };
                u.f.~C ();
              };
          }
        return +[] (Args... args, gpointer data) -> void
        {
          U u { data };
#ifdef peel_cpp_20
          F2 f2;
#else
          // Make up a fake instance of F2.
          F2 &f2 = *u.f2;
#endif
          f2 (args..., &u.data);
        };
      }
    else
      {
        C *heap_f = new C (static_cast<F &&> (f));
        *out_data = heap_f;
        if (out_notify)
          *out_notify = +[] (gpointer data)
          {
            peel_assume (data);
            C *f = reinterpret_cast<C *> (data);
            delete f;
          };
        return +[] (Args... args, gpointer data) -> void
        {
          peel_assume (data);
#ifdef peel_cpp_20
          F2 f2;
#else
          // Make up a fake instance of F2.
          F2 &f2 = *reinterpret_cast<F2 *> (data);
#endif
          f2 (args..., data);
        };
      }
  }

  template<typename F, typename F2>
  static CallbackType
  wrap_async_callback (F &&f, F2 &&, gpointer *out_data)
  {
    typedef typename std::remove_reference<F>::type C;
    if (std::is_same<C, decltype (nullptr)>::value)
      {
        *out_data = nullptr;
        return nullptr;
      }
    if (sizeof (C) <= sizeof (gpointer))
      {
        union U
          {
            C f;
            F2 *f2;
            gpointer data;
            U (gpointer data) : data (data) { }
            ~U () { }
          } u { nullptr };
        new (&u.f) C (static_cast<F &&> (f));
        *out_data = u.data;
        return +[] (Args... args, gpointer data) -> void
        {
          U u { data };
#ifdef peel_cpp_20
          F2 f2;
#else
          // Make up a fake instance of F2.
          F2 &f2 = *u.f2;
#endif
          f2 (args..., &u.data);
          u.f.~C ();
        };
      }
    else
      {
        C *heap_f = new C (static_cast<F &&> (f));
        *out_data = heap_f;
        return +[] (Args... args, gpointer data) -> void
        {
          peel_assume (data);
#ifdef peel_cpp_20
          F2 f2;
#else
          // Make up a fake instance of F2.
          F2 &f2 = *reinterpret_cast<F2 *> (data);
#endif
          f2 (args..., data);
          delete reinterpret_cast<C *> (data);
        };
      }
  }
};

} /* namespace internals */
} /* namespace peel */
