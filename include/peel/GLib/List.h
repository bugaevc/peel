#pragma once

#include <peel/lang.h>
#include <glib.h>
#include <new>

peel_begin_header

namespace peel
{

namespace internals
{

template<typename T, typename = void>
struct ListPack
{
private:
  union U
    {
      T value;
      gpointer ptr;
      U () : ptr (nullptr) { }
      ~U () { }
    };

public:
  typedef T &unpack_type;

  static gpointer
  pack (T &&value) noexcept
  {
    U u;
    new (&u.value) T (static_cast<T &&> (value));
    return u.ptr;
    if (std::is_pointer<T>::value)
      return value;
    else
      {
        new (&u.value) T (static_cast<T &&> (value));
        return u.ptr;
      }
  }

  static T &
  unpack (gpointer &ptr) noexcept
  {
    return reinterpret_cast<T &> (ptr);
  }
};

template<typename T>
struct ListPack<T, typename std::enable_if<std::is_integral<T>::value>::type>
{
  typedef T unpack_type;

  static gpointer
  pack (T value) noexcept
  {
    if (std::is_signed<T>::value)
      return GINT_TO_POINTER (value);
    else
      return GUINT_TO_POINTER (value);
  }

  static T
  unpack (gpointer ptr) noexcept
  {
    if (std::is_signed<T>::value)
      return GPOINTER_TO_INT (ptr);
    else
      return GPOINTER_TO_UINT (ptr);
  }
};

} /* namespace internals */

namespace GLib
{

template<typename T>
class List final
{
  static_assert (sizeof (T) <= sizeof (gpointer));

  ::GList *list;

public:
  constexpr List () noexcept
    : list (nullptr)
  { }

  ~List () noexcept
  {
    clear ();
  }

  void
  append (T item) noexcept
  {
    gpointer data = internals::ListPack<T>::pack (static_cast<T &&> (item));
    list = g_list_append (list, data);
    peel_assume (list);
  }

  void
  prepend (T item) noexcept
  {
    gpointer data = internals::ListPack<T>::pack (static_cast<T &&> (item));
    list = g_list_prepend (list, data);
    peel_assume (list);
  }

  void
  clear () noexcept
  {
    ::GList *l = list;
    list = nullptr;

    if (std::is_trivially_destructible<T>::value)
      g_list_free (l);
    else
      g_list_free_full (l, +[] (gpointer data)
        {
          reinterpret_cast<T *> (&data)->~T ();
        });
  }

  unsigned int
  length () const noexcept
  {
    return g_list_length (list);
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
    ::GList *l = list;
    list = nullptr;
    return l;
  }

  /* C++ iteration */

  class iterator final
  {
    friend class List;
    ::GList *list;

    constexpr iterator (::GList *list) noexcept
      : list (list)
    { }

  public:
    constexpr typename internals::ListPack<T>::unpack_type
    operator * () const noexcept
    {
      return internals::ListPack<T>::unpack (list->data);
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

  typedef typename List<const T>::iterator const_iterator;
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
};

} /* namespace GLib */
} /* namespace peel */

peel_end_header
