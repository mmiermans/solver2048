<?php
require_once(realpath(dirname(__FILE__) . "/../resources/config.php"));

if (isset($_GET["movecount"])) {
	$move_count = (int)$_GET["movecount"];
}

if (isset($_GET["gameid"])) {
	$game_id = (int)$_GET["gameid"];
}

header('Content-Type: application/json');
require('movefeed.php');
