<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>Solver2048</title>

  <link href="style/main.css" rel="stylesheet" type="text/css">
  <link rel="shortcut icon" href="favicon.ico">
  <link rel="apple-touch-icon" href="meta/apple-touch-icon.png">
  <meta name="apple-mobile-web-app-capable" content="yes">
  <meta name="apple-mobile-web-app-status-bar-style" content="black">

  <meta name="HandheldFriendly" content="True">
  <meta name="MobileOptimized" content="320">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1, user-scalable=no, minimal-ui">
</head>
<body>
  <div class="container">
    <div class="heading">
      <h1 class="title">2048</h1>
      <div class="scores-container">
        <div class="score-container">0</div>
        <div class="best-container">0</div>
      </div>
    </div>

    <div class="above-game">
      <p class="game-intro">Watch a <strong>live stream</strong> of a computer playing 2048!</p>
    </div>

    <div class="game-container">
      <div class="game-message">
        <p>Game over!</p>
        <div class="lower">
          <a class="next-game-button">Next game</a>
        </div>
      </div>

      <div class="grid-container">
        <div class="grid-row">
          <div class="grid-cell"></div>
          <div class="grid-cell"></div>
          <div class="grid-cell"></div>
          <div class="grid-cell"></div>
        </div>
        <div class="grid-row">
          <div class="grid-cell"></div>
          <div class="grid-cell"></div>
          <div class="grid-cell"></div>
          <div class="grid-cell"></div>
        </div>
        <div class="grid-row">
          <div class="grid-cell"></div>
          <div class="grid-cell"></div>
          <div class="grid-cell"></div>
          <div class="grid-cell"></div>
        </div>
        <div class="grid-row">
          <div class="grid-cell"></div>
          <div class="grid-cell"></div>
          <div class="grid-cell"></div>
          <div class="grid-cell"></div>
        </div>
      </div>

      <div class="tile-container">

      </div>
    </div>
    
    <p class="game-explanation">
      <h2>Performance statistics</h2>
      <div id="max-tile-chart" style="width:100%; height:400px;"></div>
    </p>

    <p class="game-explanation">
    <a href="https://github.com/mmiermans/solver2048">A.I.</a> created by <a href="http://miermans.com" target="_blank">Mathijs Miermans.</a>
    The game 2048 was made by <a href="http://gabrielecirulli.com" target="_blank">Gabriele Cirulli</a> and can played at <a href="http://git.io/2048">http://git.io/2048.</a>
    </p>
  </div>

  <script src="//ajax.googleapis.com/ajax/libs/jquery/2.1.1/jquery.min.js"></script>
  <script src="js/highcharts/highcharts.js"></script>
  <script src="js/highcharts/themes/theme2048.js"></script>

  <script src="js/bignumber.min.js"></script>
  <script src="js/bind_polyfill.js"></script>
  <script src="js/classlist_polyfill.js"></script>
  <script src="js/animframe_polyfill.js"></script>
  <script src="js/keyboard_input_manager.js"></script>
  <script src="js/html_actuator.js"></script>
  <script src="js/grid.js"></script>
  <script src="js/tile.js"></script>
  <script src="js/game_manager.js"></script>
  <script src="js/application.js"></script>

  <script>
    window.bootMoveFeed = <?php include 'bootstrapped.php'; ?>;
  </script>
</body>
</html>
