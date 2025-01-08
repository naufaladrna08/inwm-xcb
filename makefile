CXXFLAGS ?= -Wall -g
CXXFLAGS += `pkg-config --cflags xcb` -I.
LDFLAGS += `pkg-config --libs xcb` -lxcb-util -lxcb-ewmh  -I.

all: inwm inpanel

HEADERS = wm.hpp core.hpp
SOURCES = wm.cpp main.cpp core.cpp
OBJECTS = wm.o main.o core.o

PANEL_HEADERS = panel.hpp core.hpp
PANEL_SOURCES = panel.cpp core.cpp
PANEL_OBJECTS = panel.o core.o

inwm: $(HEADERS) $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

inpanel: $(PANEL_HEADERS) $(PANEL_OBJECTS)
	$(CXX) -o $@ $(PANEL_OBJECTS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f inwm $(OBJECTS)
