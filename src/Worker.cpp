#include "Worker.h"

#include "Filters.h"
#include "image_frame.h"

#include <mutex>
#include <vips/VImage8.h>
#include <vips/image.h>

Worker::Worker() :
    mutex_{},
    shallow_stop_{false},
    has_stopped_{false},
    done_{false},
    message_{}
{
    
}

void Worker::stop_work(vips::VImage* image)
{
    std::lock_guard<std::mutex> lock{mutex_};
    vips_image_set_kill((*image).get_image(), true);
    shallow_stop_ = true;
}

bool Worker::has_stopped() const
{
    std::lock_guard<std::mutex> lock{mutex_};
    return has_stopped_;
}

void Worker::do_work(ImageFrame* frame)
{
    std::lock_guard<std::mutex> lock {mutex_};
    has_stopped_ = false;
    message_ = "";

    Filters filters {};
    frame->current_ = filters.update(*(frame->orginal_), frame->state_);

    shallow_stop_ = false;
    has_stopped_ = true;

    frame->notify();

    
}
