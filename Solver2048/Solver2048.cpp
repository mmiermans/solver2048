// Solver2048.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <iomanip>
#include <string>
#include <time.h>
#include <math.h>

#include "mysqlconnector.h"

#include "board.h"
#include "engine.h"
#include "searchnode.h"
#include "bitmath.h"

using namespace std;

#define ENABLE_STDOUT

void askTile(Board& board) {
	int x = 0;
	int y = 0;
	Tile tile = 0;
	cout << "Enter <row> <col> <val> (0 0 2 sets top-left to 2): ";
	cin >> x;
	cin >> y;
	cin >> tile;
	tile = (Tile)round(log2((double)tile));
	BoardLogic::setTile(board, x, y, tile);
}

void playSimpleStrategy() {
	Board b;
	Engine e;

	for (int i = 0; i < 5; i++) {
		clock_t startTime = clock();

		uint64_t attempts = 0;
		uint64_t moveCount = 0;
		SearchNode* sn = new SearchNode();

		while (startTime + (3 * CLOCKS_PER_SEC) > clock()) {

			for (int j = 0; j < 1000; j++) {
				bool hasPossibleMove = true;
				BoardLogic::clearBoard(b);

				while (hasPossibleMove) {
					e.setRandomTile(b);

					// Do move
					char moves = BoardLogic::getValidMoves(b);
					if (moves & Move::Up) {
						b = BoardLogic::performMove(b, Move::Up);
					} else {
						Move::MoveEnum moveA = Move::Left;
						Move::MoveEnum moveB = Move::Right;
						Tile firstRowLastTile = (b >> (3 * TILE_BITS)) & TILE_MASK;
						if (firstRowLastTile >= 1) {
							if (((firstRowLastTile * MASK_ROW_LSB) + 0x0123) == (b & MASK_ROW_FIRST)) {
								moveA = Move::Right;
								moveB = Move::Left;
							}
						}

						if (moves & moveA) {
							b = BoardLogic::performMove(b, moveA);
						} else if (moves & moveB) {
							b = BoardLogic::performMove(b, moveB);
						} else if (moves & Move::Down) {
							b = BoardLogic::performMove(b, Move::Down);
						} else {
							// Game over!
							hasPossibleMove = false;
						}
					}

					moveCount++;
				}

				attempts++;
			}
		}

		clock_t endTime = clock();
		cout << (CLOCKS_PER_SEC * moveCount) / (endTime - startTime) << " moves/s" << endl;

		delete sn;
	}

	getchar();

}

void printChildBoards(SearchNode& sn) {
	for (int i = 0; i < 4; i++) {
		for (int v = 0; v < 2; v++) {
			for (int j = 0; j < sn.childCount[i][v]; j++) {
				ChildNode& childNode = sn.children[i][j][v];
				Board childBoard = childNode.board;
				cout << "Value: " << (1 << (v + 1)) << "\tPositions: " << (int)childNode.positions[0];
				for (int k = 1; k < 4 && childNode.positions[k] > 0; k++) {
					cout << ", " << (int)childNode.positions[k];
				}
				const char* moveNames[4] = { "Up", "Right", "Down", "Left" };
				cout << "\tMove: " << moveNames[i] << endl;
				BoardLogic::printBoard(childBoard);
			}
		}
	}
}

void precomputeMoves() {
	for (int i = 0; i < 1 << 16; i++) {
		Board iBoard = (Board)i;
		Board result = BoardLogic::performMove(iBoard, Move::Right);

		if ((result & ~0xFFFF) != 0)
			result = 0xFFFF;

		cout << "0x" << std::hex << setw(4) << uppercase << setfill('0') << result;
		if (i != (1 << 16) - 1)
			cout << ", ";
			if (i % 16 == 15)
				cout << " \\\\" << endl;
	}
	getchar();
}

int main(int argc, char* argv[]) {
#ifdef ENABLE_MYSQL
	MySqlConnector mySqlConnector;
#endif

	Engine e;

	Board b = 0;
	int moveCount = 0;
	mySqlConnector.startGame(b, moveCount);

	// Set two random tiles if this is a new game.
	if (b == 0) {
		e.setRandomTile(b);
		e.setRandomTile(b);
	}

	// Debug variables
	clock_t lastPrintTime = 0;
	int lastMoveCount = 0;
	int lastCost = 0;
	clock_t startTime = clock();
	int printStep = CLOCKS_PER_SEC;
	int maxLookAhead = 0;
	clock_t currentTime;

	// Main game loop
	bool hasValidMove = true;
	while (hasValidMove) {
		// Let game engine perform evaluation on board b.
		Move::MoveEnum bestMove = e.solve(b);

		// Execute move.
		moveCount++;
		b = BoardLogic::performMove(b, bestMove);
		// Insert new random tile.
		int newTilePosition = 0;
		Tile newTileValue = 0;
		e.getRandomTile(b, newTilePosition, newTileValue);
		BoardLogic::setTile(b, newTilePosition, newTileValue);

		// Gameover?
		hasValidMove = (BoardLogic::getValidMoves(b) != (Move::MoveEnum)0);

#ifdef ENABLE_STDOUT
		// Debug info
		if (e.dfsLookAhead > maxLookAhead)
			maxLookAhead = e.dfsLookAhead;
		int cost = e.evaluateBoard(b);
		currentTime = clock();
		if (cost - lastCost > 1024 || hasValidMove == false || currentTime - lastPrintTime >= printStep) {
			// Game statistics
			cout << "Moves: " << moveCount << "\t";
			cout << "Time: " << (currentTime - startTime) / CLOCKS_PER_SEC << "s\t";
			cout << "Score: " << BoardLogic::calculateScore(b) << "\t";
			cout << "BoardCost: " << cost;
			cout << endl;

			// Engine performance statistics
			cout << "LookAhead: " << maxLookAhead << "\t";
			maxLookAhead = 0;
			cout << "Moves/s: " << (CLOCKS_PER_SEC * (moveCount - lastMoveCount)) / (float)(currentTime - lastPrintTime) << "\t";
			cout << "AvgMoves/s: " << (CLOCKS_PER_SEC * moveCount) / (float)(currentTime - startTime) << "\t";
			lastMoveCount = moveCount;
			int kNodesPerSec = e.cpuTime == 0 ? 9999 : (int)((CLOCKS_PER_SEC * e.nodeCounter) / (1000 * e.cpuTime));
			cout << "kNodes/s: " << kNodesPerSec << "\t";
			cout << endl;

			BoardLogic::printBoard(b);
			cout << endl;
			lastPrintTime = currentTime;
			lastCost = cost;
		}
#endif
	}

	cout << "GAME OVER.";

	getchar();

	return 0;
}

