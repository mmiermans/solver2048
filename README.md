solver2048
==========

## Building the game engine

Solver2048 is an application written in C++ that computes the best moves in the game 2048. It can be compiled on Windows with Visual Studio or on Linux with G++.

### Windows

Very easy: open the .sln project solution file. Build in release mode to get the best performance.

### Linux

Clone the project:

```
git clone https://github.com/mmiermans/solver2048.git
```

You should have build-essential installed:

```
sudo apt-get install build-essential
```

Now you have the choice to use Eclipse or the command line to build the application.

If you like to use the command line, then `cd` to `Solver2048`, e.g. `cd ~/solver2048/Solver2048`. Simply run `make` or `make release` to generate an executable. The latter generates an executable with a better performance, while the former is more suitable for debugging purposes. Start the game engine with `./Solver2048`.

If your preference is to use Eclipse, then open Eclipse and import the project file that is provided. There are two targets: `Debug` and `Release`. The former can be debugged more easily and the latter is optimized for performance.
