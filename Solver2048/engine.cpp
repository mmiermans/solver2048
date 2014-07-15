#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <limits>
#include <float.h>
#include <math.h>

#include "engine.h"
#include "searchnode.h"

#define MAX_LOOK_AHEAD 16
#define MAX_SCORE (FLT_MAX)

Engine::Engine()
	: scoreMap(1<<20)
{
	fastRng = new fastrand;
	BitMath::initRng(fastRng);

	scoreMap.set_empty_key(-1);
	//scoreMap.max_load_factor(0.25);

	nodes = new SearchNode[MAX_LOOK_AHEAD];

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

Move Engine::solve(Board board) {
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

	int tileSum = BoardLogic::sumTiles(board);

	unsigned char validMoves = BoardLogic::getValidMoves(board);

	for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
		Move move = (Move)(1 << moveIndex);
		if ((validMoves & move) != 0) {
			int t = tileSum / 768;
			dfsLookAhead = 2;
			if (move != Move::Down) {
				while (t > 0 && dfsLookAhead < 5) {
					dfsLookAhead++;
					t /= 2;
				}
			}
			if (dfsLookAhead > MAX_LOOK_AHEAD) {
				dfsLookAhead = MAX_LOOK_AHEAD;
			}


			Board movedBoard = BoardLogic::performMove(board, move);
			float score = depthFirstSolve(0, movedBoard, 0);
			if (bestScore > score) {
				bestScore = score;
				bestMoveIndex = moveIndex;
			}
		}
	}

	moveCounter[bestMoveIndex]++;

	clock_t endTime = clock();
	cpuTime += (endTime - startTime);

	return (Move)(1 << bestMoveIndex);
}

float Engine::depthFirstSolve(int index, Board b, float scoreSum) {
	float bScore = (float)evaluateBoard(b);
	if (BoardLogic::hasEmptyTile(b) == false) {
		return bScore;
	}

	float scores[BOARD_SIZE_SQ][NEW_VALUE_COUNT];
	for (int i = 0; i < BOARD_SIZE_SQ; i++) {
		for (int j = 0; j < NEW_VALUE_COUNT; j++) {
			scores[i][j] = MAX_SCORE;
		}
	}

	SearchNode& node = nodes[index];
	node.generateChildren(b);
	float childScoreSum = scoreSum + bScore;

	// Divide the 4 random numbers into 8 parts.
	uint16_t* res16 = (uint16_t*)(fastRng->res);

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
						score = depthFirstSolve(index + 1, childNode.board, childScoreSum);

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
						assert(position < BOARD_SIZE_SQ);
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
	int score;

	// Subtract penalty for 'game over' board.
	if (BoardLogic::hasEmptyTile(b) == false) {
		score = 1 << 15;
	} else {
		score = 0;
	}

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
		score += tile * (1 << tile);
		b >>= TILE_BITS;
	}

	return score;
}

void Engine::setRandomTile(Board& board) {
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
	// 0xe6666666 = 0.9 * 2^32.
	Tile randomTileValue = (fastRng->res[1] < PROBABILITY_2) ? 1 : 2;
	BoardLogic::setTile(board, randomEmptyTilePosition, randomTileValue);
}