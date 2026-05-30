#include "Worker.h"

#include "Filters.h"
#include "image_frame.h"

#include <exception>
#include <iostream>
#include <mutex>
#include <vips/VImage8.h>
#include <vips/image.h>

Worker::Worker() :
    mutex_{},
    shallow_stop_{false},
    has_stopped_{false},
    done_{false},
    active_image_{nullptr},
    message_{}
{
    
}

void Worker::begin_work()
{
    std::lock_guard<std::mutex> lock {mutex_};
    shallow_stop_ = false;
    has_stopped_ = false;
    active_image_ = nullptr;
    message_ = "";
}

void Worker::stop_work()
{
    std::lock_guard<std::mutex> lock{mutex_};
    if (active_image_) {
        vips_image_set_kill(active_image_, true);
    }
    shallow_stop_ = true;
}

bool Worker::has_stopped() const
{
    std::lock_guard<std::mutex> lock{mutex_};
    return has_stopped_;
}

void Worker::do_work(ImageFrame* frame)
{
    try {
        const auto state = frame->state_;
        auto image = frame->filters_.update(*(frame->filters_.orginal_), state);

        {
            std::lock_guard<std::mutex> lock {mutex_};
            if (shallow_stop_) {
                shallow_stop_ = false;
                has_stopped_ = true;
                frame->notify();
                return;
            }

            active_image_ = image.get_image();
            vips_image_set_kill(active_image_, false);
        }

        image = image.copy_memory();

        {
            std::lock_guard<std::mutex> lock {mutex_};
            active_image_ = nullptr;
            if (shallow_stop_) {
                shallow_stop_ = false;
                has_stopped_ = true;
                frame->notify();
                return;
            }

            frame->filters_.current_ = image;
            has_stopped_ = true;
        }

        frame->notify();
    } catch (const std::exception& error) {
        std::lock_guard<std::mutex> lock {mutex_};
        active_image_ = nullptr;
        if (!shallow_stop_) {
            std::cerr << "Image render failed: " << error.what() << '\n';
        }
        shallow_stop_ = false;
        has_stopped_ = true;
        frame->notify();
    }
}
