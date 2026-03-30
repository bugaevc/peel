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
class SListRef
{
protected:
  ::GSList *list;

public:
  class iterator;

  constexpr SListRef () noexcept
    : list (nullptr)
  { }

  SListRef (const SListRef &other) noexcept
    : list (other.list)
  { }

  SListRef &
  operator = (const SListRef &other) noexcept
  {
    list = other.list;
    return *this;
  }

  unsigned int
  length () const noexcept
  {
    return g_slist_length (list);
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
    g_slist_foreach (list, raw_func, user_data);
  }

  T
  nth (unsigned index) noexcept
  {
    gpointer data = g_slist_nth_data (list, index);
    return internals::ListPack<T>::unpack_value (data);
  }

  explicit constexpr
  operator bool () const noexcept
  {
    return list;
  }

  constexpr ::GSList *
  raw_list () noexcept
  {
    return list;
  }

  static SListRef
  from_raw_list (const ::GSList *list) noexcept
  {
    SListRef l;
    l.list = const_cast<::GSList *> (list);
    return l;
  }

  /* C++ iteration */

  class iterator final
  {
    friend class SListRef;
    ::GSList *list;

    constexpr iterator (::GSList *list) noexcept
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
    operator += (unsigned n) noexcept
    {
      list = g_slist_nth (list, n);
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

  typedef typename SListRef<const T>::iterator const_iterator;
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
    return iterator { g_slist_last (list) };
  }
};

template<typename T>
class SList final : public SListRef<T>
{
private:
  using SListRef<T>::from_raw_list;

public:
  constexpr SList () noexcept
  { }

  SList (SList &&other) noexcept
  {
    this->list = other.list;
    other.list = nullptr;
  }

  template<typename U, typename... Us>
  peel_always_inline
  SList (U &&first_item, Us &&...more_items) noexcept
    : SList (static_cast<Us &&> (more_items)...)
  {
    this->list = g_slist_prepend (this->list, internals::ListPack<T>::pack (static_cast<U &&> (first_item)));
    peel_assume (this->list);
  }

  peel_always_inline
  SList &
  operator = (SList &&other) noexcept
  {
    if (this == &other)
      return *this;
    clear ();
    this->list = other.list;
    other.list = nullptr;
    return *this;
  }

  peel_always_inline
  ~SList () noexcept
  {
    clear ();
  }

  constexpr
  operator SListRef<const T> () const noexcept
  {
    return SListRef<const T>::from_raw_list (this->list);
  }

  void
  append (T item) noexcept
  {
    gpointer data = internals::ListPack<T>::pack (static_cast<T &&> (item));
    this->list = g_slist_append (this->list, data);
    peel_assume (this->list);
  }

  void
  prepend (T item) noexcept
  {
    gpointer data = internals::ListPack<T>::pack (static_cast<T &&> (item));
    this->list = g_slist_prepend (this->list, data);
    peel_assume (this->list);
  }

  void
  insert (T item, int position) noexcept
  {
    gpointer data = internals::ListPack<T>::pack (static_cast<T &&> (item));
    this->list = g_slist_insert (this->list, data, position);
    peel_assume (this->list);
  }

  void
  insert_before (SListRef<T>::iterator it, T item) noexcept
  {
    gpointer data = internals::ListPack<T>::pack (static_cast<T &&> (item));
    this->list = g_slist_insert_before (this->list, it.list, data);
    peel_assume (this->list);
  }

  void
  concat (SList other) noexcept
  {
    this->list = g_slist_concat (this->list, other.list);
    other.list = nullptr;
  }

  peel_always_inline
  void
  clear () noexcept
  {
    ::GSList *l = this->list;
    this->list = nullptr;

#ifdef __GNUC__
    if (__builtin_constant_p (l == nullptr) && (l == nullptr))
      return;
#endif

    if (std::is_trivially_destructible<T>::value)
      g_slist_free (l);
    else
      g_slist_free_full (l, +[] (gpointer data)
        {
          reinterpret_cast<T *> (reinterpret_cast<void *> (&data))->~T ();
        });
  }

  static SList
  adopt_list (::GSList *list) noexcept
  {
    SList l;
    l.list = list;
    return l;
  }

  peel_nodiscard ("the list will leak if unused")
  ::GSList *
  release_list () && noexcept
  {
    ::GSList *l = this->list;
    this->list = nullptr;
    return l;
  }
};

} /* namespace GLib */
} /* namespace peel */

peel_end_header
