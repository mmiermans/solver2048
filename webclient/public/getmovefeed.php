<?php
require_once(realpath(dirname(__FILE__) . "/../resources/config.php"));

if (isset($_GET["movecount"])) {
  $move_count = (int)$_GET["movecount"];
}

if (isset($_GET["gameid"])) {
  $game_id = (int)$_GET["gameid"];
}

if (isset($_GET["stats"]) && $_GET["stats"]) {
  $fetchGameStats = true;
}

// JSON content type enabled by default
if (!isset($enableJsonContentType) || $enableJsonContentType) {
  header('Content-Type: application/json');
}

require('movefeed.php');
