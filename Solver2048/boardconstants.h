#pragma once

typedef uint64_t Board;
typedef uint64_t Tile;

#define BOARD_SIZE 4
#define BOARD_SIZE_SQ (BOARD_SIZE*BOARD_SIZE)
#define BOARD_LOG_SIZE 2
#define BOARD_MASK (~0)

#define TILE_BITS 4
#define TILE_MAX ((1<<TILE_BITS)-1)
#define TILE_MASK ((Tile)0xF)

#define ROW_BITS (BOARD_SIZE*TILE_BITS)

#define BOARD_BITS (BOARD_SIZE_SQ*TILE_BITS)

#define MIN_NEW_VALUE 1
#define MAX_NEW_VALUE 2
#define NEW_VALUE_COUNT (MAX_NEW_VALUE-MIN_NEW_VALUE+1)

#if BOARD_SIZE == 4
	#define MASK_COL_FIRST     0x000F000F000F000FULL
	#define MASK_COL_NOT_FIRST 0xFFF0FFF0FFF0FFF0ULL
	#define MASK_COL_LAST      0xF000F000F000F000ULL
	#define MASK_COL_NOT_LAST  0x0FFF0FFF0FFF0FFFULL
	#define MASK_COL_SECOND    0x00F000F000F000F0ULL
	#define MASK_COL_01        0x00FF00FF00FF00FFULL
	#define MASK_COL_23        0xFF00FF00FF00FF00ULL
	#define MASK_COL_EVEN      0x0F0F0F0F0F0F0F0FULL
	#define MASK_COL_ODD       0xF0F0F0F0F0F0F0F0ULL
	#define MASK_ROW_FIRST     0x000000000000FFFFULL
	#define MASK_ROW_SECOND    0x00000000FFFF0000ULL
	#define MASK_ROW_LAST      0xFFFF000000000000ULL
	#define MASK_TILES_LSB     0x1111111111111111ULL
	#define MASK_TILES_MSB     0x8888888888888888ULL
#else
	#define MASK_COL_FIRST     firstColumnMask()
	#define MASK_COL_NOT_FIRST notFirstColumnMask()
	#define MASK_COL_LAST      lastColumnMask()
	#define MASK_COL_NOT_LAST  notLastColumnMask()
	#define MASK_COL_2         secondColumnMask()
	#define MASK_COL_EVEN      evenColumnMask()
	#define MASK_COL_ODD       oddColumnMask()
	#define MASK_ROW_FIRST     firstRowMask()
	#define MASK_ROW_LAST      lastRowMask()
	#define MASK_TILES_LSB     unityMask()
	#define MASK_TILES_MSB     msbMask()

	static inline Board firstColumnMask() { Board m = 0; for (int i = 0; i < BOARD_SIZE; i++) m = (m << ROW_BITS) | TILE_MASK; return m; }
	static inline Board notFirstColumnMask() { return (~firstColumnMask()) & BOARD_MASK; }
	static inline Board lastColumnMask() { return (firstColumnMask() << (ROW_BITS - TILE_BITS) & BOARD_MASK); }
	static inline Board notLastColumnMask() { return (~lastColumnMask()) & BOARD_MASK; }
	static inline Board secondColumnMask() { return (firstColumnMask() << TILE_BITS) & BOARD_MASK; }
	static inline Board evenColumnMask() { Board m = 0; for (int i = 0; i < BOARD_SIZE_SQ / 2; i++) m = TILE_MASK | (m << (2 * TILE_BITS)); return m; }
	static inline Board oddColumnMask() { return (~evenColumnMask()) & BOARD_MASK; }
	static inline Board firstRowMask() { return ROW_MASK; }
	static inline Board lastRowMask() { return (ROW_MASK << (BOARD_BITS - ROW_BITS)) & BOARD_MASK; }
	static inline Board unityMask() { Board m = 0; for (int i = 0; i < BOARD_SIZE_SQ; i++) m = 1 | (m << TILE_BITS); return m; }
	static inline Board msbMask() { return unityMask() << (TILE_BITS - 1); }
#endif

#define ROW_MASK           MASK_ROW_FIRST
