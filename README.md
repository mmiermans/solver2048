solver2048
==========

## Building the game engine

Solver2048 is an application written in C++ that computes the best moves in the game 2048. Windows and Linux are supported, but a 64-bit version is required. The result will be something like this:

```
Move count: 40	Time: 0s 	Score: 380
Lookahead: 3	Avg Moves/s: 40.8155	Moves/s: 39.1436	kNodes/s: 2424
#############################
#      #      #      #      #
#   64 #   16 #    4 #      #
#      #      #      #      #
#############################
#      #      #      #      #
#    2 #      #    2 #      #
#      #      #      #      #
#############################
#      #      #      #      #
#    4 #    4 #      #      #
#      #      #      #      #
#############################
#      #      #      #      #
#    2 #      #      #      #
#      #      #      #      #
########## 0x0001002201010246
```

### Windows

Very easy: open the `.sln` solution file. Build in release mode to get the best performance.

### Linux

You should have build-essential and git installed:

```
sudo apt-get install build-essential git
```

Navigate a place of your choosing (e.g. `cd ~/`) and clone the project:

```
git clone https://github.com/mmiermans/solver2048.git
```

This will create a directory called `solver2048`. Now you have the choice to use Eclipse or the command line to build the application.

If you like to use the command line, then `cd solver2048/Solver2048`. Simply run `make` or `make release` to generate an executable. The latter generates an executable with a better performance, while the former is more suitable for debugging purposes. Start the game engine with `./Solver2048`.

If your preference is to use Eclipse, then open Eclipse and import the project from `solver2048/Solver2048`. There are two possible Build Configurations: `Debug` and `Release`. The former can be debugged more easily and the latter is optimized for performance. You can select the Build Configuration as follows:

<img src="https://raw.githubusercontent.com/mmiermans/solver2048/master/doc/eclipse_build_configurations_dialog.png" alt="Eclipse Build configuration" width="400px"/>
