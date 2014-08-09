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
* A depth-first search is performed over an [expectimax decision tree](http://en.wikipedia.org/wiki/Expectiminimax_tree), with alternating *max* and *chance* nodes.
* *max* and *chance* levels of the tree are generated simultaneously, to remove duplication early on.
* Furthermore, duplicate nodes are prevented with a custom hashmap, which sacrifices theoretical correctness for raw performance and memory efficiency.
* An simple and efficient cost function is used: sum all tiles, except those which lie in a decreasing zig-zag pattern starting from the tile in the upper-left corner.
* For each move, the engine cleverly estimates the depth of the decision tree (the *lookahead*):
  * The lookahead is based on the cost of the previous move, such that more computational time is spent on bad board positions.
  * The lookahead is restricted to a range that depends on the tile sum.
  * If there is a sharp increase in cost after the board is evaluated, then the evaluation is performed again with the maximum lookahead.
