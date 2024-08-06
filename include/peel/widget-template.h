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

#define PEEL_WIDGET_TEMPLATE_BIND_CHILD(WidgetClass, name)                     \
do                                                                             \
  {                                                                            \
    _peel_diagnostic_push_ignored_offsetof                                     \
    this->::peel::Gtk::Widget::Class::bind_template_child_full (               \
      #name, false, G_STRUCT_OFFSET (WidgetClass, name));                      \
    _peel_diagnostic_pop                                                       \
  }                                                                            \
while (0)                                                                      \
/* end of PEEL_WIDGET_TEMPLATE_BIND_CHILD */

#define PEEL_WIDGET_TEMPLATE_BIND_CALLBACK(WidgetClass, name)                  \
  gtk_widget_class_bind_template_callback_full (                               \
    GTK_WIDGET_CLASS (static_cast<::peel::Gtk::Widget::Class *> (this)),       \
    #name,                                                                     \
    ::peel::internals::WidgetTemplateCallbackHelper<                           \
      WidgetClass, decltype (&WidgetClass::name)>                              \
      ::template wrap<&WidgetClass::name> ())                                  \
/* end of PEEL_WIDGET_TEMPLATE_BIND_CALLBACK */
