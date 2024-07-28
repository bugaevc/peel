#pragma once

#include <peel/lang.h>
#include <glib.h>

namespace peel
{
namespace internals
{

template<typename Ret, typename... Args>
class CallbackHelper
{
public:
  typedef Ret (*CallbackType) (Args..., gpointer);

  template<typename F, typename F2>
  static CallbackType
  wrap_notified_callback (F &&f, F2 &&f2, gpointer *out_data, GDestroyNotify *out_notify)
  {
    if (sizeof (F) <= sizeof (gpointer))
      {
        union U
          {
            F f;
            F2 *f2;
            gpointer data;
            U (gpointer data) : data (data) { }
            ~U () { }
          } u { nullptr };
        new (&u.f) F (static_cast<F &&> (f));
        *out_data = u.data;
        if (out_notify)
          *out_notify = +[] (gpointer data)
          {
            U u { data };
            u.f.~F ();
          };
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
        F *heap_f = new F (static_cast<F &&> (f));
        *out_data = heap_f;
        if (out_notify)
          *out_notify = +[] (gpointer data)
          {
            F *f = reinterpret_cast<F *> (data);
            delete f;
          };
        return +[] (Args... args, gpointer data) -> Ret
        {
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
  wrap_async_callback (F &&f, F2 &&f2, gpointer *out_data)
  {
    if (sizeof (F) <= sizeof (gpointer))
      {
        union U
          {
            F f;
            F2 *f2;
            gpointer data;
            U (gpointer data) : data (data) { }
            ~U () { }
          } u { nullptr };
        new (&u.f) F (static_cast<F &&> (f));
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
          u.f.~F ();
          return ret;
        };
      }
    else
      {
        F *heap_f = new F (static_cast<F &&> (f));
        *out_data = heap_f;
        return +[] (Args... args, gpointer data) -> Ret
        {
#ifdef peel_cpp_20
          F2 f2;
#else
          // Make up a fake instance of F2.
          F2 &f2 = *reinterpret_cast<F2 *> (data);
#endif
          Ret ret = f2 (args..., data);
          delete reinterpret_cast<F *> (data);
          return ret;
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
  wrap_notified_callback (F &&f, F2 &&f2, gpointer *out_data, GDestroyNotify *out_notify)
  {
    if (sizeof (F) <= sizeof (gpointer))
      {
        union U
          {
            F f;
            F2 *f2;
            gpointer data;
            U (gpointer data) : data (data) { }
            ~U () { }
          } u { nullptr };
        new (&u.f) F (static_cast<F &&> (f));
        *out_data = u.data;
        if (out_notify)
          *out_notify = +[] (gpointer data)
          {
            U u { data };
            u.f.~F ();
          };
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
        F *heap_f = new F (static_cast<F &&> (f));
        *out_data = heap_f;
        if (out_notify)
          *out_notify = +[] (gpointer data)
          {
            F *f = reinterpret_cast<F *> (data);
            delete f;
          };
        return +[] (Args... args, gpointer data) -> void
        {
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
  wrap_async_callback (F &&f, F2 &&f2, gpointer *out_data)
  {
    if (sizeof (F) <= sizeof (gpointer))
      {
        union U
          {
            F f;
            F2 *f2;
            gpointer data;
            U (gpointer data) : data (data) { }
            ~U () { }
          } u { nullptr };
        new (&u.f) F (static_cast<F &&> (f));
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
          u.f.~F ();
        };
      }
    else
      {
        F *heap_f = new F (static_cast<F &&> (f));
        *out_data = heap_f;
        return +[] (Args... args, gpointer data) -> void
        {
#ifdef peel_cpp_20
          F2 f2;
#else
          // Make up a fake instance of F2.
          F2 &f2 = *reinterpret_cast<F2 *> (data);
#endif
          f2 (args..., data);
          delete reinterpret_cast<F *> (data);
        };
      }
  }
};

} /* namespace internals */
} /* namespace peel */
