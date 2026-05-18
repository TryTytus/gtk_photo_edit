#include "sliders_frame.h"
#include "Filters.h"

#include <glibmm/main.h>
#include <gtkmm/label.h>
#include <sigc++/functors/mem_fun.h>

SlidersFrame::SlidersFrame()
    : state_(FiltersState::getInstance()),
      label_("Contrast"),
      scale_(Gtk::Orientation::HORIZONTAL)
{
    set_orientation(Gtk::Orientation::VERTICAL);
    set_spacing(8);
    set_margin_top(24);
    set_margin_bottom(24);
    set_margin_start(24);
    set_margin_end(24);

    scale_.set_range(0.0, 1.0);
    scale_.set_value(0.0);
    scale_.set_digits(2);

    scale_.set_size_request(300, 50);

    sharpness_scale_.set_range(0.0, 1.0);
    sharpness_scale_.set_value(0.0);
    sharpness_scale_.set_digits(2);

    sharpness_scale_.set_size_request(300, 50);

    sobel_scale_.set_range(0.0, 1.0);
    sobel_scale_.set_value(0.0);
    sobel_scale_.set_digits(2);

    sobel_scale_.set_size_request(300, 50);

    scale_.signal_value_changed().connect(
        sigc::mem_fun(*this, &SlidersFrame::on_scale_change)
    );

    sharpness_scale_.signal_value_changed().connect(
        sigc::mem_fun(*this, &SlidersFrame::on_sharpness_scale_change)
    );

    sobel_scale_.signal_value_changed().connect(
        sigc::mem_fun(*this, &SlidersFrame::on_sobel_scale_change)
    );
    
    append(label_);
    append(scale_);
    append(sharpness_scale_);
    append(sobel_scale_);
}


void SlidersFrame::on_scale_change()
{
    state_.contrast = std::clamp(scale_.get_value(), 0.0, 1.0);
    rerender();
}

void SlidersFrame::on_sharpness_scale_change()
{

    state_.sharpness = std::clamp(sharpness_scale_.get_value(), 0.0, 1.0);
    rerender();
}

void SlidersFrame::on_sobel_scale_change()
{
    state_.sobel = std::clamp(sobel_scale_.get_value(), 0.0, 1.0);
    rerender();
}


void SlidersFrame::rerender()
{
    if (Filters::rerender)
    {
        (*Filters::rerender)();
    }
}