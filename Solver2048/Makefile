OBJS = board.o engine.o searchnode.o Solver2048.o
CC = g++
DEBUG = -g
CFLAGS = -c $(DEBUG) -msse -msse2 -msse3 -msse4 -std=c++11 # -march= -mfpmath=sse
LFLAGS = $(DEBUG)

debug : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o Solver2048

bitmath : FastRand.h

DEPS_BOARD_H = board.h precomputedmoves.h bitmath.h direction.h boardconstants.h
board.o : board.cpp $(DEPS_BOARD_H) 
	$(CC) $(CFLAGS) board.cpp

DEPS_SEARCHNODE_H = searchnode.h $(DEPS_BOARD_H)
searchnode.o : searchnode.cpp $(DEPS_SEARCHNODE_H)
	$(CC) $(CFLAGS) searchnode.cpp

DEPS_ENGINE_H = engine.h boardhashtable.h $(DEPS_SEARCHNODE_H)
engine.o : engine.cpp $(DEPS_ENGINE_H)
	$(CC) $(CFLAGS) engine.cpp

Solver2048.o : Solver2048.cpp $(DEPS_ENGINE_H)
	$(CC) $(CFLAGS) Solver2048.cpp

clean:
	\rm *.o *~ p1

