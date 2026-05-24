#include "image_frame.h"
#include "Filters.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <exception>
#include <gdkmm/pixbuf.h>
#include <gdkmm/pixbufloader.h>
#include <gdkmm/texture.h>
#include <giomm/file.h>
#include <glib.h>
#include <glibconfig.h>
#include <glibmm/bytes.h>
#include <glibmm/error.h>
#include <gtkmm/error.h>
#include <gtkmm/filedialog.h>
#include <gtkmm/picture.h>
#include <gtkmm/window.h>
#include <iostream>
#include <sigc++/functors/mem_fun.h>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <vips/resample.h>


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

int MAX_IMAGE_HEIGHT = 1920;

ImageFrame::ImageFrame() :
  file_dialog_(Gtk::FileDialog::create()),
  save_dialog_(Gtk::FileDialog::create()),
  box_(Gtk::Orientation::VERTICAL, 12),
  choose_image_button_("Choose image"),
  save_image_button_("Save"),
  filters_(Filters::getInstace()),
  state_(FiltersState::getInstance())
{
    initialize_vips();

    file_dialog_->set_title("Choose an image");
    choose_image_button_.signal_clicked().connect(
        sigc::mem_fun(*this, &ImageFrame::on_choose_image_clicked)
    );

    save_dialog_->set_title("Save image as");
    save_dialog_->set_initial_name("edited-image.jpg");
    save_dialog_->set_accept_label("Save");
    
    save_image_button_.signal_clicked().connect(
        sigc::mem_fun(*this, &ImageFrame::on_save_clicked)
    );

    picture_.set_hexpand(true);
    picture_.set_vexpand(true);
    picture_.set_halign(Gtk::Align::FILL);
    picture_.set_valign(Gtk::Align::FILL);
    
    picture_.set_can_shrink(true);
    picture_.set_content_fit(Gtk::ContentFit::CONTAIN);
  
    box_.set_margin(24);
    box_.set_valign(Gtk::Align::CENTER);
    box_.set_halign(Gtk::Align::CENTER);
  
    choose_image_button_.set_halign(Gtk::Align::CENTER);
    save_image_button_.set_halign(Gtk::Align::CENTER);
  
    box_.append(picture_);
    box_.append(choose_image_button_);
    box_.append(save_image_button_);
    append(box_);
}

ImageFrame::~ImageFrame()
{
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

void ImageFrame::on_save_clicked()
{
    if (!filters_.current_) {
        return;
    }

    const auto callback = sigc::mem_fun(*this, &ImageFrame::on_save_file_open);
  
    if (auto* window = dynamic_cast<Gtk::Window*>(get_root())) {
      save_dialog_->save(*window, callback);
    } else {
      save_dialog_->save(callback);
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

        std::cout << path << '\n';

        auto start = std::chrono::high_resolution_clock::now();
        filters_.orginal_ = vips::VImage::thumbnail(
            path.c_str(),
            MAX_IMAGE_HEIGHT,
            vips::VImage::option()
            ->set("height", MAX_IMAGE_HEIGHT)
            ->set("size", VIPS_SIZE_DOWN)
        ).copy_memory();
        
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

    std::cout << "Time to load: " << ms << " ms \n";
}

void ImageFrame::on_save_file_open(const Glib::RefPtr<Gio::AsyncResult>& result)
{
    Glib::RefPtr<Gio::File> file;

    try {
        file = save_dialog_->save_finish(result);
    } catch (const std::exception error) {
        std::cerr << "Save dialog failed: " << error.what() << '\n';
        return;
    }

    if (!file)
    {
        return;
    }

    auto path = file->get_path();

    if (path.empty())
    {
        return;
    }

    if (!filters_.current_) {
        return;
    }

    try {
        filters_.current_->write_to_file(path.c_str());
    } catch (const std::exception& error) {
        std::cerr << "Image save failed: " << error.what() << '\n';
    }
}

void ImageFrame::on_save_image_clicked()
{
    
}
