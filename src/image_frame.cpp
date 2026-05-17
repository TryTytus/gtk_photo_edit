#include "image_frame.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <gdkmm/pixbuf.h>
#include <gdkmm/pixbufloader.h>
#include <giomm/file.h>
#include <glib.h>
#include <glibconfig.h>
#include <glibmm/error.h>
#include <gtkmm/error.h>
#include <gtkmm/picture.h>
#include <gtkmm/window.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr std::size_t kPreviewMaxEdge = 1600;
constexpr std::size_t kPreviewJpegQuality = 82;

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
  contrast_(0.0),
  sharpness_(0.0),
  sobel_(0.0)
{
    initialize_vips();
    render_dispatch_connection_ = render_dispatcher_.connect(
        sigc::mem_fun(*this, &ImageFrame::on_render_ready)
    );
    render_worker_ = std::thread{
        [this] {
            render_worker_loop();
        }
    };

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
    closing_.store(true);

    {
        const std::scoped_lock lock(render_mutex_);
        render_requested_ = true;
    }
    render_condition_.notify_one();

    if (render_worker_.joinable()) {
        render_worker_.join();
    }

    render_dispatch_connection_.disconnect();
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

    if (!path.empty()) {
      label_.set_text(path);
      std::cout << path << '\n';

      {
        const std::scoped_lock lock(render_mutex_);
        preview_source_.reset();
        pending_load_path_ = path;
        filter_state_ = PreviewFilterState{contrast_, sharpness_, sobel_};
        ++latest_render_request_id_;
        render_requested_ = false;
      }

      render_condition_.notify_one();
    }
  } catch (const Gtk::DialogError& error) {
    if (error.code() != Gtk::DialogError::DISMISSED) {
      std::cerr << "File dialog failed: " << error.what() << '\n';
    }
  } catch (const Glib::Error& error) {
    std::cerr << "File dialog failed: " << error.what() << '\n';
  }
}

void ImageFrame::set_contrast(double contrast)
{
    contrast_ = std::clamp(contrast, 0.0, 1.0);
    schedule_preview_render();
}

void ImageFrame::set_sharpness(double intensity)
{
    sharpness_ = std::clamp(intensity, 0.0, 1.0);
    schedule_preview_render();
}

void ImageFrame::set_sobel(double intensity)
{
    sobel_ = std::clamp(intensity, 0.0, 1.0);
    schedule_preview_render();
}

void ImageFrame::schedule_preview_render()
{
    {
        const std::scoped_lock lock(render_mutex_);
        if (!preview_source_) {
            return;
        }

        filter_state_ = PreviewFilterState{contrast_, sharpness_, sobel_};
        ++latest_render_request_id_;
        render_requested_ = true;
    }

    render_condition_.notify_one();
}

void ImageFrame::render_worker_loop()
{
    while (!closing_.load()) {
        std::optional<std::string> load_path;
        std::optional<vips::VImage> source;
        PreviewFilterState filters;
        std::uint64_t request_id = 0;

        {
            std::unique_lock lock(render_mutex_);
            render_condition_.wait(lock, [this] {
                return closing_.load() || pending_load_path_.has_value() || render_requested_;
            });

            if (closing_.load()) {
                break;
            }

            load_path = std::move(pending_load_path_);
            pending_load_path_.reset();
            source = preview_source_;
            filters = filter_state_;
            request_id = latest_render_request_id_;
            render_requested_ = false;
        }

        try {
            bool loaded_image = false;

            if (load_path) {
                source = vips::VImage::thumbnail(
                    load_path->c_str(),
                    static_cast<int>(kPreviewMaxEdge),
                    vips::VImage::option()
                        ->set("height", static_cast<int>(kPreviewMaxEdge))
                        ->set("size", VIPS_SIZE_DOWN)
                );
                loaded_image = true;
            }

            if (!source) {
                continue;
            }

            auto image = *source;

            const double contrast_scale = std::lerp(1.0, 2.6, filters.contrast);
            if (std::abs(contrast_scale - 1.0) > 0.001) {
                image = image.linear(contrast_scale, 128.0 * (1.0 - contrast_scale));
            }

            const double sharpness_sigma = std::lerp(0.0, 3.0, filters.sharpness);
            if (sharpness_sigma > 0.001) {
                image = image.sharpen(vips::VImage::option()->set("sigma", sharpness_sigma));
            }

            if (filters.sobel > 0.001) {
                const auto edges = image.sobel();
                image = image.linear(1.0 - filters.sobel, 0.0)
                            .add(edges.linear(filters.sobel, 0.0));
            }

            image = image.colourspace(VIPS_INTERPRETATION_sRGB).cast(VIPS_FORMAT_UCHAR);

            void* buffer = nullptr;
            std::size_t buffer_size = 0;
            image.write_to_buffer(
                ".jpg",
                &buffer,
                &buffer_size,
                vips::VImage::option()->set("Q", static_cast<int>(kPreviewJpegQuality))
            );

            std::vector<std::uint8_t> bytes;
            const auto* first = static_cast<const std::uint8_t*>(buffer);
            bytes.assign(first, first + buffer_size);
            g_free(buffer);

            {
                const std::scoped_lock lock(render_result_mutex_);
                pending_render_result_ = RenderResult{
                    request_id,
                    loaded_image,
                    filters,
                    loaded_image ? source : std::nullopt,
                    std::move(bytes)
                };
            }

            render_dispatcher_.emit();
        } catch (const std::exception& error) {
            std::cerr << "Preview render failed: " << error.what() << '\n';
        }
    }
}

void ImageFrame::on_render_ready()
{
    std::optional<RenderResult> result;

    {
        const std::scoped_lock lock(render_result_mutex_);
        result = std::move(pending_render_result_);
        pending_render_result_.reset();
    }

    if (!result) {
        return;
    }

    {
        const std::scoped_lock lock(render_mutex_);
        if (result->request_id < latest_render_request_id_) {
            return;
        }

        if (result->loaded_image && result->preview_source) {
            preview_source_ = std::move(result->preview_source);
        }
    }

    set_preview_bytes(result->bytes);
}

void ImageFrame::set_preview_bytes(const std::vector<std::uint8_t>& bytes)
{
    auto loader = Gdk::PixbufLoader::create("jpeg");
    loader->write(bytes.data(), bytes.size());
    loader->close();
    picture_.set_pixbuf(loader->get_pixbuf());
}
