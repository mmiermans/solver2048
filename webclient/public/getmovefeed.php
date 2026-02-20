<?php
require_once(realpath(dirname(__FILE__) . "/../resources/config.php"));

// Rate limiting: max 100 requests per minute per IP
$rate_limit_dir = "/tmp/rate_limits";
if (!is_dir($rate_limit_dir)) {
  mkdir($rate_limit_dir);
}

$client_ip = $_SERVER["HTTP_X_FORWARDED_FOR"] ?? $_SERVER["REMOTE_ADDR"];
$rate_file = $rate_limit_dir . "/" . $client_ip . ".json";

if (file_exists($rate_file)) {
  $rate_data = json_decode(file_get_contents($rate_file));
  $elapsed = time() - $rate_data->window_start;

  if ($elapsed < 60 && $rate_data->count >= 100) {
    header('Content-Type: application/json');
    http_response_code(429);
    echo json_encode(array("error" => "Rate limit exceeded", "retry_after" => 60 - $elapsed));
    exit();
  }

  if ($elapsed >= 60) {
    $rate_data->window_start = time();
    $rate_data->count = 0;
  }
  $rate_data->count++;
} else {
  $rate_data = (object)array("window_start" => time(), "count" => 1);
}

file_put_contents($rate_file, json_encode($rate_data));

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
