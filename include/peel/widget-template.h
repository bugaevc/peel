#pragma once

#include <peel/class.h>
#include <peel/signal.h>
#include <peel/Gtk/Widget.h>

namespace peel
{
namespace internals
{

template<typename WidgetClass, typename Signature>
struct WidgetTemplateCallbackHelper;

template<typename WidgetClass, typename Ret, typename... Args>
struct WidgetTemplateCallbackHelper<WidgetClass, Ret (WidgetClass::*) (Args...)>
{
  template<Ret (WidgetClass::*Method) (Args...)>
  static ::GCallback
  wrap ()
  {
    return GCallback (+[] (typename SignalTraits<Args>::CType ...args, void *user_data) -> typename SignalTraits<Ret>::CType
      {
        WidgetClass *widget = reinterpret_cast<WidgetClass *> (user_data);
        return SignalTraits<Ret>::to_c ((widget->*Method) (SignalTraits<Args>::from_c (args)...));
      });
  }
};

template<typename WidgetClass, typename... Args>
struct WidgetTemplateCallbackHelper<WidgetClass, void (WidgetClass::*) (Args...)>
{
  template<void (WidgetClass::*Method) (Args...)>
  static ::GCallback
  wrap ()
  {
    return GCallback (+[] (typename SignalTraits<Args>::CType ...args, void *user_data) -> void
      {
        WidgetClass *widget = reinterpret_cast<WidgetClass *> (user_data);
        (widget->*Method) (SignalTraits<Args>::from_c (args)...);
      });
  }
};

} /* namespace internals */
} /* namespace peel */

#define peel_widget_template_bind_child_impl(WidgetClass, member, name)        \
do                                                                             \
  {                                                                            \
    _peel_diagnostic_push_ignored_offsetof                                     \
    this->::peel::Gtk::Widget::Class::bind_template_child_full (               \
      name, false, G_STRUCT_OFFSET (WidgetClass, member));                     \
    _peel_diagnostic_pop                                                       \
  }                                                                            \
while (0)                                                                      \
/* end of peel_widget_template_bind_child_impl */

#define peel_widget_template_bind_child_impl2(WidgetClass, member)             \
  peel_widget_template_bind_child_impl (WidgetClass, member, #member)

#define peel_widget_template_bind_callback_impl(WidgetClass, method, name)     \
  gtk_widget_class_bind_template_callback_full (                               \
    GTK_WIDGET_CLASS (static_cast<::peel::Gtk::Widget::Class *> (this)),       \
    name,                                                                      \
    ::peel::internals::WidgetTemplateCallbackHelper<                           \
      WidgetClass, decltype (&WidgetClass::method)>                            \
      ::template wrap<&WidgetClass::method> ())                                \
/* end of peel_widget_template_bind_callback_impl */

#define peel_widget_template_bind_callback_impl2(WidgetClass, method)          \
  peel_widget_template_bind_callback_impl (WidgetClass, method, #method)

#define PEEL_WIDGET_TEMPLATE_BIND_CHILD(...)                                   \
  peel_macro_overload_23 (                                                     \
    __VA_ARGS__,                                                               \
    peel_widget_template_bind_child_impl,                                      \
    peel_widget_template_bind_child_impl2) (__VA_ARGS__)                       \
/* end of PEEL_WIDGET_TEMPLATE_BIND_CHILD */

#define PEEL_WIDGET_TEMPLATE_BIND_CALLBACK(...)                                \
  peel_macro_overload_23 (                                                     \
    __VA_ARGS__,                                                               \
    peel_widget_template_bind_callback_impl,                                   \
    peel_widget_template_bind_callback_impl2) (__VA_ARGS__)                    \
/* end of PEEL_WIDGET_TEMPLATE_BIND_CHILD */
