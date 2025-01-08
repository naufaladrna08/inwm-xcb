/*
 * X Window Manger
 *
 * (c) Naufal Adriansyah 2024-2025. Use at your own risk.
 */

#ifndef WM_HPP
#define WM_HPP
#include <iostream>

extern "C" {
	#include <xcb/xcb.h>
	#include <xcb/xcb_atom.h>
	#include <xcb/xcb_ewmh.h>
	#include <stdlib.h>
	#include <string.h>
}

#include <iostream>
#include <memory>
#include <unordered_map>
#include <core.hpp>

class WindowManager {
	public:
		/* For creating the window manager instance */ 
		static std::unique_ptr<WindowManager> create();
		~WindowManager();
		
		void run();
		
		static int on_xcb_error(xcb_connection_t* conn);
		static void check_another_wm(xcb_connection_t* conn, xcb_generic_error_t* err);

		/* event handlers */
		void on_create_notify(xcb_create_notify_event_t* event);
		void on_destroy_notify(xcb_destroy_notify_event_t* event);
		void on_reparent_notify(xcb_reparent_notify_event_t* event);
		void on_configure_request(xcb_configure_request_event_t* event);
		void on_configure_notify(xcb_configure_notify_event_t* event);
		void on_map_request(xcb_map_request_event_t* event);
		void on_map_notify(xcb_map_notify_event_t* event);
		void on_unmap_notify(xcb_unmap_notify_event_t* event);
		void on_button_press(xcb_button_press_event_t* event);
		void on_button_release(xcb_button_release_event_t* event);
		void on_motion_notify(xcb_motion_notify_event_t* event);

		void frame(xcb_window_t window, bool was_created_before_window_manager);
		void unframe(xcb_window_t window);
		
	private:
		WindowManager(xcb_connection_t* conn);
		
		xcb_connection_t* m_connection;
		xcb_window_t* m_root;
		xcb_screen_t* m_screen;
		
		static bool m_wm_detected;
		
		/* Map for window and it's frame */
		std::unordered_map<xcb_window_t, xcb_window_t> m_clients;
};

#endif