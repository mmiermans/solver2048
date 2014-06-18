#include <iostream>
#include "engine.h"

Engine::Engine(Board board)
	: board(board)
{
}


Engine::~Engine()
{
}

Move Engine::solve() {
	int moves = board.getValidMoves();

	if (moves & Move::Left)
		std::cout << "Left move is valid." << std::endl;

	if (moves & Move::Right)
		std::cout << "Right move is valid." << std::endl;

	return Move::Left;
}
