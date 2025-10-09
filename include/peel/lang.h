#pragma once

#include <type_traits>
#include <utility>

#if defined (__has_attribute)
#define peel_has_attribute(attr) __has_attribute (attr)
#else
#define peel_has_attribute(attr) 0
#endif

#if __cplusplus >= 202002L
#define peel_cpp_20 1
#endif
#if __cplusplus >= 201703L
#define peel_cpp_17 1
#endif
#if __cplusplus >= 201402L
#define peel_cpp_14 1
#endif

#if defined (peel_cpp_20)
#define peel_nodiscard(reason) [[nodiscard (reason)]]
#elif defined (peel_cpp_17)
#define peel_nodiscard(reason) [[nodiscard]]
#elif defined (__GNUC__)
#define peel_nodiscard(reason) __attribute__ ((warn_unused_result))
/* TODO: _Check_return_ on MSVC, but needs <sal.h> */
#else
#define peel_nodiscard(reason)
#endif

#ifdef __GNUC__
#define peel_no_warn_unused __attribute__ ((unused))
#else
#define peel_no_warn_unused
#endif

#ifdef __GNUC__
#define peel_always_inline __attribute__ ((always_inline)) inline
#define peel_assume(expr) do { if (!(expr)) __builtin_unreachable (); } while (0)
#elif defined (_MSC_VER)
#define peel_always_inline __forceinline
#define peel_assume(expr) __assume (expr)
#else
#define peel_always_inline inline
#define peel_assume(expr) do { } while (0)
#endif

#ifdef __cpp_lib_unreachable
#define peel_unreachable std::unreachable ()
#elif defined (__GNUC__)
#define peel_unreachable __builtin_unreachable ()
#else
#define peel_unreachable peel_assume (false)
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
#define peel_returns_nonnull __attribute__ ((returns_nonnull))
#else
#define peel_nonnull_args(...)
#define peel_returns_nonnull
#endif

#if defined (peel_cpp_14)
#define peel_is_final(T) std::is_final<T>::value
#else
/* Works on all the compilers we care about.  */
#define peel_is_final(T) __is_final(T)
#endif

#if defined (__GNUC__)
#define _peel_diagnostic_push_ignored_offsetof                                 \
  _Pragma ("GCC diagnostic push");                                             \
  _Pragma ("GCC diagnostic ignored \"-Winvalid-offsetof\"")
#define _peel_diagnostic_pop _Pragma ("GCC diagnostic pop")
#else
#define _peel_diagnostic_push_ignored_offsetof
#define _peel_diagnostic_pop
#endif

#if defined (_MSC_VER)
/* Silence "consversion from size_t to int, possible loss of data" arising
 * from our usage of ArrayRef, and various functions accepting different
 * size types.
 */
#define peel_begin_header                                                      \
  __pragma (warning (push))                                                    \
  __pragma (warning (disable : 4996 4267))
#define peel_end_header                                                        \
  __pragma (warning (pop))
#elif defined (__GNUC__)
#define peel_begin_header                                                      \
  _Pragma ("GCC diagnostic push")                                              \
  _Pragma ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#define peel_end_header                                                        \
  _Pragma ("GCC diagnostic pop")
#else
#define peel_begin_header
#define peel_end_header
#endif

#define peel_macro_overload_23(_1, _2, _3, macro, ...) macro
#define peel_expand_for_msvc(x) x

#ifndef _Bool
#define _Bool bool
#endif

peel_begin_header

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

} /* namespace peel */

peel_end_header
