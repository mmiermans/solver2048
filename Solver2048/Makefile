SHELL = /bin/sh
CC    = g++

MYSQLFLAGS   = -I/usr/include/mysql -DBIG_JOINS=1 -fno-strict-aliasing -DNDEBUG -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl
DEFS         =
CFLAGS       = -Wall -march=native
FLAGS        = $(DEFS) -msse4.1 $(CFLAGS)
DEBUGFLAGS   = -O0 -g -D _DEBUG
RELEASEFLAGS = -O3
ENABLE_MYSQL = 0
ENABLE_STDOUT = 1

ifeq (1, $(ENABLE_MYSQL))
	FLAGS += -DENABLE_MYSQL $(MYSQLFLAGS)
endif

ifeq (1, $(ENABLE_STDOUT))
	FLAGS += -DENABLE_STDOUT
endif

TARGET  = Solver2048
MODE    = debug
SOURCES = $(shell echo *.cpp)
COMMON  = bitmath.h board.h direction.h engine.h
HEADERS = $(shell echo *.h)
OBJECTS = $(addprefix Debug/,$(SOURCES:.cpp=.o))

PREFIX = $(DESTDIR)/usr/local
BINDIR = $(PREFIX)/bin

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

all : $(TARGET)
debug : $(TARGET)

# DEBUG ---------------------------------------------------
set_debug_mode:
	$(eval MODE = debug)
	@mkdir -p Debug
$(TARGET): set_debug_mode information $(OBJECTS) $(COMMON)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	$(CC) -o Debug/$(TARGET) $(OBJECTS) $(DEBUGFLAGS) $(FLAGS)
	@echo 'Finished building target: $@'
	@echo ' '

# RELEASE -------------------------------------------------
set_release_mode:
	$(eval MODE = release)
	@mkdir -p Release
release : set_release_mode information $(SOURCES) $(HEADERS) $(COMMON)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	$(CC) -o Release/$(TARGET) $(SOURCES) $(RELEASEFLAGS) $(FLAGS)
	@echo 'Finished building target: $@'
	@echo ' '

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
	-rm -rf Release/$(TARGET)
	-rm -rf Debug/$(TARGET)
 
distclean: clean
	-rm -f $(TARGET)

.SECONDEXPANSION:

Debug/%.o: %.cpp $(HEADERS) $(COMMON)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CC) -c -o $@ $< $(FLAGS) $(DEBUGFLAGS)
	@echo 'Finished building: $<'
	@echo ' '


.PHONY : all profile release debug \
	install install-strip uninstall clean distclean \
	set_release_mode set_debug_mode

