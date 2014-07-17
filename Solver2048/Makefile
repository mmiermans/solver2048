SHELL = /bin/sh
CC    = g++

FLAGS        = -msse -msse2 -msse3 -msse4 -std=c++11
CFLAGS       = -Wall -march=native
DEBUGFLAGS   = -O0 -g -D _DEBUG
RELEASEFLAGS = -O3
LFLAGS       = $(DEBUG)

TARGET  = Solver2048
SOURCES = $(shell echo *.cpp)
COMMON  = bitmath.h board.h direction.h engine.h
HEADERS = $(shell echo *.h)
OBJECTS = $(SOURCES:.cpp=.o)

PREFIX = $(DESTDIR)/usr/local
BINDIR = $(PREFIX)/bin

all : $(TARGET)
debug : $(TARGET)

$(TARGET): $(OBJECTS) $(COMMON)
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -o $(TARGET) $(OBJECTS)

release : $(SOURCES) $(HEADERS) $(COMMON)
	$(CC) $(FLAGS) $(CFLAGS) $(RELEASEFLAGS) -o $(TARGET) $(SOURCES)

profile: CFLAGS += -pg
profile: $(TARGET)
 
 
install: release
	install -D $(TARGET) $(BINDIR)/$(TARGET)
 
install-strip: release
	install -D -s $(TARGET) $(BINDIR)/$(TARGET)
 
uninstall:
	-rm $(BINDIR)/$(TARGET)


clean:
	-rm -f $(OBJECTS)
	-rm -f gmon.out
 
distclean: clean
	-rm -f $(TARGET)


.SECONDEXPANSION:

%.o: %.cpp $(HEADERS) $(COMMON)
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -c -o $@ $<


.PHONY : all profile release \
	install install-strip uninstall clean distclean


