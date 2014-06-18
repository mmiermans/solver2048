#pragma once

#include "board.h"
#include "direction.h"

class Engine
{
public:
	bool debug = true;
	int depth = 10;

	Engine(Board board);
	~Engine();

	Move solve();

private:
	Board board;
};

