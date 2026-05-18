#pragma once

#include "Filters.h"
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include <functional>

class SlidersFrame : public Gtk::Box
{
public:
    SlidersFrame();
    ~SlidersFrame() = default;

    FiltersState& state_;
    
private:
    void on_scale_change();
    void on_sharpness_scale_change();
    void on_sobel_scale_change();
    void on_brightness_scale_change();

    void rerender();

    Gtk::Label label_;
    Gtk::Scale scale_;
    Gtk::Scale sharpness_scale_;
    Gtk::Scale sobel_scale_;
    Gtk::Scale brightness_scale_;
    
};
