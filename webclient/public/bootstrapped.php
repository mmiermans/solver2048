<?php
require_once(realpath(dirname(__FILE__) . "/../resources/config.php"));

$fetchGameStats = true;

include 'movefeed.php';

unset($fetchGameStats);
