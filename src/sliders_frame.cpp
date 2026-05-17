#include "sliders_frame.h"

#include <glibmm/main.h>
#include <gtkmm/label.h>
#include <sigc++/functors/mem_fun.h>

SlidersFrame::SlidersFrame()
    : label_("Contrast"),
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

SlidersFrame::~SlidersFrame()
{
    disconnect_debounce_handlers();
}

void SlidersFrame::set_contrast_changed_handler(std::function<void(double)> handler)
{
    contrast_changed_handler_ = std::move(handler);
}

void SlidersFrame::set_sharpness_changed_handler(std::function<void(double)> handler)
{
    sharpness_changed_handler_ = std::move(handler);
}

void SlidersFrame::set_sobel_changed_handler(std::function<void(double)> handler)
{
    sobel_changed_handler_ = std::move(handler);
}

void SlidersFrame::on_scale_change()
{
    if (contrast_changed_handler_) {
        debounce_apply(contrast_debounce_, contrast_changed_handler_, scale_.get_value());
    }
}

void SlidersFrame::on_sharpness_scale_change()
{
    if (sharpness_changed_handler_) {
        debounce_apply(
            sharpness_debounce_,
            sharpness_changed_handler_,
            sharpness_scale_.get_value()
        );
    }
}

void SlidersFrame::on_sobel_scale_change()
{
    if (sobel_changed_handler_) {
        debounce_apply(sobel_debounce_, sobel_changed_handler_, sobel_scale_.get_value());
    }
}

void SlidersFrame::debounce_apply(DebounceState& state,
                                  const std::function<void(double)>& handler,
                                  double value)
{
    state.pending_value = value;
    state.pending_handler = handler;

    if (state.connection.connected()) {
        state.connection.disconnect();
    }

    state.connection = Glib::signal_timeout().connect(
        [&state] {
            if (state.pending_handler) {
                state.pending_handler(state.pending_value);
            }
            return false;
        },
        30
    );
}

void SlidersFrame::disconnect_debounce_handlers()
{
    if (contrast_debounce_.connection.connected()) {
        contrast_debounce_.connection.disconnect();
    }
    if (sharpness_debounce_.connection.connected()) {
        sharpness_debounce_.connection.disconnect();
    }
    if (sobel_debounce_.connection.connected()) {
        sobel_debounce_.connection.disconnect();
    }
}
