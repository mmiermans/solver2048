<?php

$mysqli = new mysqli('localhost','solver2048_admin','S2mfvqNYp77jbN9J','solver2048');
if ($mysqli->connect_errno) {
    echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
}

if (isset($_GET["movecount"])) {
	$move_count = $_GET["movecount"];
} else {
	$move_count = 0;
}

if (isset($_GET["gameid"])) {
	$game_id = $_GET["gameid"];
} else {
	// Parameters are not provided, get the newest game from the database.
	$game_id = 0;

	$res = $mysqli->query("SELECT MAX(id) FROM games");
	$row = $res->fetch_row();
	if ($row) {
		$game_id = $row[0];
	}
}

// Prepare statement
$stmt = $mysqli->prepare("SELECT `id`, `game_id`, Cast(`board_before_move` as char), `move_direction`, `move_count`, `new_tile_value`, `new_tile_position` FROM moves WHERE game_id=(?) AND move_count>=(?)");

// Bind parameters
$stmt->bind_param('ii', $game_id, $move_count);

// Execute statement
$stmt->execute();

// Bind result columns
$stmt->bind_result(
	$col_id,
	$col_game_id,
	$col_board_before_move,
	$col_move_direction,
	$col_move_count,
	$col_new_tile_value,
	$col_new_tile_position);

// Fetch values
$data = array();
while ($stmt->fetch()) {
	$m = array();
	$m['id'] = $col_id;
	$m['game_id'] = $col_game_id;
	$m['board_before_move'] = $col_board_before_move;
	$m['move_direction'] = $col_move_direction;
	$m['move_count'] = $col_move_count;
	$m['tile_value'] = $col_new_tile_value;
	$m['tile_position'] = $col_new_tile_position;
		
	array_push($data, $m); 
}

$stmt->close();

header('Content-Type: application/json');
echo json_encode($data);

?>
