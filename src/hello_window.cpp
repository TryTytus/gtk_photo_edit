#include "hello_window.h"
#include "image_frame.h"
#include "sliders_frame.h"
#include <gtkmm/cssprovider.h>

HelloWindow::HelloWindow() : 
    box_(Gtk::Orientation::HORIZONTAL, 12),
    image_frame_(),
    sliders_frame_()
{
  set_title("gtkmm Hello World");
  maximize();

  box_.append(image_frame_);
  box_.append(sliders_frame_);

  box_.add_css_class("app-bg");

  set_child(box_);

  setCss();

}

void HelloWindow::setCss()
{
    auto provider = Gtk::CssProvider::create();

    provider->load_from_data(R"(
        .app-bg {
            background: #202124;
            color: #eeeeee;
        }
    )");

    Gtk::StyleContext::add_provider_for_display(
        Gdk::Display::get_default(),
        provider,
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
}
