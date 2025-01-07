#ifndef PANEL_HPP
#define PANEL_HPP
extern "C" {
  #include <xcb/xcb.h>
}

#include <iostream>
#include <cstdlib>
#include <cstring>

class Panel {
  public:
    Panel();
    ~Panel();

    void run();
    void set_panel_window_type(xcb_connection_t* connection, xcb_window_t window);

  private:
    xcb_connection_t* m_connection;
    xcb_screen_t* m_screen;
    xcb_window_t m_root;

    int m_width, m_height;
};

#endif