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
      <div id="max-tile-chart" class="chart" style="width:100%; height:400px;"></div>
      <div id="score-chart" class="chart" style="width:100%; height:400px;"></div>
    </p>

    <p class="game-explanation">
    <a href="https://github.com/mmiermans/solver2048">Solver2048 engine</a> created by <a href="http://miermans.com" target="_blank">Mathijs Miermans.</a></br>
    <a href="http://git.io/2048">Original game</a> made by <a href="http://gabrielecirulli.com" target="_blank">Gabriele Cirulli.</a>
    </p>
  </div>
  
  <a href="https://github.com/mmiermans/solver2048"><img style="position: absolute; top: 0; right: 0; border: 0;" src="https://camo.githubusercontent.com/38ef81f8aca64bb9a64448d0d70f1308ef5341ab/68747470733a2f2f73332e616d617a6f6e6177732e636f6d2f6769746875622f726962626f6e732f666f726b6d655f72696768745f6461726b626c75655f3132313632312e706e67" alt="Fork me on GitHub" data-canonical-src="https://s3.amazonaws.com/github/ribbons/forkme_right_darkblue_121621.png"></a>

  <script src="//ajax.googleapis.com/ajax/libs/jquery/2.1.1/jquery.min.js"></script>
  <script src="http://code.highcharts.com/highcharts.js"></script>
  <script src="http://code.highcharts.com/highcharts-more.js"></script>
  <script src="js/highcharts/themes/theme2048.js"></script>

  <script src="//cdnjs.cloudflare.com/ajax/libs/bignumber.js/1.4.1/bignumber.min.js"></script>
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
    window.bootMoveFeed = <?php
      $fetchGameStats = true;
      $enableJsonContentType = false;
      include 'getmovefeed.php';
    ?>;
    
    window.startApplication();
  </script>
</body>
</html>
