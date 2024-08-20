#include <peel/GObject/Object.h>
#include <peel/class.h>

using namespace peel;

class MyClass final : public Object
{
  PEEL_SIMPLE_CLASS (MyClass, Object)

public:
  void
  say_hello ()
  {
    g_print ("Hello from %s!\n", get_type_name ());
  }
};

PEEL_CLASS_IMPL (MyClass, "MyClass", Object)

void
MyClass::Class::init ()
{
}

int
main ()
{
  RefPtr<MyClass> my_instance = Object::create<MyClass> ();
  my_instance->say_hello ();
}
