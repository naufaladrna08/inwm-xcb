#include <core.hpp>

void draw_text(xcb_connection_t* conn, xcb_screen_t* screen, xcb_drawable_t window, short x1, short y1, const char* label) {
  xcb_gcontext_t font_gc = get_font_gc(conn, screen, window, "fixed");
  xcb_void_cookie_t text_cookie = xcb_image_text_8_checked(conn, strlen(label), window, font_gc, x1, y1, label);
  xcb_generic_error_t* err = xcb_request_check(conn, text_cookie);
  if (err) {
    std::cerr << "Failed to draw text with error code " << err->error_code << std::endl;
    free(err);
  }

  free(err);
}

void draw_text_with_font(xcb_connection_t* conn, xcb_screen_t* screen, xcb_drawable_t window, short x1, short y1, const char* label, const char* font) {
  xcb_gcontext_t font_gc = get_font_gc(conn, screen, window, label);
  xcb_void_cookie_t text_cookie = xcb_image_text_8_checked(conn, strlen(label), window, font_gc, x1, y1, label);
  xcb_generic_error_t* err = xcb_request_check(conn, text_cookie);
  if (err) {
    std::cerr << "Failed to draw text with error code " << err->error_code << std::endl;
    free(err);
  }

  free(err);
}

xcb_gcontext_t get_font_gc(xcb_connection_t* conn, xcb_screen_t* screen, xcb_drawable_t window, const char* font) {
  xcb_font_t font_id = xcb_generate_id(conn);
  xcb_void_cookie_t font_cookie = xcb_open_font(conn, font_id, strlen(font), font);
  xcb_generic_error_t* err = xcb_request_check(conn, font_cookie);
  if (err) {
    std::cerr << "Failed to open font with error code " << err->error_code << std::endl;
    free(err);
    return 1;
  }

  free(err);

  xcb_gcontext_t gc = xcb_generate_id(conn);
  uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
  uint32_t values[3];
  values[0] = 0xFF222222;
  values[1] = 0xFFFFFFFF;
  values[2] = font_id;
  xcb_void_cookie_t gc_cookie = xcb_create_gc_checked(conn, gc, window, mask, values);

  err = xcb_request_check(conn, gc_cookie);
  if (err) {
    std::cerr << "Failed to create graphical context error code " << err->error_code << std::endl;
    free(err);
    return 1;
  }

  free(err);
  return gc;
}

xcb_atom_t get_atom(xcb_connection_t* conn, const char* atom_name) {
  xcb_intern_atom_cookie_t cookie = xcb_intern_atom(conn, 0, strlen(atom_name), atom_name);
  xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(conn, cookie, NULL);

  if (!reply) {
    std::cerr << "An error occured when get atom" << std::endl
              << "Please contact your IT support and make sure you'll get your answer" << std::endl;
    return XCB_ATOM_NONE;
  }

  xcb_atom_t atom = reply->atom;
  free(reply);
  return atom;
}