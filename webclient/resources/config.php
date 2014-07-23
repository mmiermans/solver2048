<?php

$config = array(
	"db" => array(
		"dbname" =>   "solver2048",
		"username" => "solver2048_user",
		"password" => "abc123",
		"host" =>     "localhost"
	)
);

defined("LIB_PATH")
    or define("LIB_PATH", realpath(dirname(__FILE__) . '/lib'));

set_include_path(get_include_path() . PATH_SEPARATOR . LIB_PATH);
 
ini_set("error_reporting", "true");
error_reporting(E_ALL|E_STRCT);
