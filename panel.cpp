#include <panel.hpp>
#include <memory>

Panel::Panel() {
  /* Create X server connection */
  m_connection = xcb_connect(NULL, NULL);

  /* Get screen and root window */
  const xcb_setup_t* setup = xcb_get_setup(m_connection);
  xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
  
  m_screen = iter.data;
  m_root = m_screen->root;
  m_width = m_screen->width_in_pixels;
  m_height = 32;
}

Panel::~Panel() {
  xcb_disconnect(m_connection);
}

void Panel::run() {
  /* Create a simple window */
  xcb_window_t panel = xcb_generate_id(m_connection);
  uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  uint32_t val[2] = { 0xFFDEDEDE, XCB_EVENT_MASK_EXPOSURE };
  xcb_void_cookie_t panel_cookie = xcb_create_window_checked(
    m_connection, XCB_COPY_FROM_PARENT, panel, m_root, 0, 0, m_width, 32, 2, 
    XCB_WINDOW_CLASS_INPUT_OUTPUT, m_screen->root_visual, mask, val
  );

  /* Check for errors */
  xcb_generic_error_t* err = xcb_request_check(m_connection, panel_cookie);
  if (err) {
    std::cerr << "Canot create panel window with error code " << err->error_code << std::endl;
    free(err);
    exit(EXIT_FAILURE);
  }
  free(err);

  set_panel_window_type(m_connection, panel);

  xcb_map_window(m_connection, panel);
  xcb_flush(m_connection);

  /* Application Loop */
  xcb_generic_event_t* event;
  while ((event = xcb_wait_for_event(m_connection))) {
    switch (event->response_type & ~0x80) {
      case XCB_EXPOSE:
        std::cout << "Drawing...\n";
        break;

      default: break;
    }

    free(event);
  }
}

void Panel::set_panel_window_type(xcb_connection_t* connection, xcb_window_t window) {
  xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 0, strlen("_NET_WM_WINDOW_TYPE"), "_NET_WM_WINDOW_TYPE");
  xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(connection, cookie, NULL);

  xcb_intern_atom_cookie_t dock_cookie = xcb_intern_atom(connection, 0, strlen("_NET_WM_WINDOW_TYPE_DOCK"), "_NET_WM_WINDOW_TYPE_DOCK");
  xcb_intern_atom_reply_t* dock_reply = xcb_intern_atom_reply(connection, dock_cookie, NULL);

  if (reply && dock_reply) {
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window,
                        reply->atom, XCB_ATOM_ATOM, 32, 1, &dock_reply->atom);
  }

  free(reply);
  free(dock_reply);
}

int main(int argc, char const *argv[]) {
  std::unique_ptr<Panel> panel = std::make_unique<Panel>();
  panel->run();

  return 0;
}
