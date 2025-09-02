# Gotchas and Pitfalls

## Capturing `WeakPtr`

blah blah

## Complex fields in GObject classes

GObject classes don't have a C++ constructor, instead, they have the GObject
initializer, spelled `MyType::init (Class *)` in peel. Before the initializer
is run, the body of the class is initialized to zero bytes, as if with
`memset`. This is enough to initialize many types to their default values (and
in fact similiar to how global variables located in `.bss` are initialized). In
particular:
* integers get set to 0,
* booleans get set to `false`,
* floating-point numbers get set to 0.0,
* raw pointers get set to `nullptr`,
* as an additional guarantee, peel smart pointers (such as [`RefPtr`],
  [`WeakPtr`], [`UniquePtr`], [`String`]) get set to `nullptr`.

However, for more complex types (for example, [`std::vector`]), the C++
language requires the actual constructor to be invoked. This happens implicitly
when you _do_ use a C++ constructor in your own class:

```cpp
class MyClass
{
public:
  MyClass ();
  int m_int;
  std::vector<int> m_vector;
};

MyClass::MyClass ()
 : m_int (42)
 /* m_vector implicitly constructed here */
{
  std::cout << "My C++ constructor" << std::endl;
}
```

In peel, as your class doesn't have a C++ constructor, you have to instead
explicitly use [placement new] in your initializer to construct the values of
complex types:

```cpp
class MyWidget : public Gtk::Widget
{
  /* ... */

  int m_int;
  std::vector<int> m_vector;

  inline void
  init (Class *);
};

inline void
MyWidget::init (Class *)
{
  m_int = 42;
  new (&m_vector) std::vector<int>;
}
```

On the other hand, peel classes _do_ have a C++ destructor, which is mapped to
the GObject [`finalize` vfunc]. For this reason, destructing the object will
properly destruct all of its fields, invoking their C++ destructors. You don't
have to do anything explicitly to make it happen.

[`RefPtr`]: ref-ptr.md
[`WeakPtr`]: weak-ptr.md
[`UniquePtr`]: unique-ptr.md
[`String`]: string.md
[`std::vector`]: https://en.cppreference.com/w/cpp/container/vector.html
[placement new]: https://en.cppreference.com/w/cpp/language/new#Placement_new
[`finalize` vfunc]: https://docs.gtk.org/gobject/vfunc.Object.finalize.html
