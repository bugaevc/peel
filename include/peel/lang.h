#pragma once

#include <type_traits>

#if defined (__has_attribute)
#define peel_has_attribute(attr) __has_attribute (attr)
#else
#define peel_has_attribute(attr) 0
#endif

#if __cplusplus >= 202002L
#define peel_nodiscard(reason) [[nodiscard (reason)]]
#else
#define peel_nodiscard(reason) [[nodiscard]]
#endif

#ifdef __GNUC__
#define peel_assume(expr) do { if (!(expr)) __builtin_unreachable (); } while (0)
#define peel_nothrow __attribute__ ((nothrow))
#define peel_no_warn_unused __attribute__ ((unused))
#else
#define peel_assume(expr) do { } while (0)
#define peel_nothrow
#define peel_no_warn_unused
#endif

#ifdef __GNUC__
#define peel_always_inline __attribute__ ((always_inline))
#elif defined (_MSC_VER)
#define peel_always_inline __forceinline
#else
#define peel_always_inline
#endif

#if defined (__GNUC__) && __GNUC__ >= 10
#define peel_arg_in(index) __attribute__ ((access (read_only, (index))))
#define peel_arg_out(index) __attribute__ ((access (write_only, (index))))
#define peel_arg_inout(index) __attribute__ ((access (read_write, (index))))
#else
#define peel_arg_in(index)
#define peel_arg_out(index)
#define peel_arg_inout(index)
#endif

#if defined (__GNUC__)
#define peel_nonnull_args(...) __attribute__ ((nonnull (__VA_ARGS__)))
#else
#define peel_nonnull_args(...)
#endif

#ifndef _Bool
#define _Bool bool
#endif

namespace peel
{

template<typename...>
using void_t = void;

template<typename Base, typename Derived, typename T = void>
using enable_if_derived = typename std::enable_if<std::is_base_of<Base, Derived>::value, T>::type;

template<typename Class, typename Member, typename BaseClass>
constexpr static bool
has_own_member (Member BaseClass::*)
{
  return std::is_same<Class, BaseClass>::value;
}

struct UnsupportedForNowToken
{
private:
  UnsupportedForNowToken () = delete;
};

#if 0
template<typename T, typename U>
/* not constexpr */ size_t G_GNUC_CONST
offset_of_member_ptr (U T::*mptr)
{
  // Try to invoke as little UB as possible, while getting
  // the best generated code.
  T *value = reinterpret_cast<T *> (sizeof (T));
  return reinterpret_cast<uintptr_t> (&(value->*mptr)) - sizeof (T);
}
#endif

}
