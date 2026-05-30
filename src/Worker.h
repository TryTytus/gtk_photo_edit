#pragma once

#include <glibmm/ustring.h>
#include <mutex>
#include <vips/image.h>
#include <vips/vips8>

class ImageFrame;

class Worker
{
public:
    Worker();

    void begin_work();
    void do_work(ImageFrame* caller);

    void get_data(vips::VImage*);
    void stop_work();
    bool has_stopped() const;

private:
    mutable std::mutex mutex_;
    bool shallow_stop_;
    bool has_stopped_;
    bool done_;
    VipsImage* active_image_;
    Glib::ustring message_;
};
