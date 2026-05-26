#include "sliders_frame.h"
#include "Filters.h"

#include <glibmm/main.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/eventcontrollerscroll.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>
#include <gtkmm/scrolledwindow.h>
#include <sigc++/functors/mem_fun.h>

namespace
{
Gtk::ScrolledWindow* find_parent_scrolled_window(Gtk::Widget& widget)
{
    for (auto* parent = widget.get_parent(); parent; parent = parent->get_parent())
    {
        if (auto* scrolled_window = dynamic_cast<Gtk::ScrolledWindow*>(parent))
        {
            return scrolled_window;
        }
    }

    return nullptr;
}

void scroll_adjustment(const Glib::RefPtr<Gtk::Adjustment>& adjustment, double delta)
{
    if (!adjustment)
    {
        return;
    }

    const double lower = adjustment->get_lower();
    const double upper = std::max(lower, adjustment->get_upper() - adjustment->get_page_size());
    adjustment->set_value(std::clamp(adjustment->get_value() + delta, lower, upper));
}

void disable_scroll_value_change(Gtk::Scale& scale)
{
    auto scroll = Gtk::EventControllerScroll::create();
    scroll->set_flags(Gtk::EventControllerScroll::Flags::BOTH_AXES);
    scroll->set_propagation_phase(Gtk::PropagationPhase::CAPTURE);
    auto* scroll_controller = scroll.get();
    scroll->signal_scroll().connect([&scale, scroll_controller](double dx, double dy) {
        auto* scrolled_window = find_parent_scrolled_window(scale);
        if (!scrolled_window)
        {
            return true;
        }

        if (scroll_controller->get_unit() == Gdk::ScrollUnit::WHEEL)
        {
            dx *= 48.0;
            dy *= 48.0;
        }

        scroll_adjustment(scrolled_window->get_hadjustment(), dx);
        scroll_adjustment(scrolled_window->get_vadjustment(), dy);
        return true;
    }, false);
    scale.add_controller(scroll);
}
}

SlidersFrame::SlidersFrame()
    : state_(FiltersState::getInstance()),
      label_contrast_("Contrast"),
      scale_(Gtk::Orientation::HORIZONTAL),
      label_sharpness_("Sharpness"),
      label_sobel_("Sobel effect"),
      label_brightness_("Brightness"),
      label_exposure_("Exposure"),
      label_gamma_("Gamma"),
      label_blur_("Blur")
{
    set_orientation(Gtk::Orientation::VERTICAL);
    add_css_class("controls-panel");

    set_scale_default(scale_);

    sharpness_scale_.set_range(0.0, 1.0);
    sharpness_scale_.set_value(0.0);
    sharpness_scale_.set_digits(2);

    sharpness_scale_.set_size_request(300, 50);
    disable_scroll_value_change(sharpness_scale_);

    sobel_scale_.set_range(0.0, 1.0);
    sobel_scale_.set_value(0.0);
    sobel_scale_.set_digits(2);

    sobel_scale_.set_size_request(300, 50);
    disable_scroll_value_change(sobel_scale_);

    set_scale_default(brightness_scale_);
    set_scale_default(exposure_scale);


    blur_scale_.set_range(0.0, 1.0);
    blur_scale_.set_value(0.0);
    blur_scale_.set_digits(2);

    blur_scale_.set_size_request(300, 50);
    disable_scroll_value_change(blur_scale_);


    gamma_scale_.set_range(0, 1.0);
    gamma_scale_.set_value(0.0);
    gamma_scale_.set_digits(2);
    disable_scroll_value_change(gamma_scale_);

    brightness_scale_.set_size_request(300, 50);
    
    set_scale_default(saturation_scale_);
    set_scale_default(warmth_scale_);
    set_scale_default(tint_scale_);
    set_scale_default(highlight_scale_);
    set_scale_default(shadows_scale_);
    set_scale_default(black_scale_);
    set_scale_default(white_scale_);
    set_scale_default(vibrance_scale_);
    set_scale_default(vignette_scale_);
    set_scale_default(clarity_scale_);
    set_scale_default(noise_scale_);

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


    saturation_scale_.signal_value_changed().connect( sigc::mem_fun(*this, &SlidersFrame::on_saturation_scale_change) );
    warmth_scale_.signal_value_changed().connect( sigc::mem_fun(*this, &SlidersFrame::on_warmth_scale_change) );
    tint_scale_.signal_value_changed().connect( sigc::mem_fun(*this, &SlidersFrame::on_tint_scale_change) );
    highlight_scale_.signal_value_changed().connect( sigc::mem_fun(*this, &SlidersFrame::on_highlight_scale_change) );
    shadows_scale_.signal_value_changed().connect( sigc::mem_fun(*this, &SlidersFrame::on_shadows_scale_change) );
    black_scale_.signal_value_changed().connect( sigc::mem_fun(*this, &SlidersFrame::on_black_scale_change) );
    white_scale_.signal_value_changed().connect( sigc::mem_fun(*this, &SlidersFrame::on_white_scale_change) );
    vibrance_scale_.signal_value_changed().connect( sigc::mem_fun(*this, &SlidersFrame::on_vibrance_scale_change) );
    vignette_scale_.signal_value_changed().connect( sigc::mem_fun(*this, &SlidersFrame::on_vignette_scale_change) );
    clarity_scale_.signal_value_changed().connect( sigc::mem_fun(*this, &SlidersFrame::on_clarity_scale_change) );
    noise_scale_.signal_value_changed().connect( sigc::mem_fun(*this, &SlidersFrame::on_noise_scale_change) );


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

    append(label_saturation_);
    append(saturation_scale_);
    append(label_warmth_);
    append(warmth_scale_);
    append(label_tint_);
    append(tint_scale_);
    append(label_highlight_);
    append(highlight_scale_);
    append(label_shadows_);
    append(shadows_scale_);
    append(label_black_);
    append(black_scale_);
    append(label_white_);
    append(white_scale_);
    append(label_vibrance_);
    append(vibrance_scale_);
    append(label_vignette_);
    append(vignette_scale_);
    append(label_clarity_);
    append(clarity_scale_);
    append(label_noise_);
    append(noise_scale_);
}



void SlidersFrame::set_scale_default(Gtk::Scale& scale)
{
    scale.set_range(-1.0, 1.0);
    scale.set_value(0.0);
    scale.set_digits(2);

    scale.set_size_request(300, 50);

    disable_scroll_value_change(scale);

}

void SlidersFrame::on_scale_change()
{
    state_.contrast = std::clamp(scale_.get_value(), -1.0, 1.0);
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
    state_.brightness = std::clamp(brightness_scale_.get_value(), -1.0, 1.0);
    rerender();
}


void SlidersFrame::on_exposure_scale_change()
{
    state_.exposure = std::clamp(exposure_scale.get_value(), -1.0, 1.0);
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

void SlidersFrame::on_saturation_scale_change()
{ 
    state_.saturation = std::clamp(saturation_scale_.get_value(), -1.0, 1.0);
    rerender();
}
void SlidersFrame::on_warmth_scale_change()
{ 
    state_.warmth = std::clamp(warmth_scale_.get_value(), -1.0, 1.0);
    rerender();
}
void SlidersFrame::on_tint_scale_change()
{ 
    state_.tint = std::clamp(tint_scale_.get_value(), -1.0, 1.0);
    rerender();
}
void SlidersFrame::on_highlight_scale_change()
{ 
    state_.highlight = std::clamp(highlight_scale_.get_value(), -1.0, 1.0);
    rerender();
}
void SlidersFrame::on_shadows_scale_change()
{ 
    state_.shadows = std::clamp(shadows_scale_.get_value(), -1.0, 1.0);
    rerender();
}
void SlidersFrame::on_black_scale_change()
{ 
    state_.black = std::clamp(black_scale_.get_value(), -1.0, 1.0);
    rerender();
}
void SlidersFrame::on_white_scale_change()
{ 
    state_.white = std::clamp(white_scale_.get_value(), -1.0, 1.0);
    rerender();
}
void SlidersFrame::on_vibrance_scale_change()
{ 
    state_.vibrance = std::clamp(vibrance_scale_.get_value(), -1.0, 1.0);
    rerender();
}
void SlidersFrame::on_vignette_scale_change()
{ 
    state_.vignette = std::clamp(vignette_scale_.get_value(), -1.0, 1.0);
    rerender();
}
void SlidersFrame::on_clarity_scale_change()
{ 
    state_.clarity = std::clamp(clarity_scale_.get_value(), -1.0, 1.0);
    rerender();
}
void SlidersFrame::on_noise_scale_change()
{ 
    state_.noise = std::clamp(noise_scale_.get_value(), -1.0, 1.0);
    rerender();
}

void SlidersFrame::rerender()
{
    if (Filters::rerender)
    {
        (*Filters::rerender)();
    }
}
