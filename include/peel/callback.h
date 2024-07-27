#pragma once

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

  template<typename F>
  static CallbackType
  wrap_notified_callback (F f, gpointer *out_data, GDestroyNotify *out_notify)
  {
    if (sizeof (F) <= sizeof (gpointer))
      {
        union U
          {
            F f;
            gpointer data;
            U () { }
            ~U () { }
          } u;
        new (&u.f) F (static_cast<F &&> (f));
        *out_data = u.data;
        if (out_notify)
          *out_notify = +[] (gpointer data)
          {
            U u;
            u.data = data;
            u.f.~F ();
          };
        return +[] (Args... args, gpointer data) -> Ret
        {
          U u;
          u.data = data;
          return u.f (args...);
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
          F &f = *reinterpret_cast<F *> (data);
          return f (args...);
        };
      }
  }

  template<typename F>
  static CallbackType
  wrap_async_callback (F f, gpointer *out_data)
  {
    if (sizeof (F) <= sizeof (gpointer))
      {
        union U
          {
            F f;
            gpointer data;
            U () { }
            ~U () { }
          } u;
        new (&u.f) F (static_cast<F &&> (f));
        *out_data = u.data;
        return +[] (Args... args, gpointer data) -> Ret
        {
          U u;
          u.data = data;
          Ret ret = u.f (args...);
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
          F &f = *reinterpret_cast<F *> (data);
          Ret ret = f (args...);
          delete &f;
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

  template<typename F>
  static CallbackType
  wrap_notified_callback (F f, gpointer *out_data, GDestroyNotify *out_notify)
  {
    if (sizeof (F) <= sizeof (gpointer))
      {
        union U
          {
            F f;
            gpointer data;
            U () { }
            ~U () { }
          } u;
        new (&u.f) F (static_cast<F &&> (f));
        *out_data = u.data;
        if (out_notify)
          *out_notify = +[] (gpointer data)
          {
            U u;
            u.data = data;
            u.f.~F ();
          };
        return +[] (Args... args, gpointer data) -> void
        {
          U u;
          u.data = data;
          u.f (args...);
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
          F &f = *reinterpret_cast<F *> (data);
          f (args...);
        };
      }
  }

  template<typename F>
  static CallbackType
  wrap_async_callback (F f, gpointer *out_data)
  {
    if (sizeof (F) <= sizeof (gpointer))
      {
        union U
          {
            F f;
            gpointer data;
            U () { }
            ~U () { }
          } u;
        new (&u.f) F (static_cast<F &&> (f));
        *out_data = u.data;
        return +[] (Args... args, gpointer data) -> void
        {
          U u;
          u.data = data;
          u.f (args...);
          u.f.~F ();
        };
      }
    else
      {
        F *heap_f = new F (static_cast<F &&> (f));
        *out_data = heap_f;
        return +[] (Args... args, gpointer data) -> void
        {
          F &f = *reinterpret_cast<F *> (data);
          f (args...);
          delete &f;
        };
      }
  }
};

} /* namespace internals */
} /* namespace peel */
