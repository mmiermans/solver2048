#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <limits>
#include <float.h>
#include <math.h>

#include "engine.h"
#include "searchnode.h"

#define MAX_LOOK_AHEAD 11
#define MAX_SCORE (FLT_MAX)

Engine::Engine()
#ifdef GOOGLE_HASHING
	: scoreMap(1<<20)
#endif
{
	hashHits = 0;
	hashMisses = 0;
	nodeCounter = 0;
	cpuTime = 0;

	fastRng = new fastrand;
	BitMath::initRng(fastRng);

#ifdef GOOGLE_HASHING
	scoreMap.set_empty_key(-1);
#endif

	nodes = new SearchNode[MAX_LOOK_AHEAD];
	dfsLookAhead = 2;

	moveCounter[0] = 0;
	moveCounter[1] = 0;
	moveCounter[2] = 0;
	moveCounter[3] = 0;
}

Engine::~Engine()
{
	delete fastRng;
	delete[] nodes;
}

Move::MoveEnum Engine::solve(Board board) {
	clock_t startTime = clock();

#ifdef ENABLE_HASHING
#ifdef CUSTOM_HASHING
	boardHashTable.clear();
#else
	scoreMap.clear_no_resize();
#endif
//	reverseHashTable.clear();
#endif
	hashHits = 0;
	hashMisses = 0;

	float bestScore = MAX_SCORE;
	int bestMoveIndex = 0;

	// Based on the highest tile after the increasing sequence.
	int maxBadTile = maxTileAfterSequence(board);
	int baseLookAhead = (int)fmax(3, maxBadTile - 2);

	// Based on empty tiles after moves.
	int maxEmptyTileCount = 0;
	for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
		Move::MoveEnum move = (Move::MoveEnum)(1 << moveIndex);
		Board movedBoard = BoardLogic::performMove(board, move);
		int emptyTileCount = BoardLogic::getEmptyCount(movedBoard);
		if (emptyTileCount > maxEmptyTileCount) {
			maxEmptyTileCount = emptyTileCount;
		}
	}
	if (maxEmptyTileCount <= 2 && BoardLogic::getTile(board, 0, 0) >= 13) {
		baseLookAhead = (int)fmax(baseLookAhead, 11 - maxEmptyTileCount);
	}

	if (BoardLogic::getTile(board, 0, 0) == 0) {
		baseLookAhead = 1;
	}

	int tileSum = BoardLogic::sumTiles(board);
	if (tileSum >= 30800) {
		baseLookAhead = (int)fmax(6, baseLookAhead);
	}

	// Increase lookahead in case of locked rows.
	Board selectedRows = MASK_ROW_FIRST;
	for (int i = 2; i < BOARD_SIZE; i++) {
		selectedRows <<= ROW_BITS;
		selectedRows |= ROW_MASK;
		unsigned char moves = BoardLogic::getValidMoves(board & selectedRows);
		bool hasEmptyTiles = (BoardLogic::hasEmptyTile(board | ~selectedRows) != 0);
		if ((moves & ~Move::Down) == 0 && hasEmptyTiles == false) {
			baseLookAhead += 2;
		}
	}

	// Maximum lookahead.
	if ((board & 0xFFF) == 0xCBA ||
		(board & 0xFFF) == 0xDCB ||
		(board & 0xFFF) == 0xEDC) {
		// A good board deserves a good look ahead.
		baseLookAhead = (int)fmin(10, baseLookAhead);
	} else {
		// This board is not good, restrict the look ahead.
		baseLookAhead = (int)fmin(8, baseLookAhead);
	}

	// Bind lookahead to valid range
	if (baseLookAhead > MAX_LOOK_AHEAD - 1) {
		baseLookAhead = MAX_LOOK_AHEAD - 1;
	} else if (baseLookAhead < 1) {
		baseLookAhead = 1;
	}
	//std::cout << baseLookAhead << std::endl;

	unsigned char validMoves = BoardLogic::getValidMoves(board);
	bool isSingleMove = false;
	for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
		Move::MoveEnum move = (Move::MoveEnum)(1 << moveIndex);
		if (validMoves == move) {
			isSingleMove = true;
			bestMoveIndex = moveIndex;
		}
	}

	if (isSingleMove == false) {
		for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
			Move::MoveEnum move = (Move::MoveEnum)(1 << moveIndex);
			if ((validMoves & move) != 0) {
				if (move == Move::Down) {
					dfsLookAhead = fmin(5, baseLookAhead);
				} else {
					dfsLookAhead = baseLookAhead;
				}

				Board movedBoard = BoardLogic::performMove(board, move);
				float score = depthFirstSolve(0, movedBoard);
				if (bestScore > score) {
					bestScore = score;
					bestMoveIndex = moveIndex;
				}
			}
		}
	}

	moveCounter[bestMoveIndex]++;

	clock_t endTime = clock();
	cpuTime += (endTime - startTime);
	dfsLookAhead = baseLookAhead;

	return (Move::MoveEnum)(1 << bestMoveIndex);
}

float Engine::depthFirstSolve(int index, Board b) {
	if (BoardLogic::hasEmptyTile(b) == false) {
		return (float)evaluateBoard(b);
	}

	float scores[BOARD_SIZE_SQ][NEW_VALUE_COUNT];
	for (int i = 0; i < BOARD_SIZE_SQ; i++) {
		for (int j = 0; j < NEW_VALUE_COUNT; j++) {
			scores[i][j] = MAX_SCORE;
		}
	}

	SearchNode& node = nodes[index];
	node.generateChildren(b);

#ifdef ENABLE_SAMPLING
	// Divide the 4 random numbers into 8 parts.
	uint16_t* res16 = (uint16_t*)(fastRng->res);
#endif

	for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
		for (int v = 0; v < NEW_VALUE_COUNT; v++) {
			int childCount = node.childCount[moveIndex][v];

#ifdef ENABLE_SAMPLING
			// Initialize sampling variables.
			int sampleCount = 8;
			int samples[MAX_SEARCH_NODE_CHILDREN];
			for (int i = 0; i < childCount; i++) {
				samples[i] = i;
			}
			if (sampleCount < childCount) {
				// Get random numbers
				FastRand_SSE(fastRng);
				for (int i = 0; i < sampleCount; i++) {
					// j is a random integer with i <= j < n.
					int j = ((res16[i] % (childCount - i)) + i);
					// exchange samples[j] and samples[i].
					int t = samples[i];
					samples[i] = samples[j];
					samples[j] = t;
				}
			} else {
				sampleCount = childCount;
			}

			for (int sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++) {
				// Get child node
				int childIndex = samples[sampleIndex];
#else
			for (int childIndex = 0; childIndex < childCount; childIndex++) {
#endif
				ChildNode& childNode = node.children[moveIndex][childIndex][v];

				// Get score from hashmap, by recursion or from evaluateBoard().
				float score;
				if (index >= dfsLookAhead - 1) {
					score = (float)evaluateBoard(childNode.board);
				} else {
#ifdef ENABLE_HASHING
#ifdef CUSTOM_HASHING
					score = boardHashTable.get(childNode.board);
					if (score != boardHashTable.nullValue) {
#else
					hash_t::const_iterator it = scoreMap.find(childNode.board);
					if (it != scoreMap.end()) {
						score = it->second;
#endif
					//hash_t::const_iterator it = scoreMap.find(childNode.board);
					//if (score != boardHashTable.nullValue) {
					//	if (it == scoreMap.end() || score != it->second) {
					//		std::cout << "Hash collision: {";
					//		Board h = BoardHashTable::hash(childNode.board) % boardHashTable.size;
					//		std::vector<Board> boards = reverseHashTable[h];
					//		for (int i = 0; i < boards.size(); i++) {
					//			std::cout << "16^^" << std::hex << std::setfill('0') << std::setw(16) << boards.at(i) << ",";
					//		}
					//		std::cout << "16^^" << std::hex << std::setfill('0') << std::setw(16) << childNode.board << "}" << std::endl;
					//	}
					//}

						hashHits++;
					} else {
						score = depthFirstSolve(index + 1, childNode.board);

#ifdef CUSTOM_HASHING
						boardHashTable.put(childNode.board, score);
#else
						scoreMap.insert(hash_t::value_type(childNode.board, score));
#endif

						//Board h = BoardHashTable::hash(childNode.board) % boardHashTable.size;
						//reverseHashTable[h].push_back(childNode.board);

						hashMisses++;
					}
#else
					score = depthFirstSolve(index + 1, childNode.board, childScoreSum);
#endif
				}

				// Update score matrix.
				for (int p = 0; p < 4 && childNode.positions[p] >= 0; p++) {
					int position = childNode.positions[p];
					if (position >= 0) {
						float& oldScore = scores[position][v];
						if (oldScore > score)
							oldScore = score;
						nodeCounter++;
					} else {
						break;
					}
				}
			}
		}
	}

	// Aggregate scores
	float result = 0;
	const float pValue[NEW_VALUE_COUNT] = { 0.9f, 0.1f };
	float totalWeight = 0;
	for (int p = 0; p < BOARD_SIZE_SQ; p++) {
		for (int v = 0; v < NEW_VALUE_COUNT; v++) {
			if (scores[p][v] != MAX_SCORE) {
				result += pValue[v] * scores[p][v];
				totalWeight += pValue[v];
			}
		}
	}

	result /= totalWeight;

	return result;
}

uint32_t Engine::evaluateBoard(Board b) {

	if (BoardLogic::hasValidMoves(b) == false) {
		return 1 << 14;
	}

	Board previousTile = b & TILE_MASK;
	// If first tile is empty, ignore first two tiles.
	if (previousTile == 0)
		b >>= TILE_BITS;

	// Flip odd rows
#if BOARD_SIZE == 4
	b = (b & 0x0000FFFF0000FFFF) |
		((b & 0x000F0000000F0000) << 12) |
		((b & 0x00F0000000F00000) << 4) |
		((b & 0x0F0000000F000000) >> 4) |
		((b & 0xF0000000F0000000) >> 12);
#else
	#error TODO: implement for case != 4
#endif

	uint32_t score = 0;
	// Find first tile that is lower than its previous tile.
	while (b != 0) {
		b >>= TILE_BITS;
		Board tile = b & TILE_MASK;

		if (tile > previousTile) {
			break;
		}

		previousTile = tile;
	}

	// Sum all subsequent tiles.
	while (b != 0) {
		int tile = (b & TILE_MASK);
		score += (1 << tile) - 1;
		b >>= TILE_BITS;
	}

	return score;
}

int Engine::maxTileAfterSequence(Board b) {
	int maxTile = 0;
	if (BoardLogic::hasValidMoves(b) == false)
		return 1;

	// Flip odd rows
#if BOARD_SIZE == 4
	b = (b & 0x0000FFFF0000FFFF) |
		((b & 0x000F0000000F0000) << 12) |
		((b & 0x00F0000000F00000) << 4) |
		((b & 0x0F0000000F000000) >> 4) |
		((b & 0xF0000000F0000000) >> 12);
#else
#error TODO: implement for case != 4
#endif

	// Find first tile that is lower than its previous tile.
	Board previousTile = b & TILE_MASK;
	while (b != 0) {
		b >>= TILE_BITS;
		Board tile = b & TILE_MASK;

		if (tile > previousTile) {
			break;
		}

		previousTile = tile;
	}

	// Sum all subsequent tiles.
	while (b != 0) {
		int tile = (b & TILE_MASK);
		if (tile > maxTile) maxTile = tile;
		b >>= TILE_BITS;
	}

	return maxTile;

}

void Engine::setRandomTile(Board& board) {
	int position = 0;
	Tile value = 0;
	getRandomTile(board, position, value);
	BoardLogic::setTile(board, position, value);
}

void Engine::getRandomTile(Board board, int& position, Tile& value) {
	// Get random numbers
	FastRand_SSE(fastRng);

	// Get random empty tile.
	Board emptyMask = BoardLogic::getEmptyMask(board);
	int emptyCount = BitMath::popCount(emptyMask) / TILE_BITS;
	int randomEmptyTileNumber = fastRng->res[0] % emptyCount;
	int randomEmptyTilePosition = 0;
	while (true) {
		if (emptyMask & (TILE_MASK << randomEmptyTilePosition)) {
			if (randomEmptyTileNumber-- == 0)
				break;
		}
		randomEmptyTilePosition += TILE_BITS;
	}

	// Get random tile value with P(2|0.9) and P(4|0.1).
	Tile randomTileValue = (fastRng->res[1] < PROBABILITY_2) ? 1 : 2;

	position = randomEmptyTilePosition;
	value = randomTileValue;
}
