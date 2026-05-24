#include "sliders_frame.h"
#include "Filters.h"

#include <glibmm/main.h>
#include <gtkmm/label.h>
#include <sigc++/functors/mem_fun.h>

SlidersFrame::SlidersFrame()
    : state_(FiltersState::getInstance()),
      label_contrast_("Contrast"),
      label_sharpness_("Sharpness"),
      label_brightness_("Brightness"),
      label_sobel_("Sobel effect"),
      label_exposure_("Exposure"),
      label_gamma_("Gamma"),
      label_blur_("Blur"),
      scale_(Gtk::Orientation::HORIZONTAL)
{
    set_orientation(Gtk::Orientation::VERTICAL);

    scale_.set_range(0.0, 1.0);
    scale_.set_value(0.0);
    scale_.set_digits(2);

    add_css_class("controls-panel");


    scale_.set_size_request(300, 50);

    sharpness_scale_.set_range(0.0, 1.0);
    sharpness_scale_.set_value(0.0);
    sharpness_scale_.set_digits(2);

    sharpness_scale_.set_size_request(300, 50);

    sobel_scale_.set_range(0.0, 1.0);
    sobel_scale_.set_value(0.0);
    sobel_scale_.set_digits(2);

    sobel_scale_.set_size_request(300, 50);

    brightness_scale_.set_range(0.0, 1.0);
    brightness_scale_.set_value(0.0);
    brightness_scale_.set_digits(2);

    brightness_scale_.set_size_request(300, 50);

    

    exposure_scale.set_range(0.0, 1.0);
    exposure_scale.set_value(0.0);
    exposure_scale.set_digits(2);

    exposure_scale.set_size_request(300, 50);


    blur_scale_.set_range(0.0, 1.0);
    blur_scale_.set_value(0.0);
    blur_scale_.set_digits(2);

    blur_scale_.set_size_request(300, 50);


    gamma_scale_.set_range(0.0, 1.0);
    gamma_scale_.set_value(0.0);
    gamma_scale_.set_digits(2);

    brightness_scale_.set_size_request(300, 50);

    scale_.signal_value_changed().connect(
        sigc::mem_fun(*this, &SlidersFrame::on_scale_change)
    );

    sharpness_scale_.signal_value_changed().connect(
        sigc::mem_fun(*this, &SlidersFrame::on_sharpness_scale_change)
    );

    sobel_scale_.signal_value_changed().connect(
        sigc::mem_fun(*this, &SlidersFrame::on_sobel_scale_change)
    );

    brightness_scale_.signal_value_changed().connect(
        sigc::mem_fun(*this, &SlidersFrame::on_brightness_scale_change)
    );

    exposure_scale.signal_value_changed().connect(
        sigc::mem_fun(*this, &SlidersFrame::on_exposure_scale_change)
    );

    blur_scale_.signal_value_changed().connect(
        sigc::mem_fun(*this, &SlidersFrame::on_blur_scale_change)
    );

    gamma_scale_.signal_value_changed().connect(
        sigc::mem_fun(*this, &SlidersFrame::on_gamma_scale_change)
    );

    add_css_class("image-bg");
    
    append(label_contrast_);
    append(scale_);
    append(label_sharpness_);
    append(sharpness_scale_);
    append(label_sobel_);
    append(sobel_scale_);
    append(label_brightness_);
    append(brightness_scale_);
    append(label_exposure_);
    append(exposure_scale);
    append(label_gamma_);
    append(gamma_scale_);
    append(label_blur_);
    append(blur_scale_);
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

void SlidersFrame::on_brightness_scale_change()
{
    state_.brightness = std::clamp(brightness_scale_.get_value(), 0.0, 1.0);
    rerender();
}


void SlidersFrame::on_exposure_scale_change()
{
    state_.exposure = std::clamp(exposure_scale.get_value(), 0.0, 1.0);
    rerender();
}


void SlidersFrame::on_blur_scale_change()
{
    state_.blur = std::clamp(blur_scale_.get_value(), 0.0, 1.0);
    rerender();
}

void SlidersFrame::on_gamma_scale_change()
{
    state_.gamma = std::clamp(gamma_scale_.get_value(), 0.0, 1.0);
    rerender();
}

void SlidersFrame::rerender()
{
    if (Filters::rerender)
    {
        (*Filters::rerender)();
    }
}

