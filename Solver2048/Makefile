SHELL = /bin/sh
CC    = g++

MYSQLFLAGS   = -I/usr/include/mysql -DBIG_JOINS=1 -fno-strict-aliasing -DNDEBUG -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl
DEFS         =
CFLAGS       = -Wall -march=native
FLAGS        = -msse -msse2 -msse3 -msse4 $(CFLAGS) $(DEFS)
DEBUGFLAGS   = -O0 -g -D _DEBUG
RELEASEFLAGS = -O3
ENABLE_MYSQL = 0
ENABLE_STDOUT = 1

ifeq (1, $(ENABLE_MYSQL))
	FLAGS += -DENABLE_MYSQL $(MYSQLFLAGS)
endif

TARGET  = Solver2048
MODE    = debug
SOURCES = $(shell echo *.cpp)
COMMON  = bitmath.h board.h direction.h engine.h
HEADERS = $(shell echo *.h)
OBJECTS = $(SOURCES:.cpp=.o)

PREFIX = $(DESTDIR)/usr/local
BINDIR = $(PREFIX)/bin

all : $(TARGET)
debug : $(TARGET)

$(TARGET): information $(OBJECTS) $(COMMON)
	-rm -f $(TARGET)
	$(CC) -o $(TARGET) $(OBJECTS) $(DEBUGFLAGS) $(FLAGS)

release :
	MODE = release
	clean information $(SOURCES) $(HEADERS) $(COMMON)
	$(CC) -o $(TARGET) $(SOURCES) $(RELEASEFLAGS) $(FLAGS)

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

information:
	@echo " _____     _             ___ ___ ___ ___"
	@echo "|   __|___| |_ _ ___ ___|_  |   | | | . |"
	@echo "|__   | . | | | | -_|  _|  _| | |_  | . |"
	@echo "|_____|___|_|\_/|___|_| |___|___| |_|___|"
	@echo ""
	@echo "Building in "$(MODE)" mode"
ifeq (1, $(ENABLE_MYSQL))
	@echo "|x| MySQL"
else
	@echo "| | MySQL"
endif
ifeq (1, $(ENABLE_STDOUT))
	@echo "|x| stdout"
else
	@echo "| | stdout"
endif
	@echo ""

.SECONDEXPANSION:

%.o: %.cpp $(HEADERS) $(COMMON)
	$(CC) -c -o $@ $< $(FLAGS) $(DEBUGFLAGS)


.PHONY : all profile release debug \
	install install-strip uninstall clean distclean

