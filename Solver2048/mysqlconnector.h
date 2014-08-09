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

	void startGame(Board& board, int& moveCount, int& score);

	void insertMove(
			Board boardBeforeMove,
			Board boardAfterMove,
			Move::MoveEnum move,
			int newTilePosition,
			int newTileValue,
			int scoreBeforeMove,
			int scoreAfterMove,
			int maxTile,
			bool hasEnded);

	void flush();

private:
	MYSQL* con;

	int gameId;
	int moveCount;

	int gameScore;
	int gameMaxTile;
	bool gameHasEnded;
	Board gameBoardAfter;

	std::string queryBuffer;
	int queryBufferCount;
	clock_t queryBufferFlushTime;
	static const int queryBufferCountTrigger = 512;
	static const clock_t queryBufferTimeTrigger = 4*CLOCKS_PER_SEC;

	const std::string getUpdateGameQuery(int score, int maxTile, bool hasEnded, Board board);

	std::string getSqlMoveEnum(Move::MoveEnum move);

	void flushQueryBuffer();

	void throwMySqlException();
};

#endif /* MYSQLCONNECTOR_H_ */

#endif /* ENABLE_MYSQL */
