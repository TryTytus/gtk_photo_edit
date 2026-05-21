#pragma once

#include <glibmm/refptr.h>
#include <giomm/asyncresult.h>
#include <gtkmm.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/filedialog.h>
#include <gtkmm/grid.h>
#include <gtkmm/picture.h>
#include "image_frame.h"
#include "sliders_frame.h"

class HelloWindow : public Gtk::Window {
public:
  HelloWindow();

  void setCss();

private:
    Gtk::Grid box_;
    ImageFrame image_frame_;
    SlidersFrame sliders_frame_;
    FiltersState state_{};
};
