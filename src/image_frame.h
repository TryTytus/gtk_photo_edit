#pragma once

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


class ImageFrame : public Gtk::Box {
public:
    ImageFrame();
    ~ImageFrame() override;

    void on_close_clicked();
    void on_choose_image_clicked();
    void on_file_open(const Glib::RefPtr<Gio::AsyncResult>& result);
    void set_contrast(double contrast);
    void set_sharpness(double intensity);
    void set_sobel(double intensity);
    void render_current();
  
    Glib::RefPtr<Gtk::FileDialog> file_dialog_;
    Gtk::Box box_;
    Gtk::Label label_;
    Gtk::Button close_button_;
    Gtk::Button choose_image_button_;
    Gtk::Picture picture_;
    std::optional<vips::VImage> orginal_;
    std::optional<vips::VImage> current_;

    
    Filters& filters_;
    FiltersState& state_;
    

};
