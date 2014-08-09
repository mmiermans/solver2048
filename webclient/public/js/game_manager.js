function GameManager(size, InputManager, Actuator, bootFeed) {
  this.size           = size; // Size of the grid
  this.actuator       = new Actuator;
  this.inputManager   = new InputManager;

  this.startTiles     = 2;

  this.inputManager.on("nextGame", this.nextGame.bind(this));

  this.requestPeriod  = 15000;
  this.maxMovePeriod = 2000;
  this.isRequestInProgress = false;
  this.lastRequestTime = Date.now();
  this.gamesAddedToStats = [];
  
  this.setup(bootFeed, true);

  this.stats = bootFeed["stats"];
  this.actuator.loadCharts(this.stats);
  this.bestScore = Math.max.apply(0, this.stats.score);

  window.actuator = this.actuator;
}

// Continue to the next game
GameManager.prototype.nextGame = function () {
  this.requestMinGame = this.gameInfo.id + 1;
  
  this.actuator.continueGame(); // Clear the game won/lost message
  
  this.initGrid();
  this.setup();
  
  this.requestMoves();
};

// Initialize grid to first move in feed
GameManager.prototype.initGrid = function (m) {
  if (m) {
    this.grid    = new Grid(this.size, m.board_before_move);
    this.score   = m.score_before_move;
    this.over    = false;
    this.won     = this.hasWon();
  } else {
    this.grid    = new Grid(this.size);
    this.score   = 0;
    this.over    = false;
    this.won     = false;
  }
};

// Return true if the game is lost, or has won and the user hasn't kept playing
GameManager.prototype.isGameTerminated = function () {
  return this.over;
};

// Set up the game
GameManager.prototype.setup = function (bootFeed, enableTimer) {
  this.roundTrip = 1000;

  this.movePeriod = 200;
  this.movePeriodDelta = 0;
  this.movePeriodTarget = this.movePeriod;
  this.feedTarget = 3 * this.requestPeriod;

  this.moveFeed = [];
  this.moveFeedEnd = 0;
  this.gameInfo = null;
  
  if (bootFeed) {
    var parsedFeed = this.parseMoveFeed(bootFeed);
    this.gameInfo = parsedFeed.game;
    
    var len = parsedFeed.moves.length;
    var sliceIndex = Math.max(0, len - 2*(this.requestPeriod / this.movePeriod));
    if (bootFeed.game.has_ended) {
      this.gamesAddedToStats.push(bootFeed.game.id);
      sliceIndex = 0;
      // TODO: DEBUG CODE
      this.movePeriodTarget = 0;
      this.movePeriod = 0;
      this.maxMovePeriod = 5000;
    }

    this.executedMoves = parsedFeed.moves.slice(0, sliceIndex);
    this.concatMoves(parsedFeed.moves.slice(sliceIndex, len));
    
  }

  if (enableTimer) {
    this.processMoveFeed();
  }

  // Update the actuator
  this.actuate();
};

// Add moves to feed
GameManager.prototype.concatMoves = function (moves) {
  if (moves instanceof Array) {
    this.moveFeed = this.moveFeed.concat(moves);
    if (moves.length > 0)
      this.moveFeedEnd = moves[moves.length-1].move_count + 1;
  } else {
    throw "Expecting moves to be an array";
  }
};

// Perform AJAX request to receive moves
GameManager.prototype.requestMoves = function () {
  var that = this;
  
  // Prevent too many requests from happening.
  if (!this.requestMinGame) {
    if (this.isRequestInProgress || Date.now() - this.lastRequestTime < this.requestPeriod)
      return;
  }
  
  // Build URL
  var url = "getmovefeed.php"
  if (this.gameInfo && this.gameInfo.id && this.moveFeedEnd !== undefined) {
    url += "?" + encodeQueryData({
      gameid: this.gameInfo.id,
      movecount: this.moveFeedEnd
    });
  }

  xmlhttp = new XMLHttpRequest();
  // Completion callback
  xmlhttp.onreadystatechange = function() {
    if (xmlhttp.readyState==4 && xmlhttp.status==200) {
      var json = JSON.parse(xmlhttp.responseText);
      var parsed = that.parseMoveFeed(json);
      
      // Only accept games larger than requestMinGame.
      if (!that.requestMinGame || parsed.game.id >= that.requestMinGame) {
        that.gameInfo = parsed.game;
        var newMoves = parsed.moves;
        var curTime = Date.now();
        
        if (!that.requestMinGame) {
          // Estimate milliseconds/move
          var requestDelta = Math.max(0, curTime - that.lastRequestTime - (that.requestPeriod / 2));
          var newEstimate = Math.min(2000, requestDelta / (newMoves.length + 0.5));
          var avgPeriod = (newEstimate + that.movePeriod) / 2;
          var feedError = ((that.moveFeed.length + 0.5) * avgPeriod) / that.feedTarget;
          that.movePeriodTarget = newEstimate / feedError;
          that.movePeriodTarget = Math.max(that.movePeriodTarget, 0);
          that.movePeriodTarget = Math.min(that.movePeriodTarget, that.requestPeriod);
        }

        that.roundTrip = curTime - that.requestTime;
        that.concatMoves(newMoves);
        that.requestMinGame = 0;
        that.isRequestInProgress = false;
      }
      
      that.lastRequestTime = curTime;
    }
  };

  xmlhttp.open("GET", url, true);
  xmlhttp.send();
  this.requestTime = Date.now();
  this.isRequestInProgress = true;
};

GameManager.prototype.parseMoveFeed = function(numFeed) {
  var directionMap = {
    1: 0,  // Up
    2: 1,  // Right
    4: 2,  // Down
    8: 3   // Left
  };
  
  return {
    "game": numFeed["game"],
    "moves": numFeed["moves"].map(function(move) {
      return {
        "board_before_move": move[0],
        "score_before_move": move[1],
        "direction":         directionMap[move[2]],
        "move_count":        move[3],
        "new_tile_value":    move[4],
        "new_tile_position": move[5]
      };
    }),
  };
};

// Performs moves in the move feed and request more moves when necessary
GameManager.prototype.processMoveFeed = function () {
  var that = this;
  
  // Determine whether to make a new request.
  if (!this.gameInfo) {
      this.requestMoves();
  } else if (this.gameInfo.has_ended != 1) {
    if (this.moveFeed.length * this.movePeriod < this.feedTarget + this.roundTrip ||
        Date.now() - this.lastRequestTime > this.feedTarget) {
      this.requestMoves();
    }
  }

  // Perform the next move in the feed.
  var timeoutPeriod = 1000;
  if (this.moveFeed.length > 0) {
    this.executeMoveFromFeed();
    
    // Slowly change the movePeriod towards the movePeriodTarget.
    var steps;
    if (this.movePeriod < this.movePeriodTarget) {
      steps = this.moveFeed.length / 10;
    } else {
      steps = 10;
    }
    steps = Math.min(50, Math.max(1, steps));
    var change = this.movePeriodTarget - this.movePeriod;
    var newDelta = 0.5 * (change / steps) + 0.5 * this.movePeriodDelta;
    if (newDelta + this.movePeriod < 0)
      newDelta = -this.movePeriod;
    else if (newDelta + this.movePeriod > this.maxMovePeriod)
      newDelta = this.maxMovePeriod - this.movePeriod;
    this.movePeriod += newDelta;
    this.movePeriodDelta = newDelta;
    
    timeoutPeriod = this.movePeriod;
  }

  // Schedule a new move.
  if (timeoutPeriod == 0 &&
      that.moveStackCounter !== undefined &&
      that.lastTimerEvent !== undefined &&
      that.moveStackCounter < 2048 &&
      window.performance.now() - that.lastTimerEvent < 1000/24) {
    that.moveStackCounter++;
    that.processMoveFeed();
  } else {
    that.moveStackCounter = 0;
    that.lastTimerEvent = window.performance.now();
    window.setTimeout(function() {
      that.processMoveFeed();
    }, timeoutPeriod);
  }
};

// Performs the first move in the move feed
GameManager.prototype.executeMoveFromFeed = function () {
  var m = this.moveFeed.shift();
  
  if (this.moveCount + 1 != m.move_count) {
    this.initGrid(m);
  }
  this.moveCount = m.move_count;

  this.move(m.direction);

  var tile = new Tile(
    {
      x: m.new_tile_position % this.size,
      y: Math.floor(m.new_tile_position / this.size)
    },
    m.new_tile_value
  );
  this.grid.insertTile(tile);

  if (this.moveFeed.length == 0 && this.gameInfo.has_ended) {
    this.over = true;
    if (this.gamesAddedToStats.indexOf(this.gameInfo.id) < 0) {
      this.gamesAddedToStats.push(this.gameInfo.id);
      this.actuator.updateCharts(this.stats, this.getMaxTile(), this.score);
    }
  }
  
  this.moveFeedIndex++;

  if (this.over || (window.performance.now() - this.lastActuateTime > 1000/24)) {
    this.actuate();
  }
};

// Sends the updated grid to the actuator
GameManager.prototype.actuate = function () {
  if (this.grid) {
    this.actuator.actuate(this.grid, {
      score:      this.score,
      over:       this.over,
      won:        this.won,
      bestScore:  this.bestScore,
      terminated: this.isGameTerminated()
    });
    this.lastActuateTime = window.performance.now();
  }
};

// Save all tile positions and remove merger info
GameManager.prototype.prepareTiles = function () {
  this.grid.eachCell(function (x, y, tile) {
    if (tile) {
      tile.mergedFrom = null;
      tile.savePosition();
    }
  });
};

// Move a tile and its representation
GameManager.prototype.moveTile = function (tile, cell) {
  this.grid.cells[tile.x][tile.y] = null;
  this.grid.cells[cell.x][cell.y] = tile;
  tile.updatePosition(cell);
};

// Move tiles on the grid in the specified direction
GameManager.prototype.move = function (direction) {
  // 0: up, 1: right, 2: down, 3: left
  var self = this;

  if (this.isGameTerminated()) return; // Don't do anything if the game's over

  var cell, tile;

  var vector     = this.getVector(direction);
  var traversals = this.buildTraversals(vector);
  var moved      = false;

  // Save the current tile positions and remove merger information
  this.prepareTiles();

  // Traverse the grid in the right direction and move tiles
  traversals.x.forEach(function (x) {
    traversals.y.forEach(function (y) {
      cell = { x: x, y: y };
      tile = self.grid.cellContent(cell);

      if (tile) {
        var positions = self.findFarthestPosition(cell, vector);
        var next      = self.grid.cellContent(positions.next);

        // Only one merger per row traversal?
        if (next && next.value === tile.value && !next.mergedFrom) {
          var merged = new Tile(positions.next, tile.value * 2);
          merged.mergedFrom = [tile, next];

          self.grid.insertTile(merged);
          self.grid.removeTile(tile);

          // Converge the two tiles' positions
          tile.updatePosition(positions.next);

          // Update the score
          self.score += merged.value;
          self.bestScore = Math.max(self.bestScore, self.score);

          // The mighty 2048 tile
          if (merged.value === 2048) self.won = true;
        } else {
          self.moveTile(tile, positions.farthest);
        }

        if (!self.positionsEqual(cell, tile)) {
          moved = true; // The tile moved from its original cell!
        }
      }
    });
  });

  if (moved) {
    if (!this.movesAvailable()) {
      this.over = true; // Game over!
    }
  }
};

// Get the vector representing the chosen direction
GameManager.prototype.getVector = function (direction) {
  // Vectors representing tile movement
  var map = {
    0: { x: 0,  y: -1 }, // Up
    1: { x: 1,  y: 0 },  // Right
    2: { x: 0,  y: 1 },  // Down
    3: { x: -1, y: 0 }   // Left
  };

  return map[direction];
};

// Build a list of positions to traverse in the right order
GameManager.prototype.buildTraversals = function (vector) {
  var traversals = { x: [], y: [] };

  for (var pos = 0; pos < this.size; pos++) {
    traversals.x.push(pos);
    traversals.y.push(pos);
  }

  // Always traverse from the farthest cell in the chosen direction
  if (vector.x === 1) traversals.x = traversals.x.reverse();
  if (vector.y === 1) traversals.y = traversals.y.reverse();

  return traversals;
};

GameManager.prototype.findFarthestPosition = function (cell, vector) {
  var previous;

  // Progress towards the vector direction until an obstacle is found
  do {
    previous = cell;
    cell     = { x: previous.x + vector.x, y: previous.y + vector.y };
  } while (this.grid.withinBounds(cell) &&
           this.grid.cellAvailable(cell));

  return {
    farthest: previous,
    next: cell // Used to check if a merge is required
  };
};

GameManager.prototype.movesAvailable = function () {
  return this.grid.cellsAvailable() || this.tileMatchesAvailable();
};

// Check for available matches between tiles (more expensive check)
GameManager.prototype.tileMatchesAvailable = function () {
  var self = this;

  var tile;

  for (var x = 0; x < this.size; x++) {
    for (var y = 0; y < this.size; y++) {
      tile = this.grid.cellContent({ x: x, y: y });

      if (tile) {
        for (var direction = 0; direction < 4; direction++) {
          var vector = self.getVector(direction);
          var cell   = { x: x + vector.x, y: y + vector.y };

          var other  = self.grid.cellContent(cell);

          if (other && other.value === tile.value) {
            return true; // These two tiles can be merged
          }
        }
      }
    }
  }

  return false;
};

// Find maximum tile on grid
GameManager.prototype.getMaxTile = function() {
  var maxTile = 0;
  this.grid.eachCell(function (x, y, tile) {
    if (tile && tile.value > maxTile) {
      maxTile = tile.value;
    }
  });
  return maxTile;
}

// Determine whether a tile of value >= 2048 exists
GameManager.prototype.hasWon = function () {
  this.grid.eachCell(function (x, y, tile) {
    if (tile && tile >= 2048) {
      return true;
    }
  });
  return false;
};

GameManager.prototype.positionsEqual = function (first, second) {
  return first.x === second.x && first.y === second.y;
};
