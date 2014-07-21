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
		finish_with_error(con);
	}
}

MySqlConnector::~MySqlConnector() {
	// TODO Auto-generated destructor stub
}

void MySqlConnector::finish_with_error(MYSQL *con) {
	string error(mysql_error(con));
	mysql_close(con);
	throw std::runtime_error(error);
}

void MySqlConnector::startGame(Board& board, int& moveCount) {
	MYSQL_RES* result;
	MYSQL_ROW row;

	if (mysql_query(con, "SELECT id,board,move_count FROM games WHERE has_ended=0")) {
		finish_with_error(con);
	}

	result = mysql_store_result(con);
	if (result == NULL) {
		finish_with_error(con);
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
			finish_with_error(con);
		}

		gameId = mysql_insert_id(con);
		board = 0;
		moveCount = 0;
	}
}

void MySqlConnector::insertMove(Board before,
		Move::MoveEnum move,
		int newTileValue,
		int newTilePosition,
		bool hasEnded) {

}

void MySqlConnector::flush() {

}

#endif
