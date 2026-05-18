#include "hello_window.h"
#include "image_frame.h"
#include "sliders_frame.h"

HelloWindow::HelloWindow() : 
    box_(Gtk::Orientation::HORIZONTAL, 12),
    image_frame_(),
    sliders_frame_()
{
  set_title("gtkmm Hello World");
  maximize();

  sliders_frame_.set_contrast_changed_handler(
      [this](double contrast) { 
          image_frame_.set_contrast(contrast);
      });
  sliders_frame_.set_sharpness_changed_handler([this](double intensity) {
      image_frame_.set_sharpness(intensity);
  });

  sliders_frame_.set_sobel_changed_handler(
      [this](double intensity) {
          image_frame_.set_sobel(intensity);
      }
  );

  box_.append(image_frame_);
  box_.append(sliders_frame_);

  set_child(box_);
}
