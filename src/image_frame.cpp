#include "image_frame.h"
#include "Filters.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <gdkmm/pixbuf.h>
#include <gdkmm/pixbufloader.h>
#include <gdkmm/texture.h>
#include <giomm/file.h>
#include <glib.h>
#include <glibconfig.h>
#include <glibmm/bytes.h>
#include <glibmm/error.h>
#include <gtkmm/error.h>
#include <gtkmm/picture.h>
#include <gtkmm/window.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <vips/VImage8.h>


namespace {

void initialize_vips()
{
    static const bool initialized = [] {
        if (VIPS_INIT("gtk_second")) {
            throw std::runtime_error(vips_error_buffer());
        }
        return true;
    }();
    (void)initialized;
}

}

ImageFrame::ImageFrame() :
  file_dialog_(Gtk::FileDialog::create()),
  box_(Gtk::Orientation::VERTICAL, 12),
  label_("Hello, world!"),
  close_button_("Close"),
  choose_image_button_("Choose image"),
  filters_(Filters::getInstace()),
  state_(FiltersState::getInstance())
{
    initialize_vips();

    file_dialog_->set_title("Choose an image");
    choose_image_button_.signal_clicked().connect(
        sigc::mem_fun(*this, &ImageFrame::on_choose_image_clicked)
    );
  
    box_.set_margin_top(24);
    box_.set_margin_bottom(24);
    box_.set_margin_start(24);
    box_.set_margin_end(24);
    box_.set_valign(Gtk::Align::CENTER);
    box_.set_halign(Gtk::Align::CENTER);
  
    label_.set_halign(Gtk::Align::CENTER);
    choose_image_button_.set_halign(Gtk::Align::CENTER);
    close_button_.set_halign(Gtk::Align::CENTER);
    close_button_.signal_clicked().connect(
        sigc::mem_fun(*this, &ImageFrame::on_close_clicked)
    );
  
    box_.append(picture_);
    box_.append(label_);
    box_.append(choose_image_button_);
    box_.append(close_button_);
    append(box_);
}

ImageFrame::~ImageFrame()
{
}

void ImageFrame::on_close_clicked()
{
  if (auto* window = dynamic_cast<Gtk::Window*>(get_root())) {
    window->close();
  }
}

void ImageFrame::on_choose_image_clicked() 
{
  const auto callback = sigc::mem_fun(*this, &ImageFrame::on_file_open);

  if (auto* window = dynamic_cast<Gtk::Window*>(get_root())) {
    file_dialog_->open(*window, callback);
  } else {
    file_dialog_->open(callback);
  }
}

void ImageFrame::on_file_open(const Glib::RefPtr<Gio::AsyncResult>& result) 
{
    try {
        const auto file = file_dialog_->open_finish(result);
        const auto path = file->get_path();

        if (path.empty()) {
            return;
        }

        label_.set_text(path);
        std::cout << path << '\n';

        auto start = std::chrono::high_resolution_clock::now();
        filters_.orginal_ = vips::VImage::new_from_file(path.c_str());
        Filters::rerender = [this]() {
            render_current();
        };
        render_current();

        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start
        ).count();

        std::cout << "Time to load image from file: " << ms << " ms \n";
    } catch (const std::exception& error) {
        std::cerr << "Image load failed: " << error.what() << '\n';
    }
}

void ImageFrame::set_contrast(double contrast)
{
    state_.contrast = std::clamp(contrast, 0.0, 1.0);
    render_current();
}

void ImageFrame::set_sharpness(double intensity)
{
    state_.sharpness = std::clamp(intensity, 0.0, 1.0);
    render_current();
}

void ImageFrame::set_sobel(double intensity)
{
    state_.sobel = std::clamp(intensity, 0.0, 1.0);
    render_current();
}

void ImageFrame::render_current()
{
    if (!filters_.orginal_) {
        return;
    }
    auto start = std::chrono::high_resolution_clock::now();
    
    filters_.current_ = filters_.update(*filters_.orginal_, state_);

    void* buffer = nullptr;
    size_t size { 0 };
    filters_.current_->write_to_buffer(".png", &buffer, &size);

    const auto bytes = Glib::Bytes::create(buffer, size);
    g_free(buffer);
    auto texture = Gdk::Texture::create_from_bytes(bytes);
    picture_.set_paintable(texture);

    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - start
    ).count();

    std::cout << "Time to load image from file: " << ms << " ms \n";
}
