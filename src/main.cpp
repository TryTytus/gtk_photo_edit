#include "hello_window.h"
#include <gtkmm.h>

int main(int argc, char* argv[]) {
  auto app = Gtk::Application::create("com.example.gtk_first");
  return app->make_window_and_run<HelloWindow>(argc, argv);
}
