#pragma once

#include <glibmm/dispatcher.h>
#include <glibmm/refptr.h>
#include <giomm/asyncresult.h>
#include <gtkmm.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/filedialog.h>
#include <gtkmm/picture.h>
#include <vips/vips8>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "Filters.h"
#include "Worker.h"

class ImageFrame : public Gtk::Box {
public:
    ImageFrame();
    ~ImageFrame() override;

    void on_save_clicked();
    void on_save_file_open(const Glib::RefPtr<Gio::AsyncResult>& result);
    void on_save_image_clicked();
    void on_choose_image_clicked();
    void on_file_open(const Glib::RefPtr<Gio::AsyncResult>& result);
    void set_contrast(double contrast);
    void set_sharpness(double intensity);
    void set_sobel(double intensity);
    void render_current();
    void render_current_job();

    void on_image_update();

    void notify();
  
    Glib::RefPtr<Gtk::FileDialog> file_dialog_;
    Glib::RefPtr<Gtk::FileDialog> save_dialog_;
    Gtk::Box box_;
    Gtk::Button choose_image_button_;
    Gtk::Button save_image_button_;
    Gtk::Picture picture_;
    std::optional<vips::VImage> orginal_;
    std::optional<vips::VImage> current_;
    std::thread* thread_;

    
    Filters& filters_;
    FiltersState& state_;

    Glib::Dispatcher dispatcher_;

    Worker worker_;

};
