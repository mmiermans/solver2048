<?php
require_once(realpath(dirname(__FILE__) . "/../resources/config.php"));

// Rate limiting: max 100 requests per minute per IP
$rate_limit_dir = "/tmp/rate_limits";
if (!is_dir($rate_limit_dir)) {
  if (!mkdir($rate_limit_dir, 0700, true)) {
    http_response_code(503);
    exit();
  }
}

// Use REMOTE_ADDR to avoid spoofing via X-Forwarded-For
$client_ip = $_SERVER["REMOTE_ADDR"];
$rate_file = $rate_limit_dir . "/" . hash('sha256', $client_ip) . ".json";

$fp = fopen($rate_file, 'c+');
flock($fp, LOCK_EX);
$contents = stream_get_contents($fp);
$rate_data = $contents ? json_decode($contents) : null;

if ($rate_data === null) {
  $rate_data = (object)array("window_start" => time(), "count" => 1);
} else {
  $elapsed = time() - $rate_data->window_start;

  if ($elapsed < 60 && $rate_data->count >= 100) {
    flock($fp, LOCK_UN);
    fclose($fp);
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
}

rewind($fp);
fwrite($fp, json_encode($rate_data));
fclose($fp);

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
