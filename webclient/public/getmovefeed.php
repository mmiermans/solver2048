<?php
require_once(realpath(dirname(__FILE__) . "/../resources/config.php"));

if (isset($_GET["limit"])) {
	$limit = $_GET["limit"];
}

if (isset($_GET["movecount"])) {
	$move_count = $_GET["movecount"];
}

if (isset($_GET["gameid"])) {
	$game_id = $_GET["gameid"];
}

header('Content-Type: application/json');
require('movefeed.php');
