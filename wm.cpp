#include <wm.hpp>
bool WindowManager::m_wm_detected = false;

std::unique_ptr<WindowManager> WindowManager::create() {
  /* Open connection to X server */
  xcb_connection_t* conn = xcb_connect(NULL, NULL);
  if (conn == nullptr) {
    std::cerr << "Cannot open connection to the X server\n";
    return nullptr;
  }
  
  return std::unique_ptr<WindowManager>(new WindowManager(conn));
}

WindowManager::WindowManager(xcb_connection_t* conn) {
  /* The `conn` should be not null by now */
  m_connection = conn;
  
  /* In order to get the root window, we need to get the screen first */
  const xcb_setup_t* setup = xcb_get_setup(conn);
  xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
  
  m_screen = iter.data;
  m_root = &m_screen->root;
}

WindowManager::~WindowManager() {
  xcb_disconnect(m_connection);
}

void WindowManager::run() {
  m_wm_detected = false;
  
  uint32_t root_mask   = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  uint32_t mask_val[2] = { 
    0xff9da1cd, XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_EXPOSURE
  };

  xcb_void_cookie_t chwin_cookie = xcb_change_window_attributes_checked(m_connection, *m_root, root_mask, mask_val);
  xcb_generic_error_t* err = xcb_request_check(m_connection, chwin_cookie);
  if (err) {
    check_another_wm(m_connection, err);
    
    std::cerr << "Failed to change root attributes with error code " << err->error_code << std::endl
              << "Please contact your IT support and make sure you'll get your answer" << std::endl;
    
    free(err);
    exit(EXIT_FAILURE);
  }

  /* Clear so that we can see the "Windows 1.0" background color */
  xcb_clear_area(m_connection, 0, *m_root, 0, 0, m_screen->width_in_pixels, m_screen->height_in_pixels);

  /* Frame existing top-level windows */
  xcb_query_tree_cookie_t tlw_cookie = xcb_query_tree(m_connection, *m_root);
  xcb_query_tree_reply_t* tlw_reply = xcb_query_tree_reply(m_connection, tlw_cookie, NULL);

  xcb_window_t *children = xcb_query_tree_children(tlw_reply);
  uint32_t num_children = xcb_query_tree_children_length(tlw_reply);

  if (tlw_reply->root != *m_root) {
    free(tlw_reply);
    return;
  }

  // Frame each top-level window
  std::cout << "Num of children is " << num_children << std::endl;
  if (num_children > 3) std::cout << "Geez, take it easy dude.\n";
  for (uint32_t i = 0; i < num_children; i++) {
    frame(children[i], true);
  }

  free(tlw_reply);

  /* Grab Button */
  xcb_grab_button(m_connection, 0, *m_root,
    XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE,
    XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC,
    *m_root, XCB_NONE, XCB_BUTTON_INDEX_1, XCB_MOD_MASK_1);

  xcb_generic_event_t* event;
  while ((event = xcb_wait_for_event(m_connection))) {
    switch (event->response_type & ~0x80) {
      case XCB_CREATE_NOTIFY:     on_create_notify((xcb_create_notify_event_t*) event);         break;
      case XCB_DESTROY_NOTIFY:    on_destroy_notify((xcb_destroy_notify_event_t*) event);       break;
      case XCB_REPARENT_NOTIFY:   on_reparent_notify((xcb_reparent_notify_event_t*) event);     break;
      case XCB_CONFIGURE_REQUEST: on_configure_request((xcb_configure_request_event_t*) event); break;
      case XCB_CONFIGURE_NOTIFY:  on_configure_notify((xcb_configure_notify_event_t*) event);   break;
      case XCB_MAP_REQUEST:       on_map_request((xcb_map_request_event_t*) event);             break;
      case XCB_MAP_NOTIFY:        on_map_notify((xcb_map_notify_event_t*) event);               break;
      case XCB_UNMAP_NOTIFY:      on_unmap_notify((xcb_unmap_notify_event_t*) event);           break;
      case XCB_BUTTON_PRESS:      on_button_press((xcb_button_press_event_t*) event);           break;
      case XCB_BUTTON_RELEASE:    on_button_release((xcb_button_release_event_t*) event);       break;
      case XCB_MOTION_NOTIFY:     on_motion_notify((xcb_motion_notify_event_t*) event);         break;
      
      case XCB_EXPOSE: break;
      default:
        std::cout << "UNHANDLED EVENT\n";
    }

    free(event);
  }
}

void WindowManager::check_another_wm(xcb_connection_t* conn, xcb_generic_error_t* err) {
  if (err->error_code == XCB_ACCESS) {
    std::cerr << "Another window manager has been detected." << std::endl
              << "Please contact your IT support and make sure you'll get your answer" << std::endl;
    m_wm_detected = true;
    free(err);
    exit(EXIT_FAILURE);
  }
}

void WindowManager::on_create_notify(xcb_create_notify_event_t* event) {

}

void WindowManager::on_destroy_notify(xcb_destroy_notify_event_t* event) {

}

void WindowManager::on_reparent_notify(xcb_reparent_notify_event_t* event) {

}

void WindowManager::on_configure_request(xcb_configure_request_event_t* e) {
  xcb_configure_window_value_list_t changes;
  uint16_t mask = 0;   // Will hold which values we're actually changing
  uint32_t values[7];  // Values buffer for configure window

  // Copy fields and set corresponding mask bits
  if (e->value_mask & XCB_CONFIG_WINDOW_X) {
    values[0] = e->x;
    mask |= XCB_CONFIG_WINDOW_X;
  }
  if (e->value_mask & XCB_CONFIG_WINDOW_Y) {
    values[1] = e->y;
    mask |= XCB_CONFIG_WINDOW_Y;
  }
  if (e->value_mask & XCB_CONFIG_WINDOW_WIDTH) {
    values[2] = e->width;
    mask |= XCB_CONFIG_WINDOW_WIDTH;
  }
  if (e->value_mask & XCB_CONFIG_WINDOW_HEIGHT) {
    values[3] = e->height;
    mask |= XCB_CONFIG_WINDOW_HEIGHT;
  }
  if (e->value_mask & XCB_CONFIG_WINDOW_BORDER_WIDTH) {
    values[4] = e->border_width;
    mask |= XCB_CONFIG_WINDOW_BORDER_WIDTH;
  }
  if (e->value_mask & XCB_CONFIG_WINDOW_SIBLING) {
    values[5] = e->sibling;
    mask |= XCB_CONFIG_WINDOW_SIBLING;
  }
  if (e->value_mask & XCB_CONFIG_WINDOW_STACK_MODE) {
    values[6] = e->stack_mode;
    mask |= XCB_CONFIG_WINDOW_STACK_MODE;
  }

  // Configure the window
  xcb_void_cookie_t cookie = xcb_configure_window(m_connection,
    e->window,
    mask,
    values
  );

  xcb_generic_error_t *error = xcb_request_check(m_connection, cookie);
  if (error) {
    std::cerr << "An error occured when trying to configure client window" << std::endl
              << "Please contact your IT support and make sure you'll get your answer" << std::endl;
    free(error);
  }

  /* Grant request */
  if (m_clients.count(e->window)) {
    const xcb_window_t frame = m_clients[e->window];
    xcb_configure_window(m_connection, frame, mask, values);
  }

  xcb_configure_window(m_connection, e->window, mask, values);
}

void WindowManager::on_configure_notify(xcb_configure_notify_event_t* event) {

}

void WindowManager::on_map_request(xcb_map_request_event_t* event) {
  frame(event->window, false);
  
  xcb_void_cookie_t map_cookie = xcb_map_window_checked(m_connection, event->window);
  xcb_generic_error_t *error = xcb_request_check(m_connection, map_cookie);
  if (error) {
    std::cerr << "An error occured when trying to map client window" << std::endl
              << "Please contact your IT support and make sure you'll get your answer" << std::endl;
    free(error);
  }
}

void WindowManager::on_map_notify(xcb_map_notify_event_t* event) {
  
}

void WindowManager::on_unmap_notify(xcb_unmap_notify_event_t* event) {
  if (!m_clients.count(event->window)) return;
  unframe(event->window);
}

void WindowManager::on_button_press(xcb_button_press_event_t* event) {

}

void WindowManager::on_button_release(xcb_button_release_event_t* event) {

}

void WindowManager::on_motion_notify(xcb_motion_notify_event_t* event) {
  
}

void WindowManager::frame(xcb_window_t window, bool was_created_before_window_manager) {
  const uint32_t border_width = 2;
  const uint64_t border_color = 0xFF222222;
  const uint64_t back_color = 0xFF222222;

  /* Retreive window attribute for the frame */
  xcb_get_window_attributes_cookie_t winattr_cookie = xcb_get_window_attributes(m_connection, window);
  xcb_get_geometry_cookie_t geom_cookie =  xcb_get_geometry(m_connection, window);

  xcb_get_window_attributes_reply_t* winattr = xcb_get_window_attributes_reply(m_connection, winattr_cookie, NULL);
  xcb_get_geometry_reply_t* geomattr = xcb_get_geometry_reply(m_connection, geom_cookie, NULL);

  if (!winattr || !geomattr) {
    std::cerr << "An error occured when trying to get window attributes" << std::endl
              << "Please contact your IT support and make sure you'll get your answer" << std::endl;

    free(winattr);
    free(geomattr);
    return;
  }

  if (was_created_before_window_manager) {
    if (winattr->override_redirect || winattr->map_state != XCB_MAP_STATE_VIEWABLE) return; 
  }

  uint32_t mask = XCB_CW_BORDER_PIXEL | XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK | XCB_CW_CURSOR;
  uint32_t values[3];
  values[0] = border_color;
  values[1] = back_color;
  values[2] = XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | 
    XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
    XCB_EVENT_MASK_BUTTON_PRESS |
    XCB_EVENT_MASK_BUTTON_RELEASE |
    XCB_EVENT_MASK_POINTER_MOTION |
    XCB_EVENT_MASK_ENTER_WINDOW |
    XCB_EVENT_MASK_LEAVE_WINDOW;

  xcb_font_t font = xcb_generate_id(m_connection);
  xcb_open_font(m_connection, font, strlen("cursor"), "cursor");
  xcb_cursor_t cursor = xcb_generate_id(m_connection);
  xcb_create_glyph_cursor(m_connection, cursor, font, font,
                          58, 58 + 1, 0, 0, 0,  // Appearance
                          0xFFFF, 0xFFFF, 0xFFFF);  // RGB colors
  values[3] = cursor;
  
  xcb_window_t frame = xcb_generate_id(m_connection);
  xcb_void_cookie_t framewin_cookie = xcb_create_window(
    m_connection, XCB_COPY_FROM_PARENT, frame, *m_root, geomattr->x, geomattr->x, 
    geomattr->width, geomattr->height,
    border_width, XCB_WINDOW_CLASS_INPUT_OUTPUT, XCB_COPY_FROM_PARENT , mask, values
  );

  xcb_generic_error_t* err = xcb_request_check(m_connection, framewin_cookie);
  if (err) {
    std::cerr << "An error occured when creating frame window" << std::endl
              << "Please contact your IT support and make sure you'll get your answer" << std::endl;
    free(err);
    return;
  }

  xcb_change_save_set(m_connection, XCB_SET_MODE_INSERT, window);
  xcb_reparent_window(m_connection, window, frame, 0, 0);
  xcb_map_window(m_connection, frame);
  xcb_flush(m_connection);

  /* Create a new map */
  m_clients[window] = frame;


  // Free resources
  xcb_free_cursor(m_connection, cursor);
  xcb_close_font(m_connection, font);

  /* Free up */
  free(winattr);
  free(geomattr);
  free(err);
}

void WindowManager::unframe(xcb_window_t window) {
  const xcb_window_t frame = m_clients[window];
  xcb_unmap_window(m_connection, frame); // TODO: Check for errors
  xcb_reparent_window(m_connection, window, *m_root, 0, 0); 
  xcb_change_save_set(m_connection, XCB_SET_MODE_DELETE, window);
  xcb_destroy_window(m_connection, frame);
  m_clients.erase(window);

  std::cout << "Unframed window" << std::endl;
}