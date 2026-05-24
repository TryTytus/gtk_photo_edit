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

    void on_exposure_scale_change();
    void on_blur_scale_change();
    void on_gamma_scale_change();

    void set_scale_default(Gtk::Scale& scale);

    void rerender();

    Gtk::Label label_contrast_;
    Gtk::Scale scale_;
    Gtk::Label label_sharpness_;
    Gtk::Scale sharpness_scale_;
    Gtk::Label label_sobel_;
    Gtk::Scale sobel_scale_;
    Gtk::Label label_brightness_;
    Gtk::Scale brightness_scale_;
    Gtk::Label label_exposure_;
    Gtk::Scale exposure_scale;
    Gtk::Label label_gamma_;
    Gtk::Scale gamma_scale_;
    Gtk::Label label_blur_;
    Gtk::Scale blur_scale_;

    Gtk::Label label_saturation_ {"saturation"};
    Gtk::Label label_warmth_ {"warmth"};
    Gtk::Label label_tint_ {"tint"};
    Gtk::Label label_highlight_ {"highlight"};
    Gtk::Label label_shadows_ {"shadows"};
    Gtk::Label label_black_ {"black"};
    Gtk::Label label_white_ {"white"};
    Gtk::Label label_vibrance_ {"vibrance"};
    Gtk::Label label_vignette_ {"vignette"};
    Gtk::Label label_clarity_ {"clarity"};
    Gtk::Label label_noise_ {"noise"};

    Gtk::Scale saturation_scale_ {};
    Gtk::Scale warmth_scale_ {};
    Gtk::Scale tint_scale_ {};
    Gtk::Scale highlight_scale_ {};
    Gtk::Scale shadows_scale_ {};
    Gtk::Scale black_scale_ {};
    Gtk::Scale white_scale_ {};
    Gtk::Scale vibrance_scale_ {};
    Gtk::Scale vignette_scale_ {};
    Gtk::Scale clarity_scale_ {};
    Gtk::Scale noise_scale_ {};


    void on_saturation_scale_change();
    void on_warmth_scale_change();
    void on_tint_scale_change();
    void on_highlight_scale_change();
    void on_shadows_scale_change();
    void on_black_scale_change();
    void on_white_scale_change();
    void on_vibrance_scale_change();
    void on_vignette_scale_change();
    void on_clarity_scale_change();
    void on_noise_scale_change();
    
};
