Solver2048
==========

## Demo

You can view a live stream of the A.I. in action here: [http://32768.eu](http://32768.eu).

## Build

*Main article: [Building Solver2048](/doc/building_solver2048.md)*

TL;DR: `git clone https://github.com/mmiermans/solver2048.git && cd solver2048/Solver2048 && make && ./Debug/Solver2048`

## Design

*Main article: [Solver Design](/doc/solver_design.md)*

The engine has the following features:

* The game state is stored compactly in a 64-bit integer [bitboard](http://en.wikipedia.org/wiki/Bitboard).
* A depth-first search is performed over an [expectimax tree](http://en.wikipedia.org/wiki/Expectiminimax_tree) with alternating *max* and *chance* nodes.
* *Max* and *chance* nodes are generated simultaneously to remove some duplicate nodes early on.
* More duplicate nodes are prevented with a custom hashmap, which sacrifices theoretical correctness for raw performance and memory efficiency.
* A simple and efficient cost function is used: sum all tiles, except those lying in a decreasing zig-zag pattern starting from the upper-left corner.
* For each move, the engine cleverly determines what the *look-ahead* (tree depth) should be:
  * The look-ahead is based on the cost of the previous move, such that more computational time is spent on bad board positions.
  * The look-ahead is restricted to a range that depends on the sum of all tiles.
  * If there is a sharp increase in cost after the board is evaluated, then the evaluation is performed again with maximum look-ahead.
