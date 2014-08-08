function HTMLActuator() {
  this.tileContainer    = document.querySelector(".tile-container");
  this.scoreContainer   = document.querySelector(".score-container");
  this.bestContainer    = document.querySelector(".best-container");
  this.messageContainer = document.querySelector(".game-message");
  this.maxTileContainer = document.querySelector(".max-tile-chart");

  this.score = 0;
}

HTMLActuator.prototype.actuate = function (grid, metadata) {
  var self = this;

  window.requestAnimationFrame(function () {
    self.clearContainer(self.tileContainer);

    grid.cells.forEach(function (column) {
      column.forEach(function (cell) {
        if (cell) {
          self.addTile(cell);
        }
      });
    });

    self.updateScore(metadata.score);
    self.updateBestScore(metadata.bestScore);

    if (metadata.terminated) {
      if (metadata.over) {
        self.message(false); // You lose
      } else if (metadata.won) {
        self.message(true); // You win!
      }
    }

  });
};

// Continues the game (both restart and keep playing)
HTMLActuator.prototype.continueGame = function () {
  this.clearMessage();
};

HTMLActuator.prototype.clearContainer = function (container) {
  while (container.firstChild) {
    container.removeChild(container.firstChild);
  }
};

HTMLActuator.prototype.addTile = function (tile) {
  var self = this;

  var wrapper   = document.createElement("div");
  var inner     = document.createElement("div");
  var position  = tile.previousPosition || { x: tile.x, y: tile.y };
  var positionClass = this.positionClass(position);

  // We can't use classlist because it somehow glitches when replacing classes
  var classes = ["tile", "tile-" + tile.value, positionClass];

  if (tile.value > 2048) classes.push("tile-super");

  this.applyClasses(wrapper, classes);

  inner.classList.add("tile-inner");
  inner.textContent = tile.value;

  if (tile.previousPosition) {
    // Make sure that the tile gets rendered in the previous position first
    window.requestAnimationFrame(function () {
      classes[2] = self.positionClass({ x: tile.x, y: tile.y });
      self.applyClasses(wrapper, classes); // Update the position
    });
  } else if (tile.mergedFrom) {
    classes.push("tile-merged");
    this.applyClasses(wrapper, classes);

    // Render the tiles that merged
    tile.mergedFrom.forEach(function (merged) {
      self.addTile(merged);
    });
  } else {
    classes.push("tile-new");
    this.applyClasses(wrapper, classes);
  }

  // Add the inner part of the tile to the wrapper
  wrapper.appendChild(inner);

  // Put the tile on the board
  this.tileContainer.appendChild(wrapper);
};

HTMLActuator.prototype.applyClasses = function (element, classes) {
  element.setAttribute("class", classes.join(" "));
};

HTMLActuator.prototype.normalizePosition = function (position) {
  return { x: position.x + 1, y: position.y + 1 };
};

HTMLActuator.prototype.positionClass = function (position) {
  position = this.normalizePosition(position);
  return "tile-position-" + position.x + "-" + position.y;
};

HTMLActuator.prototype.updateScore = function (score) {
  this.clearContainer(this.scoreContainer);

  var difference = score - this.score;
  this.score = score;

  this.scoreContainer.textContent = this.score;

  if (difference > 0) {
    var addition = document.createElement("div");
    addition.classList.add("score-addition");
    addition.textContent = "+" + difference;

    this.scoreContainer.appendChild(addition);
  }
};

HTMLActuator.prototype.updateBestScore = function (bestScore) {
  this.bestContainer.textContent = bestScore;
};

HTMLActuator.prototype.message = function (won) {
  var type    = won ? "game-won" : "game-over";
  var message = won ? "You win!" : "Game over!";

  this.messageContainer.classList.add(type);
  this.messageContainer.getElementsByTagName("p")[0].textContent = message;
};

HTMLActuator.prototype.clearMessage = function () {
  // IE only takes one value to remove at a time.
  this.messageContainer.classList.remove("game-won");
  this.messageContainer.classList.remove("game-over");
};

HTMLActuator.prototype.loadCharts = function (data) {
  if (!data || data.length == 0) return;
  
  var maxTileData = data["max_tile"];
  var minCat = 1<<16;
  var maxCat = 2;
  for (var i = 0; i < maxTileData.length; i++) {
    minCat = Math.min(minCat, maxTileData[i][0]);
    maxCat = Math.max(maxCat, maxTileData[i][0]);
  }
  
  var maxTileCategories = [];
  var maxTileSeries = [];
  for (var i = minCat, j = 0; i <= maxCat; i *= 2) {
    var name = i.toString();
    maxTileCategories.push(name);
    var y = 0;
    if (j < maxTileData.length && maxTileData[j][0] == i) {
      y = maxTileData[j][1];
      j++;
    }
    var logI = Math.round(Math.log(i)/Math.log(2));
    maxTileSeries.push({name:name, x:logI, y:y});
  }

  $('#max-tile-chart').highcharts({
    chart: {
      type: 'column',
      backgroundColor: '#faf8ef',
    },
    title: {
      text: null,
    },
    legend: {
      enabled: false,
    },
    xAxis: {
      type: 'category',
      title: {
        text: 'Highest tile',
      },
    },
    yAxis: {
      title: {
        text: 'Games',
      },
      allowDecimals: false,
    },
    series: [{
      name: 'Number of games',
      data: maxTileSeries,
    }]
  });
  
  this.loadScoreChart(data.score);
};

HTMLActuator.prototype.loadScoreChart = function (scoreData) {
  var scoreLen = scoreData.length;
  if (scoreLen == 0)
    return;
  
  scoreData.sort(function(a, b){return a-b});
  
  var sum = scoreData.reduce(function(a, b) {
    return a + b;
  });
  var avg = sum / scoreLen;

  // Calculate quartiles
  var qData = [];
  for (var i = 0; i <= 4; i++) {
    var j = (i/4) * (scoreLen - 1);
    var l = Math.floor(j);
    var h = Math.ceil(j);
    var val = Math.round(((1+l-j) * scoreData[l]) + ((j-l) * scoreData[h]));
    qData.push(val);
  }
 
  var chart = $('#score-chart').highcharts();
  if (chart) {
    chart.destroy();
  }
  
  if (scoreData.length >= 5) {
    $('#score-chart').show();
    $('#score-chart').highcharts({
      chart: {
        type: 'boxplot'
      },
      
      title: {
        text: null
      },
      
      legend: {
        enabled: false
      },

      xAxis: {
        lineWidth: 0,
        minorGridLineWidth: 0,
        lineColor: 'transparent',
        minorTickLength: 0,
        tickLength: 0,
        title: {
          text: 'Score box plot'
        },
        labels: {
          enabled: false
        },
      },
      
      yAxis: {
        title: {
          text: 'Score'
        },
        gridLineWidth: 1,
        gridLineColor: '#A59D94',
        plotLines: [{
          value: avg,
          color: '#f65e3b',
          width: 2,
          label: {
            text: 'Average: ' + Math.round(avg/1000) + 'k',
            align: 'left',
            style: {
              color: '#776e65'
            }
          }
        }]
      },

      series: [{
        name: 'Score stats',
        //pointWidth: '100',
        pointPadding: 0,
        //pointValKey: 'y',
        groupPadding: 0.32,
        data: [ qData ],
        tooltip: {
          headerFormat: ''
        }
      }],
      
      plotOptions: {
        boxplot: {
          fillColor: '#FFE8D5',
        },
      }
    });
  } else {
    $('#score-chart').hide();
  }
}

HTMLActuator.prototype.updateCharts = function (data, maxTile, score) {
  maxTile = maxTile.toString();
  
  var maxTileChart = $('#max-tile-chart').highcharts();
  var maxTileSeries = maxTileChart.series[0];
  
  // Update or add a point to the maxTileChart.
  var logMaxTile = Math.round(Math.log(maxTile)/Math.log(2));
  var index = maxTileSeries.processedXData.indexOf(logMaxTile);
  if (index >= 0) {
    var point = maxTileSeries.data[index];
    point.update(point.y + 1);
  } else {
    if (logMaxTile < maxTileSeries.xData[0]) {
      startI = logMaxTile;
      endI = maxTileSeries.xData[0];
    } else {
      endI = logMaxTile+1;
      startI = maxTileSeries.xData[maxTileSeries.xData.length-1];
    }
    for (var i = startI; i < endI; i++) {
      var y = (i == logMaxTile) ? 1 : 0;
      var logI = Math.round(Math.log(i)/Math.log(2));
      maxTileSeries.addPoint({name: (1 << i).toString(), x:i, y: y});
    }
  }
  
  // Add a point to the scoreChart.
  data.score.push(score);
  this.loadScoreChart(data.score);
};

