/*
 * mysqlconnector.h
 *
 *  Created on: Jul 21, 2014
 *      Author: mathijs
 */

// if ENABLE_MYSQL is defined then the database will be updated.
#ifdef ENABLE_MYSQL

#ifndef MYSQLCONNECTOR_H_
#define MYSQLCONNECTOR_H_

#include <string>
#include <map>

#if !defined(max)
#define max max
#define min min
#endif
#include <my_global.h>
#include <mysql.h>

#include "board.h"

class MySqlConnector {
public:
	MySqlConnector();
	virtual ~MySqlConnector();

	void startGame(Board& board, int& moveCount);

	void insertMove(
			Board before,
			Board after,
			Move::MoveEnum move,
			int newTilePosition,
			int newTileValue,
			int score,
			int maxTile,
			bool hasEnded);

	void flush();

private:
	MYSQL* con;

	int gameId;
	int moveCount;

	std::string movesBuffer;
	int movesBufferCount;
	clock_t movesBufferFlushTime;
	const int movesBufferCountTrigger = 512;
	const clock_t movesBufferTimeTrigger = 4;

	void updateGame(int score, int maxTile, bool hasEnded, Board board);

	std::string getSqlMoveEnum(Move::MoveEnum move);

	void flushInsertMovesBuffer();

	void throwMySqlException();
};

#endif /* MYSQLCONNECTOR_H_ */

#endif /* ENABLE_MYSQL */
