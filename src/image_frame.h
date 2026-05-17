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

struct PreviewFilterState {
    double contrast = 0.0;
    double sharpness = 0.0;
    double sobel = 0.0;
};

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
  
    Glib::RefPtr<Gtk::FileDialog> file_dialog_;
    Gtk::Box box_;
    Gtk::Label label_;
    Gtk::Button close_button_;
    Gtk::Button choose_image_button_;
    Gtk::Picture picture_;
    std::optional<vips::VImage> preview_source_;
    double contrast_;
    double sharpness_;
    double sobel_;

private:
    struct RenderResult {
        std::uint64_t request_id = 0;
        bool loaded_image = false;
        PreviewFilterState filters;
        std::optional<vips::VImage> preview_source;
        std::vector<std::uint8_t> bytes;
    };

    void schedule_preview_render();
    void render_worker_loop();
    void on_render_ready();
    void set_preview_bytes(const std::vector<std::uint8_t>& bytes);

    Glib::Dispatcher render_dispatcher_;
    sigc::connection render_dispatch_connection_;

    std::thread render_worker_;
    std::mutex render_mutex_;

    std::condition_variable render_condition_;
    PreviewFilterState filter_state_;

    bool render_requested_ = false;
    std::optional<std::string> pending_load_path_;
    std::uint64_t latest_render_request_id_ = 0;

    std::mutex render_result_mutex_;
    std::optional<RenderResult> pending_render_result_;
    std::atomic_bool closing_ = false;
};
