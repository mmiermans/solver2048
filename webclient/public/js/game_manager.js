function GameManager(size, Actuator, bootFeed) {
  this.size           = size; // Size of the grid
  this.actuator       = new Actuator;

  this.startTiles     = 2;

  this.requestPeriod  = 5000;
  this.isRequestInProgress = false;
  this.lastRequestTime = 0;

  this.movePeriod = 100;

  this.moveFeed = [];
  this.concatMoves(bootFeed);
  
  this.setup();
}

// Restart the game
GameManager.prototype.restart = function () {
  this.actuator.continueGame(); // Clear the game won/lost message
  this.setup();
};

// Return true if the game is lost, or has won and the user hasn't kept playing
GameManager.prototype.isGameTerminated = function () {
  return this.over;
};

// Set up the game
GameManager.prototype.setup = function () {
  this.processMoveFeed();

  // Update the actuator
  this.actuate();
};

// Initialize grid to first move in feed
GameManager.prototype.initGrid = function (m) {
  this.grid        = new Grid(this.size, m.board_before_move);
  this.score       = this.calculateScore(m.move_count);
  this.over        = false;
  this.won         = this.hasWon();
}

// Add moves to feed
GameManager.prototype.concatMoves = function (moves) {
  if (moves instanceof Array) {
    this.moveFeed = this.moveFeed.concat(moves);
    if (moves.length > 0)
      this.moveFeedEnd = moves[moves.length-1].move_count + 1;
  } else {
    throw "Expecting moves to be an array";
  }
}

// Perform AJAX request to receive moves
GameManager.prototype.requestMoves = function () {
  var that = this;
  
  // Don't handle two requests at the same time.
  if (this.isRequestInProgress || Date.now() - this.lastRequestTime < this.requestPeriod)
    return;
  
  var url = "getmovefeed.php?" + encodeQueryData({
    gameid: this.gameId,
    movecount: this.moveFeedEnd
  });

  xmlhttp = new XMLHttpRequest();
  // Completion callback
  xmlhttp.onreadystatechange = function() {
    if (xmlhttp.readyState==4 && xmlhttp.status==200) {
      var newMoves = JSON.parse(xmlhttp.responseText);
      that.concatMoves(newMoves);
      that.isRequestInProgress = false;
    }
    // TODO: handle other cases besides 4/200.
  };

  xmlhttp.open("GET", url, true);
  xmlhttp.send();
  this.isRequestInProgress = true;
  this.lastRequestTime = Date.now();
}

// Performs moves in the move feed and request more moves when necessary
GameManager.prototype.processMoveFeed = function () {
  var that = this;

  if (this.moveFeed.length > 0)
    this.executeMoveFromFeed();
  
  if (this.moveFeed.length * this.movePeriod < this.requestPeriod) {
    this.requestMoves();
  }

  window.setTimeout(function() {
    that.processMoveFeed();
  }, that.movePeriod);
}

// Performs the first move in the move feed
GameManager.prototype.executeMoveFromFeed = function () {
  var m = this.moveFeed.shift();
  
  this.gameId = m.game_id;
  
  if (this.moveCount + 1 != m.move_count) {
    this.initGrid(m);
  }
  this.moveCount = m.move_count;

  this.move(this.getDirection(m.move_direction));

  var tile = new Tile(
    { x: m.tile_position%this.size, y: Math.floor(m.tile_position/this.size) },
    m.tile_value
  );
  this.grid.insertTile(tile);
  
  this.moveFeedIndex++;

  this.actuate();
};

// Sends the updated grid to the actuator
GameManager.prototype.actuate = function () {
  this.actuator.actuate(this.grid, {
    score:      this.score,
    over:       this.over,
    won:        this.won,
    bestScore:  1234, // TODO
    terminated: this.isGameTerminated()
  });

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

GameManager.prototype.getDirection = function (direction) {
  var map = {
    'up': 0,
    'right': 1,
    'down': 2,
    'left': 3
  };

  return map[direction];
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

// Calculate score from grid
GameManager.prototype.calculateScore = function (moveCount) {
  var score = 0;
  var tileSum = 0;
  this.grid.eachCell(function (x, y, tile) {
    if (tile && tile.value > 0) {
      score += tile.value * Math.round(Math.log(tile.value) / Math.LN2) - 1;
      tileSum += tile.value;
    }
  });
  // Subtract score lost from 4 tiles that were inserted.
  // Assumes first two tiles were 2's.
  var lossFrom4 = 2 * (tileSum - (2 * moveCount) - 4);
  return score - lossFrom4;
};

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
