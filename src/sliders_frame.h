#pragma once

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include <functional>

class SlidersFrame : public Gtk::Box
{
public:
    SlidersFrame();
    ~SlidersFrame() override;

    void set_contrast_changed_handler(std::function<void(double)> handler);
    void set_sharpness_changed_handler(std::function<void(double)> handler);
    void set_sobel_changed_handler(std::function<void(double)> handler);
    
private:
    struct DebounceState {
        double pending_value = 0.0;
        std::function<void(double)> pending_handler;
        sigc::connection connection;
    };

    void debounce_apply(DebounceState& state,
                        const std::function<void(double)>& handler,
                        double value);
    void disconnect_debounce_handlers();

    DebounceState contrast_debounce_;
    DebounceState sharpness_debounce_;
    DebounceState sobel_debounce_;

    void on_scale_change();
    void on_sharpness_scale_change();
    void on_sobel_scale_change();

    Gtk::Label label_;
    Gtk::Scale scale_;
    Gtk::Scale sharpness_scale_;
    Gtk::Scale sobel_scale_;
    
    std::function<void(double)> contrast_changed_handler_;
    std::function<void(double)> sharpness_changed_handler_;
    std::function<void(double)> sobel_changed_handler_;
};
