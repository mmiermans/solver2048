/*
 * mysqlconnector.cpp
 *
 *  Created on: Jul 21, 2014
 *      Author: mathijs
 */

#ifdef ENABLE_MYSQL

#include <iostream>
#include <stdexcept>
#include <string>
#include <time.h>

#include "mysqlconnector.h"
#include "convert.h"

#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY(s)

#ifndef MYSQL_HOSTNAME
#define MYSQL_HOSTNAME localhost
#endif

#ifndef MYSQL_PORT
#define MYSQL_PORT 0
#endif

#ifndef MYSQL_DATABASE
#define MYSQL_DATABASE solver2048
#endif

#ifndef MYSQL_USERNAME
#define MYSQL_USERNAME solver2048_user
#endif

#ifndef MYSQL_PASSWORD
#define MYSQL_PASSWORD "abc123"
#endif

using namespace std;

MySqlConnector::MySqlConnector() {
	gameId = -1;
	moveCount = -1;

	con = mysql_init(
	NULL);
	if (con == NULL) {
		throw std::runtime_error("mysql_init() failed");
	}

	// Connect to database
	MYSQL* ret = mysql_real_connect(
			con,                        // MYSQL structure
			STRINGIFY(MYSQL_HOSTNAME),  // host
			STRINGIFY(MYSQL_USERNAME),  // username
			STRINGIFY(MYSQL_PASSWORD),  // password
			STRINGIFY(MYSQL_DATABASE),  // database
			MYSQL_PORT,                 // port
			NULL,                       // unix_socket
			CLIENT_MULTI_STATEMENTS);   // client_flag

	if (ret == NULL) {
		throwMySqlException();
	}

	queryBuffer.reserve(64 * 1024);
	queryBufferCount = 0;

	flush();
}

MySqlConnector::~MySqlConnector() {
	mysql_close(con);
}

void MySqlConnector::throwMySqlException() {
	string error(mysql_error(con));
	mysql_close(con);
	throw std::runtime_error(error);
}

void MySqlConnector::startGame(Board& board, int& moveCount, int& score) {
	MYSQL_RES* result;
	MYSQL_ROW row;
	mysql_set_server_option(con, MYSQL_OPTION_MULTI_STATEMENTS_OFF);

	const char* selectGameQuery =
		"SELECT id, board, move_count, score"
		" FROM games"
		" WHERE has_ended=0";
	if (mysql_query(con, selectGameQuery)) {
		throwMySqlException();
	}

	result = mysql_store_result(con);
	if (result == NULL) {
		throwMySqlException();
	}

	row = mysql_fetch_row(result);
	if (row != NULL) {
		// There is already a game in progress: resume this game.
		gameId = Convert<int>::fromString(row[0]);
		board = Convert<Board>::fromString(row[1]);
		moveCount = Convert<int>::fromString(row[2]);
		score = Convert<int>::fromString(row[3]);
	} else {
		// Start a new game.
		const char* createGameQuery = "INSERT INTO games (start_date, end_date) VALUES(NULL, NULL)";
		if (mysql_query(con, createGameQuery)) {
			throwMySqlException();
		}

		gameId = mysql_insert_id(con);
		board = 0;
		moveCount = 0;
	}

	// Also update internal move counter.
	this->moveCount = moveCount;
}

void MySqlConnector::insertMove(
		Board boardBeforeMove,
		Board boardAfterMove,
		Move::MoveEnum move,
		int newTilePosition,
		int newTileValue,
		int scoreBeforeMove,
		int scoreAfterMove,
		int maxTile,
		bool hasEnded) {
	// Add to query buffer.
	moveCount++;

	gameScore = scoreAfterMove;
	gameMaxTile = maxTile;
	gameHasEnded = hasEnded;
	gameBoardAfter = boardAfterMove;

	string values;
	values.reserve(128);
	if (queryBufferCount > 0)
		values += ",";
	values += "\n(" + Convert<int>::toString(gameId);
	values += "," + Convert<Board>::toString(boardBeforeMove);
	values += "," + Convert<int>::toString(scoreBeforeMove);
	values += "," + Convert<int>::toString((int)move);
	values += "," + Convert<int>::toString(moveCount);
	values += "," + Convert<int>::toString(newTileValue);
	values += "," + Convert<int>::toString(newTilePosition);
	values += ")";
	queryBuffer += values;
	queryBufferCount++;

	clock_t currentTime = clock();
	bool isLengthTriggered = (queryBufferCount >= queryBufferCountTrigger);
	bool isTimeTriggered = ((currentTime - queryBufferFlushTime) >= queryBufferTimeTrigger);
	// Flush buffer if triggers are activated.
	if (isLengthTriggered || isTimeTriggered || hasEnded) {
		// Send the query to MySQL
		flush();
	}
}

const string MySqlConnector::getUpdateGameQuery(int score, int maxTile,
		bool hasEnded, Board board) {
	string query;
	query.reserve(128);
	query += "UPDATE games SET score=" + Convert<int>::toString(score);
	query += ",max_tile=" + Convert<int>::toString(maxTile);
	query += ",has_ended=" + Convert<int>::toString(hasEnded ? 1 : 0);
	query += ",board=" + Convert<Board>::toString(board);
	query += ",move_count=" + Convert<int>::toString(moveCount);
	query += " WHERE id=" + Convert<int>::toString(gameId);
	return query;
}

void MySqlConnector::flush() {
	queryBuffer += ";\n";
	queryBuffer += getUpdateGameQuery(gameScore, gameMaxTile, gameHasEnded, gameBoardAfter);

	flushQueryBuffer();
}

void MySqlConnector::flushQueryBuffer() {
	if (queryBufferCount > 0) {
		// Perform a multi result query, to ensure that all queries either
		// get executed or not.
		mysql_set_server_option(con, MYSQL_OPTION_MULTI_STATEMENTS_ON);
		int status = mysql_query(con, queryBuffer.c_str());
		if (status) {
			throwMySqlException();
		}

		// process each statement result, as required by MySQL
		do {
			MYSQL_RES* result = mysql_store_result(con);
			if (result) {
				mysql_free_result(result);
			}
		} while (mysql_next_result(con) == 0);
	}

	queryBufferCount = 0;
	queryBuffer.clear();
	queryBuffer += "INSERT INTO moves ("
			"game_id, board_before_move, score_before_move, move_direction, move_count, "
			"new_tile_value, new_tile_position) VALUES";

	queryBufferFlushTime = clock();
}

#endif
