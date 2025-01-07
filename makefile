CXXFLAGS ?= -Wall -g
CXXFLAGS += `pkg-config --cflags xcb` -I.
LDFLAGS += `pkg-config --libs xcb` -lxcb-util   -I.

all: inwm

HEADERS = \
	wm.hpp
SOURCES = \
	wm.cpp \
	main.cpp
OBJECTS = $(SOURCES:.c=.o)

inwm: $(HEADERS) $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f inwm $(OBJECTS)
