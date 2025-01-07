CXXFLAGS ?= -Wall -g
CXXFLAGS += `pkg-config --cflags xcb` -I.
LDFLAGS += `pkg-config --libs xcb` -lxcb-util   -I.

all: inwm inpanel

HEADERS = wm.hpp
SOURCES = wm.cpp main.cpp
OBJECTS = wm.o main.o

PANEL_HEADERS = panel.hpp
PANEL_SOURCES = panel.cpp
PANEL_OBJECTS = panel.o

inwm: $(HEADERS) $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

inpanel: $(PANEL_HEADERS) $(PANEL_OBJECTS)
	$(CXX) -o $@ $(PANEL_OBJECTS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f inwm $(OBJECTS)
