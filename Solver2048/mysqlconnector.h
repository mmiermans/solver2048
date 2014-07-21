/*
 * mysqlconnector.h
 *
 *  Created on: Jul 21, 2014
 *      Author: mathijs
 */

// if ENABLE_MYSQL is defined then the database will be updated.
#define ENABLE_MYSQL
#ifdef ENABLE_MYSQL

#ifndef MYSQLCONNECTOR_H_
#define MYSQLCONNECTOR_H_

#include <string>

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

	void insertMove(Board before, Move::MoveEnum move, int newTileValue, int newTilePosition, bool hasEnded);

	void flush();

private:
	MYSQL* con;

	int gameId;
	int moveCount;

	std::string moveBuffer;

	void finish_with_error(MYSQL *con);
};

#endif /* MYSQLCONNECTOR_H_ */

#endif /* ENABLE_MYSQL */
