#include "searchnode.h"
#include <limits.h>
#include <string.h>

SearchNode::SearchNode() {
}

void SearchNode::generateChildren(Board b) {
	// Clear child nodes.
	memset(&childCount, 0, sizeof(childCount));

	Board emptyMask = BoardLogic::getEmptyMask(b);
	emptyTileCount = BitMath::popCount(emptyMask) / TILE_BITS;
	if (emptyTileCount == 0)
		return;

	// addLR is a mask of where tiles should be inserted to generate unique boards.
	Board add[2][2];
	unsigned char emptyCounts[2][2 * BOARD_SIZE];
	int8_t positions[2][2 * BOARD_SIZE][4];
	int8_t positionFirst[2][2 * BOARD_SIZE];
	int8_t positionLast[2][2 * BOARD_SIZE];
	memset(add, 0, sizeof(add));
	memset(positions, -1, sizeof(positions));
	memset(emptyCounts, 0, sizeof(emptyCounts));
	// Group adjacent empty tiles together. Keep track of which positions
	//  belong to a sequence and the length of the sequence (emptyCounts).
	// First loop through horizontal (d=0) and vertical (d=1) dimensions.
	for (int d = 0; d < 2; d++) {
		int posStep;
		int posStepI;
		if (d == 0) {
			posStep = 1;
			posStepI = BOARD_SIZE;
		} else {
			posStep = BOARD_SIZE;
			posStepI = 1;
		}

		// Loop through primary coordinate.
		for (int i = 0; i < BOARD_SIZE; i++) {
			int sequenceCount = 0;
			unsigned char* emptyCount;
			int8_t* position;
			bool isNewSequence = true;

			// Loop through secondary coordinate.
			int posStart = i * posStepI;
			int posEnd = posStart + BOARD_SIZE*posStep;
			int seqIndex;
			for (int pos = posStart; pos < posEnd; pos += posStep) {
				// Test whether the board is empty at pos.
				if ((emptyMask & (TILE_MASK << (pos * TILE_BITS))) != 0) {
					if (isNewSequence) {
						// Initialize variables for new sequence.
						add[d][sequenceCount] |= TILE_MASK << (pos*TILE_BITS);
						seqIndex = (2 * i) + sequenceCount;
						position = &(positions[d][seqIndex][0]);
						positionFirst[d][seqIndex] = pos;
						emptyCount = &(emptyCounts[d][seqIndex]);
						sequenceCount++;
						isNewSequence = false;
					}
					// Extend sequence.
					(*emptyCount)++;
					*position = pos;
					positionLast[d][seqIndex] = pos;
					position++;
				} else {
					// A new sequence will start at the next empty tile.
					isNewSequence = true;
				}
			}
		}
	}

	// Perform moves and generate child nodes.
	for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
		Move move = (Move)(1 << moveIndex);

		Board mask; // Mask of the current row/column.
		int maskShift; // How much to shift the current row column.
		int d; // dimension index (horizontal = 0, vertical = 1).
		// This position should be checked to determine whether move is valid.
		int8_t* checkPosition;

		if (move & (Move::Left | Move::Right)) {
			d = 0;
			mask = MASK_ROW_FIRST;
			maskShift = ROW_BITS;
			if ((move & Move::Left) != 0) {
				checkPosition = positionFirst[0];
			} else {
				checkPosition = positionLast[0];
			}
		} else {
			d = 1;
			mask = MASK_COL_FIRST;
			maskShift = TILE_BITS;
			if ((move & Move::Up) != 0) {
				checkPosition = positionFirst[1];
			} else {
				checkPosition = positionLast[1];
			}
		}

		// Move the board with the first and second tile (if any exists).
		// Multiple rows/columns in which a tile is inserted are combined in
		//  one or two calls to performMove() to increase performance.
		// Indicies: [primary/secondary sequence][tile value]
		Board addAndMove[2][NEW_VALUE_COUNT];
		for (int i = 0; i < 2; i++) {
			Board addMask = add[d][i];
			if (addMask) {
				for (int v = 0; v < NEW_VALUE_COUNT; v++) {
					Board a = b | (((v + MIN_NEW_VALUE) * MASK_TILES_LSB) & addMask);
					addAndMove[i][v] = BoardLogic::performMove(a, move);
				}
			}
		}

		// Move the board without any added tiles.
		Board baseBoard = BoardLogic::performMove(b, move);
		Board baseDiff = b ^ baseBoard;

		// Left and right move: loop through rows.
		// Up and down move: loop through columns.
		for (int i = 0; i < BOARD_SIZE; i++, mask <<= maskShift) {
			// Loop through primary/secondary added tile.
			for (int j = 0; j < 2 && (add[d][j] & mask) != 0; j++) {
				int8_t* posArr = positions[d][2 * i + j];
				// Loop through new tile values
				for (int v = 0; v < NEW_VALUE_COUNT; v++) {
					int8_t currentPositions[4] = { posArr[0], posArr[1], posArr[2], posArr[3]};
					// Test whether the performed move is valid.
					if ((baseDiff & ~mask) == 0) {
						int8_t p = checkPosition[2 * i + j];
						Board bWithCheckTile = (b | ((Board)(v + MIN_NEW_VALUE) << (p * TILE_BITS)));
						if (((bWithCheckTile ^ addAndMove[j][v]) & mask) == 0) {
							// Nothing changed. If this was the only tile in this sequence,
							//  then continue, otherwise delete p from the list of positions.
							if (currentPositions[1] < 0) {
								continue;
							} else {
								currentPositions[3] = -1;
								for (int kSrc = 0, kDst = 0; kSrc < 4; kSrc++) {
									if (currentPositions[kSrc] != p) {
										currentPositions[kDst] = currentPositions[kSrc];
										kDst++;
									}
								}
							}
						}
					}

					int count = childCount[moveIndex][v]++;
					ChildNode& childNode = children[moveIndex][count][v];
					// Combine two boards, one with an inserted tiles and one without.
					childNode.board = (baseBoard & ~mask) | (mask & addAndMove[j][v]);
					// Copy all four positions in one go.
					childNode.positions[0] = currentPositions[0];
					childNode.positions[1] = currentPositions[1];
					childNode.positions[2] = currentPositions[2];
					childNode.positions[3] = currentPositions[3];
				}
			}
		}
	}

	// Handle 'game over' cases. These do not have any valid moves, so therefore
	// the above loop does not create any child nodes. Remedy this by creating an
	// (arbitrary) left move node.
	if (emptyTileCount == 1) {
		for (int v = 0; v < NEW_VALUE_COUNT; v++) {
			if ((childCount[0][v] + childCount[1][v] + childCount[2][v] + childCount[3][v]) == 0) {
				childCount[0][v] = 1;
				for (int i = 0; i < BOARD_SIZE; i++) {
					if (positions[0][2 * i][0] >= 0) {
						ChildNode& childNode = children[0][0][v];
						childNode.board = b | (((v + MIN_NEW_VALUE) * MASK_TILES_LSB) & emptyMask);
						childNode.positions[0] = positions[0][2 * i][0];
						childNode.positions[1] = -1;
						break;
					}
				}
			}
		}
	}
}

