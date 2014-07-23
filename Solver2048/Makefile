SHELL = /bin/sh
CC    = g++

MYSQLFLAGS   = -I/usr/include/mysql -DBIG_JOINS=1 -fno-strict-aliasing -DNDEBUG -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl
CFLAGS       = -Wall -march=native
FLAGS        = -msse -msse2 -msse3 -msse4 -std=c++11 $(CFLAGS)
DEBUGFLAGS   = -O0 -g -D _DEBUG
RELEASEFLAGS = -O3
ENABLE_MYSQL = 0

ifeq (1, $(ENABLE_MYSQL))
	FLAGS += -DENABLE_MYSQL $(MYSQLFLAGS)
endif

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
	-rm -f $(TARGET)
	$(CC) -o $(TARGET) $(OBJECTS) $(FLAGS) $(DEBUGFLAGS)

release : clean $(SOURCES) $(HEADERS) $(COMMON)
	$(CC) -o $(TARGET) $(SOURCES) $(FLAGS) $(RELEASEFLAGS)

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
	$(CC) -c -o $@ $< $(FLAGS) $(DEBUGFLAGS)


.PHONY : all profile release debug \
	install install-strip uninstall clean distclean

