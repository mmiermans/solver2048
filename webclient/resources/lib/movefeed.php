<?php

$mysqli = new mysqli(
  $config["db"]["host"],
  $config["db"]["username"],
  $config["db"]["password"],
  $config["db"]["dbname"]);

if (mysqli_connect_errno()) {
    printf("Connect failed: %s\n", mysqli_connect_error());
    exit();
}

// JSON structure
$data = array(
  "stats" => array(
    "max_tile" => array(),
    "score" => array(),
  ),
  "game" => array(),
  "moves" => array(),
);

// Fetch game statistics
if (isset($fetchGameStats) && $fetchGameStats) {
$maxTileQuery = <<<'EOD'
SELECT max_tile, COUNT(*) AS count
FROM games
WHERE games.has_ended = 1
GROUP BY max_tile
EOD;

  $result = $mysqli->query($maxTileQuery);
  if (!$result) {
    echo "CALL failed: (" . $mysqli->errno . ") " . $mysqli->error;
  }
  
  while ($row = $result->fetch_row()) {
    $x = array((int)$row[0], (int)$row[1]);
    array_push($data["stats"]["max_tile"], $x);
  }

  $result->close();

$scoreQuery = <<<'EOD'
SELECT score
FROM games
WHERE games.has_ended = 1
EOD;

  $result = $mysqli->query($scoreQuery);
  if (!$result) {
    echo "CALL failed: (" . $mysqli->errno . ") " . $mysqli->error;
  }
  
  while ($row = $result->fetch_row()) {
    array_push($data["stats"]["score"], (int)$row[0]);
  }

  $result->close();

}

// Fetch move feed
if (isset($move_count) && isset($game_id)) {
  $move_count = (int)$move_count;
  $game_id = (int)$game_id;
  $query = "CALL get_game_moves({$game_id}, {$move_count})";
} else {
  $query = "CALL get_newest_game_moves()";
}

if (!$mysqli->multi_query($query)) {
  echo "CALL failed: (" . $mysqli->errno . ") " . $mysqli->error;
}

// Fetch results
$resultNum = 0;
do {
  if ($res = $mysqli->store_result()) {
    $rows = $res->fetch_all(MYSQLI_NUM);

    if ($resultNum == 0) {
      // game
      $data["game"] = array(
        "id"        => (int)$rows[0][0],
        "has_ended" => (int)$rows[0][1]);
    } else if ($resultNum == 1) {
      // moves
      foreach ($rows as $row) {
        // Convert columns
        $move = array();
        array_push($move, ltrim($row[0], '0')); // board_before_move
        array_push($move, (int)$row[1]);        // score_before_move
        array_push($move, (int)$row[2]);        // move_direction
        array_push($move, (int)$row[3]);        // move_count
        array_push($move, (int)$row[4]);        // new_tile_value
        array_push($move, (int)$row[5]);        // new_tile_position
        array_push($data["moves"], $move);
      }
    }
    $resultNum++;

    $res->free();
  } else if ($mysqli->errno) {
    echo "store_result failed: (" . $mysqli->errno . ") " . $mysqli->error;
  }
} while ($mysqli->more_results() && $mysqli->next_result());

$mysqli->close();

echo json_encode($data);
