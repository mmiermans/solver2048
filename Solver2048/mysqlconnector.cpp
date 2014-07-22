/*
 * mysqlconnector.cpp
 *
 *  Created on: Jul 21, 2014
 *      Author: mathijs
 */

#define ENABLE_MYSQL
#ifdef ENABLE_MYSQL

#include <iostream>
#include <stdexcept>
#include <string>
#include <time.h>

#include "mysqlconnector.h"

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
			con,                 // MYSQL structure
			"localhost",         // host
			"solver2048_admin",  // username
			"S2mfvqNYp77jbN9J",  // password
			"solver2048",        // database
			0,                   // port
			NULL,                // unix_socket
			0);                  // client_flag

	if (ret == NULL) {
		throwMySqlException();
	}

	movesBuffer.reserve(64 * 1024);
	movesBufferCount = 0;

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

void MySqlConnector::startGame(Board& board, int& moveCount) {
	MYSQL_RES* result;
	MYSQL_ROW row;

	if (mysql_query(con, "SELECT id,board,move_count FROM games WHERE has_ended=0")) {
		throwMySqlException();
	}

	result = mysql_store_result(con);
	if (result == NULL) {
		throwMySqlException();
	}

	row = mysql_fetch_row(result);
	if (row != NULL) {
		// There is already a game in progress: resume this game.
		gameId = std::stoi(row[0]);
		board = std::stoull(row[1]);
		moveCount = std::stoi(row[2]);
	} else {
		// Start a new game.
		if (mysql_query(con, "INSERT INTO games (start_date, end_date) VALUES(NULL, NULL)")) {
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
		Board before,
		Board after,
		Move::MoveEnum move,
		int newTilePosition,
		int newTileValue,
		int score,
		int maxTile,
		bool hasEnded) {
	// Add to query buffer.
	moveCount++;

	string values = ((movesBufferCount > 0) ? "," : "");
	values += "\n(" + std::to_string(gameId) +
			"," + std::to_string(before) +
			",'" + getSqlMoveEnum(move) + "'" +
			"," + std::to_string(moveCount) +
			"," + std::to_string(newTileValue) +
			"," + std::to_string(newTilePosition) +
			")";
	movesBuffer += values;
	movesBufferCount++;

	// Flush buffer if triggers are activated.
	bool isLengthTriggered = (movesBufferCount >= movesBufferCountTrigger);
	clock_t currentTime = clock();
	bool isTimeTriggered = ((currentTime - movesBufferFlushTime) >= (movesBufferTimeTrigger * CLOCKS_PER_SEC));
	if (isLengthTriggered || isTimeTriggered || hasEnded) {
		flushInsertMovesBuffer();
		updateGame(score, maxTile, hasEnded, after);
	}
}

void MySqlConnector::updateGame(int score, int maxTile, bool hasEnded, Board board) {
	string query = "UPDATE games SET score=" + to_string(score) +
			",max_tile=" + to_string(maxTile) +
			",has_ended=" + to_string(hasEnded ? 1 : 0) +
			",board=" + to_string(board) +
			",move_count=" + to_string(moveCount) +
			" WHERE id=" + to_string(gameId);

	if (mysql_query(con, query.c_str())) {
		throwMySqlException();
	}
}

void MySqlConnector::flush() {
	flushInsertMovesBuffer();
}

void MySqlConnector::flushInsertMovesBuffer() {
	if (movesBufferCount > 0) {
		if (mysql_query(con, movesBuffer.c_str())) {
			throwMySqlException();
		}
	}

	movesBufferCount = 0;
	movesBuffer.clear();
	movesBuffer += "INSERT INTO moves ("
			"game_id, board_before_move, move_direction, move_count, "
			"new_tile_value, new_tile_position) VALUES";

	movesBufferFlushTime = clock();
}

std::string MySqlConnector::getSqlMoveEnum(Move::MoveEnum move) {
	switch (move) {
	case Move::Left:
		return "left";
	case Move::Right:
		return "right";
	case Move::Up:
		return "up";
	case Move::Down:
		return "down";
	default:
		return "";
	}
}

#endif
