// Solver2048.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <iomanip>
#include <string>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "mysqlconnector.h"

#include "board.h"
#include "engine.h"
#include "searchnode.h"
#include "bitmath.h"

using namespace std;

void askTile(Board& board) {
	int x = 0;
	int y = 0;
	Tile tile = 0;
	cout << "Enter <row> <col> <val> (0 0 2 sets top-left to 2): ";
	cin >> x;
	cin >> y;
	cin >> tile;
	tile = (Tile) round(log2((double) tile));
	BoardLogic::setTile(board, x, y, tile);
}

void precomputeMoves() {
	for (int i = 0; i < 1 << 16; i++) {
		Board iBoard = (Board) i;
		Board result = BoardLogic::performMove(iBoard, Move::Right);

		if ((result & ~0xFFFF) != 0)
			result = 0xFFFF;

		cout << "0x" << std::hex << setw(4) << uppercase << setfill('0')
				<< result;
		if (i != (1 << 16) - 1)
			cout << ", ";
		if (i % 16 == 15)
			cout << " \\\\" << endl;
	}
}

int main(int argc, char* argv[]) {
	BitMath::sanityCheck();
#ifdef ENABLE_MYSQL
	MySqlConnector mySqlConnector;
#endif

	clock_t startTime = clock();
	clock_t lastPrintTime = startTime;

	// RESTART GAME LOOP
	while (true) {
		Engine e;
		Board b = 0;
		int moveCount = 0;
		int score = 0;

#ifdef ENABLE_MYSQL
		mySqlConnector.startGame(b, moveCount, score);
#endif
		int startMoveCount = moveCount;

		// Set two random tiles if this is a new game.
		if (b == 0) {
			e.setRandomTile(b);
			e.setRandomTile(b);
		}

		// Debug variables
		int lastMoveCount = moveCount;
		int lastCost = 0;
		const int printStep = CLOCKS_PER_SEC;
		int maxLookAhead = 0;
		clock_t currentTime;

		// Main game loop
		bool hasValidMove = true;
		while (hasValidMove) {

			// Let game engine perform evaluation on board b.
			Move::MoveEnum bestMove = e.solve(b);

			Board boardBeforeMove = b;
			moveCount++;

#ifdef ENABLE_STDOUT
			// Debug info
			if (e.dfsLookAhead > maxLookAhead)
				maxLookAhead = e.dfsLookAhead;
			int cost = e.evaluateBoard(b);
			currentTime = clock();
			if (cost - lastCost > 1024 ||
				hasValidMove == false ||
				moveCount == 1 ||
				currentTime - lastPrintTime >= printStep) {
				// Game statistics
				cout << "Moves: " << moveCount << "\t";
				cout << "Time: " << (currentTime - startTime) / CLOCKS_PER_SEC
					<< "s\t";
				cout << "Score: " << score;
				cout << endl;

				cout << "LookAhead: " << e.dfsLookAhead << "\t";
				cout << "Eval: " << e.costEst << endl;

				// Speed stats
				cout << "Moves/s: "
					<< (CLOCKS_PER_SEC * (moveCount - lastMoveCount))
					/ (float)(currentTime - lastPrintTime) << "\t";
				cout << "AvgMoves/s: "
					<< (CLOCKS_PER_SEC * (moveCount - startMoveCount))
					/ (float)(currentTime - startTime) << "\t";
				lastMoveCount = moveCount;
				int kNodesPerSec =
					e.cpuTime == 0 ?
					9999 :
					(int)((CLOCKS_PER_SEC * e.nodeCounter)
					/ (1000 * e.cpuTime));
				cout << "kNodes/s: " << kNodesPerSec << "\t";
				cout << endl;

				BoardLogic::printBoard(b);
				cout << endl;
				lastPrintTime = currentTime;
				lastCost = cost;
			}
#endif

			// Execute move.
			b = BoardLogic::performMove(b, bestMove);
			Board boardAfterMove = b;

			// Insert new random tile.
			int newTilePosition = 0;
			Tile newTileValue = 0;
			e.getRandomTile(b, newTilePosition, newTileValue);
			BoardLogic::setTile(b, newTilePosition, newTileValue);

			// Gameover?
			hasValidMove = (BoardLogic::getValidMoves(b) != (Move::MoveEnum) 0);

			int scoreIncrease = BoardLogic::calculateScoreIncrease(boardBeforeMove, boardAfterMove);
			score += scoreIncrease;

#ifdef ENABLE_MYSQL
			int maxTile = BoardLogic::maxTile(b);
			// Add move to MySQL database
			mySqlConnector.insertMove(
					boardBeforeMove,
					b,
					bestMove,
					newTilePosition / TILE_BITS,
					1 << newTileValue,
					score,
					maxTile,
					!hasValidMove);
#endif
		}

#ifdef ENABLE_MYSQL
		mySqlConnector.flush();
#endif
#ifdef ENABLE_STDOUT
		cout << endl << "GAME OVER." << endl;
		cout << "********************************************************************************" << endl << endl;
#endif
	}

	return 0;
}

