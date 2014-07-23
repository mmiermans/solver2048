solver2048
==========

## Building the game engine

TL;DR: `git clone https://github.com/mmiermans/solver2048.git && cd solver2048/Solver2048 && make && ./Solver2048`

More detailed instructions follow for building the engine on Windows or Linux. A 64-bit system is required.

### Windows 64-bit

Open the `.sln` solution file with Visual Studio. Build in release mode to get the best performance. I have only tested it with VS2014, so modifications might be necessary if you would like to use an earlier version.

### Linux 64-bit

Clone the project:

```
git clone https://github.com/mmiermans/solver2048.git
```

This will create a directory called `solver2048`. Now you have the choice to use the command line or Eclipse to build the application.

#### Command line
Compiling the application on the command line requires the following package:

```
sudo apt-get install build-essential
```

Navigate to the project directory with `cd solver2048/Solver2048`. Simply run `make` or `make release` to generate an executable. The `release` target compiles with the `-O3` optimization option enabled, resulting in significantly better performance. Start the game engine with `./Solver2048`.

#### Eclipse
If your preference is to use Eclipse, then open Eclipse and import the project from `solver2048/Solver2048`. There are two possible Build Configurations: `Debug` and `Release`. The former can be debugged more easily and the latter is optimized for performance. You can select the Build Configuration as follows:

<img src="https://raw.githubusercontent.com/mmiermans/solver2048/master/doc/eclipse_build_configurations_dialog.png" alt="Eclipse Build configuration" width="400px"/>

## Running it as a server

The game engine can store its moves in a MySQL database such that the progress can be monitored on the web. Setup the server with these steps:

1. Refresh your package index: `sudo apt-get update`
2. Install LAMP: `sudo apt-get install lamp-server^`
3. Test that your server works: [http://localhost](http://localhost)
4. Import 
4. Install MySQL C development libraries: `sudo apt-get install libmysqlclient-dev`
5. Rebuild the game engine with MySQL output enabled: `make release ENABLE_SQL=1`





