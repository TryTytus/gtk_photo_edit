#include "hello_window.h"
#include "image_frame.h"
#include "sliders_frame.h"
#include <gtkmm/cssprovider.h>
#include <gtkmm/enums.h>

HelloWindow::HelloWindow()
    : box_(),
      image_frame_(),
      sliders_frame_()
{
    set_title("Photo editor");
    maximize();

    background_.set_hexpand(true);
    background_.set_vexpand(true);
    background_.set_can_target(false);

    background_.set_draw_func(
        sigc::mem_fun(*this, &HelloWindow::draw_dots_background)
    );

    box_.set_column_spacing(8);
    box_.set_hexpand(true);
    box_.set_vexpand(true);
    box_.set_halign(Gtk::Align::FILL);
    box_.set_valign(Gtk::Align::FILL);

    sliders_frame_.set_size_request(220, -1);
    sliders_frame_.set_hexpand(false);
    sliders_frame_.set_vexpand(true);

    image_frame_.set_hexpand(true);
    image_frame_.set_vexpand(true);

    box_.attach(sliders_frame_, 1, 0, 1, 1);
    box_.attach(image_frame_, 0, 0, 1, 1);

    overlay_.set_child(background_);
    overlay_.add_overlay(box_);

    set_child(overlay_);

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

        .image-bg {
            background: #2C2F33;
        }

        button {
            border-color: #3a78ba;
            background: #616161;
            color: white;
            border-radius: 5rem;
            border-width: 3px;
        }


        scale {
            padding: 8px;
        }

        scale trough {
            min-height: 8px;
            background: #2b2b2b;
            border-radius: 999px;
            border: 1px solid #555;
        }

        scale highlight {
            background: #4da3ff;
            border-radius: 999px;
        }

        scale slider {
            min-width: 10px;
            min-height: 10px;
            background: #616161;
            border: 2px solid #4da3ff;
            border-radius: 999px;
            box-shadow: 0 2px 6px rgba(0, 0, 0, 0.35);
            margin: -8px;
        }

        scale slider:hover {
            background: #eaf4ff;
        }

        scale slider:active {
            background: #cfe8ff;
        }

        
        .controls-panel {
            color: white;
            padding-top: 24px;
            padding-bottom: 24px;
            padding-left: 18px;
            padding-right: 18px;
        }
    )");

    Gtk::StyleContext::add_provider_for_display(
        Gdk::Display::get_default(),
        provider,
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
}


void HelloWindow::draw_dots_background(
    const Cairo::RefPtr<Cairo::Context>& cr,
    int width,
    int height
) {

    
    cr->set_source_rgb(0.06, 0.07, 0.09);
    cr->paint();

    constexpr double spacing = 15.0;
    constexpr double radius = 1.1;
    constexpr double tau = 6.28318530718;

    cr->set_source_rgba(1.0, 1.0, 1.0, 0.13);

    for (double y = spacing / 2.0; y < height; y += spacing) {
        for (double x = spacing / 2.0; x < width; x += spacing) {
            cr->arc(x, y, radius, 0.0, tau);
            cr->fill();
        }
    }
}