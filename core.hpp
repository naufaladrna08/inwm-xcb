#ifndef CORE_HPP
#define CORE_HPP

#define BORDER_SIZE 2
#define PANEL_SIZE 32

extern "C" {
  #include <xcb/xcb.h>
  #include <string.h>
  #include <stdlib.h>
}
#include <iostream>

/*
 * draw_text 
 * 
 * Draw text with and without default font
 */
void draw_text(xcb_connection_t* conn, xcb_screen_t* screen, xcb_drawable_t window, short x1, short y1, const char* label);
void draw_text_with_font(xcb_connection_t* conn, xcb_screen_t* screen, xcb_drawable_t window, short x1, short y1, const char* label, const char* font);
xcb_gcontext_t get_font_gc(xcb_connection_t* conn, xcb_screen_t* screen, xcb_drawable_t window, const char* font);
xcb_atom_t get_atom(xcb_connection_t* conn, const char* atom_name);

#endif