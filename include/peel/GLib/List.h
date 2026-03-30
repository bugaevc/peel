#pragma once

#include <peel/lang.h>
#include <peel/list.h>
#include <peel/callback.h>
#include <glib.h>

peel_begin_header

namespace peel
{
namespace GLib
{

template<typename T>
class ListRef
{
protected:
  ::GList *list;

public:
  class iterator;

  constexpr ListRef () noexcept
    : list (nullptr)
  { }

  ListRef (const ListRef &other) noexcept
    : list (other.list)
  { }

  ListRef &
  operator = (const ListRef &other) noexcept
  {
    list = other.list;
    return *this;
  }

  unsigned int
  length () const noexcept
  {
    return g_list_length (list);
  }

  template<typename Func>
  void
  foreach (Func &&func) noexcept
  {
    gpointer user_data;
    ::GFunc raw_func = internals::CallbackHelper<void, gpointer>::wrap_call_callback (
      static_cast<Func &&> (func),
      [] (gpointer data, gpointer user_data) -> void
      {
        Func &captured_func = *reinterpret_cast<typename std::remove_reference<Func>::type *> (user_data);
        typename internals::ListPack<T>::UnpackValueType item = internals::ListPack<T>::unpack_value (data);
        captured_func (static_cast<const T &> (item));
      },
      &user_data, internals::is_const_invocable<Func, void, const T &>::value);
    g_list_foreach (list, raw_func, user_data);
  }

  T
  nth (unsigned index) noexcept
  {
    gpointer data = g_list_nth_data (list, index);
    return internals::ListPack<T>::unpack_value (data);
  }

  explicit constexpr
  operator bool () const noexcept
  {
    return list;
  }

  constexpr ::GList *
  raw_list () noexcept
  {
    return list;
  }

  static ListRef
  from_raw_list (const ::GList *list) noexcept
  {
    ListRef l;
    l.list = const_cast<::GList *> (list);
    return l;
  }

  /* C++ iteration */

  class iterator final
  {
    friend class ListRef;
    ::GList *list;

    constexpr iterator (::GList *list) noexcept
      : list (list)
    { }

  public:
    constexpr typename internals::ListPack<T>::UnpackRefType
    operator * () const noexcept
    {
      return internals::ListPack<T>::unpack_ref (list->data);
    }

    constexpr T
    operator -> () const noexcept
    {
      return *this;
    }

    iterator &
    operator ++ () noexcept
    {
      list = list->next;
      return *this;
    }

    iterator
    operator ++ (int) noexcept
    {
      iterator other { list };
      list = list->next;
      return other;
    }

    iterator &
    operator -- () noexcept
    {
      list = list->prev;
      return *this;
    }

    iterator
    operator -- (int) noexcept
    {
      iterator other { list };
      list = list->prev;
      return other;
    }

    iterator &
    operator += (unsigned n) noexcept
    {
      /* Despite the "must point to the top of the list" comment on
       * g_list_nth, we believe it to be safe to call on any element.
       */
      list = g_list_nth (list, n);
      return *this;
    }

    iterator &
    operator -= (unsigned n) noexcept
    {
      list = g_list_nth_prev (list, n);
      return *this;
    }

    bool
    operator == (const iterator &other) const noexcept
    {
      return list == other.list;
    }

    bool
    operator != (const iterator &other) const noexcept
    {
      return list != other.list;
    }
  };

  typedef typename ListRef<const T>::iterator const_iterator;
  typedef T value_type;

  iterator
  begin () const noexcept
  {
    return iterator { list };
  }

  const_iterator
  cbegin () const noexcept
  {
    return const_iterator { list };
  }

  iterator
  end () const noexcept
  {
    return iterator { nullptr };
  }

  const_iterator
  cend () const noexcept
  {
    return const_iterator { nullptr };
  }

  iterator
  last () const noexcept
  {
    return iterator { g_list_last (list) };
  }
};

template<typename T>
class List final : public ListRef<T>
{
private:
  using ListRef<T>::from_raw_list;

public:
  constexpr List () noexcept
  { }

  List (List &&other) noexcept
  {
    this->list = other.list;
    other.list = nullptr;
  }

  template<typename U, typename... Us>
  peel_always_inline
  List (U &&first_item, Us &&...more_items) noexcept
    : List (static_cast<Us &&> (more_items)...)
  {
    this->list = g_list_prepend (this->list, internals::ListPack<T>::pack (static_cast<U &&> (first_item)));
    peel_assume (this->list);
  }

/*
  peel_always_inline
  explicit List (const List &other, typename std::enable_if<std::is_nothrow_copy_constructible<T>::value, int>::type = 0) noexcept
  {
    if (std::is_trivially_copy_constructible<T>::value)
      list = g_list_copy (other.list);
    else
      list = g_list_copy_deep (other.list, +[] (gconstpointer data, gpointer) -> gpointer
        {
          T copy { static_cast<const T &> (internals::ListPack<T>::unpack (data)) };
          return internals::ListPack<T>::pack (static_cast<T &&> (copy));
        }, nullptr);
  }
*/

  peel_always_inline
  List &
  operator = (List &&other) noexcept
  {
    if (this == &other)
      return *this;
    clear ();
    this->list = other.list;
    other.list = nullptr;
    return *this;
  }

  peel_always_inline
  ~List () noexcept
  {
    clear ();
  }

  constexpr
  operator ListRef<const T> () const noexcept
  {
    return ListRef<const T>::from_raw_list (this->list);
  }

  void
  append (T item) noexcept
  {
    gpointer data = internals::ListPack<T>::pack (static_cast<T &&> (item));
    this->list = g_list_append (this->list, data);
    peel_assume (this->list);
  }

  void
  prepend (T item) noexcept
  {
    gpointer data = internals::ListPack<T>::pack (static_cast<T &&> (item));
    this->list = g_list_prepend (this->list, data);
    peel_assume (this->list);
  }

  void
  insert (T item, int position) noexcept
  {
    gpointer data = internals::ListPack<T>::pack (static_cast<T &&> (item));
    this->list = g_list_insert (this->list, data, position);
    peel_assume (this->list);
  }

  void
  insert_before (ListRef<T>::iterator it, T item) noexcept
  {
    gpointer data = internals::ListPack<T>::pack (static_cast<T &&> (item));
    this->list = g_list_insert_before (this->list, it.list, data);
    peel_assume (this->list);
  }

  void
  concat (List other) noexcept
  {
    this->list = g_list_concat (this->list, other.list);
    other.list = nullptr;
  }

  void
  reverse () noexcept
  {
    this->list = g_list_reverse (this->list);
  }

  peel_always_inline
  void
  clear () noexcept
  {
    ::GList *l = this->list;
    this->list = nullptr;

#ifdef __GNUC__
    if (__builtin_constant_p (l == nullptr) && (l == nullptr))
      return;
#endif

    if (std::is_trivially_destructible<T>::value)
      g_list_free (l);
    else
      g_list_free_full (l, +[] (gpointer data)
        {
          reinterpret_cast<T *> (reinterpret_cast<void *> (&data))->~T ();
        });
  }

  template<typename CompareDataFunc>
  peel_always_inline
  void
  sort (CompareDataFunc &&compare_func) noexcept
  {
    gpointer user_data;
    ::GCompareDataFunc raw_compare_func = internals::CallbackHelper<gint, gconstpointer, gconstpointer>::wrap_call_callback (
      static_cast<CompareDataFunc &&> (compare_func),
      [] (gconstpointer a, gconstpointer b, gpointer user_data) -> gint
      {
        CompareDataFunc &captured_func = *reinterpret_cast<typename std::remove_reference<CompareDataFunc>::type *> (user_data);
        typename internals::ListPack<T>::UnpackValueType item_a = internals::ListPack<T>::unpack_value (a);
        typename internals::ListPack<T>::UnpackValueType item_b = internals::ListPack<T>::unpack_value (b);
        return captured_func (static_cast<const T &> (item_a), static_cast<const T &> (item_b));
      },
      &user_data, internals::is_const_invocable<CompareDataFunc, int, const T &, const T &>::value);
    this->list = g_list_sort_with_data (this->list, raw_compare_func, user_data);
  }

  static List
  adopt_list (::GList *list) noexcept
  {
    List l;
    l.list = list;
    return l;
  }

  peel_nodiscard ("the list will leak if unused")
  ::GList *
  release_list () && noexcept
  {
    ::GList *l = this->list;
    this->list = nullptr;
    return l;
  }
};

} /* namespace GLib */
} /* namespace peel */

peel_end_header
