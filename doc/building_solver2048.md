Building Solver2048
==========

**Table of Contents**

- [Game engine](#user-content-game-engine)
	- [Windows 64-bit](#user-content-windows-64-bit)
	- [Linux 64-bit](#user-content-linux-64-bit)
		- [Command line](#user-content-command-line)
		- [Eclipse](#user-content-eclipse)
- [Streaming server](#user-content-streaming-server)

## Game engine

TL;DR: `git clone https://github.com/mmiermans/solver2048.git && cd solver2048/Solver2048 && make && ./Debug/Solver2048`

That was the short version! What follows are more detailed instructions for building the engine on Windows or Linux. A 64-bit system is required in both cases. The Linux instructions are targeted towards Ubuntu -- because that's what I use -- so substitute apt-get with your favorite package manager if you use something else.

### Windows 64-bit

Open the `.sln` solution file with Visual Studio. Build in release mode to get the best performance. I have only tested it with VS2013 and modifications might be necessary if you use an earlier version.

### Linux 64-bit

```
git clone https://github.com/mmiermans/solver2048.git
```

This will create a directory called `solver2048`. Now you have the choice to use the command line or Eclipse to build the application.

#### Command line
Compiling the application on the command line requires the following package:

```
sudo apt-get install build-essential
```

Navigate to the project directory with `cd solver2048/Solver2048`. Simply run `make` or `make release` to generate an executable. The `release` target compiles with the `-O3` optimization option enabled, resulting in significantly better performance. Start the game engine with `./Debug/Solver2048` or `./Release/Solver2048`.

#### Eclipse
If your preference is to use Eclipse, then open Eclipse and import the project from `solver2048/Solver2048`. There are two possible Build Configurations: `Debug_Solver2048` and `Release_Solver2048`. The former can be debugged more easily and the latter is optimized for performance.

## Streaming server

The game engine's progress can be [streamed live over the internet](http://32768.eu), and the following steps tell you how to accomplish this:

1. Refresh your package index: `sudo apt-get update`
2. Install LAMP server: `sudo apt-get install lamp-server^ `
  1. Test that your server works: [http://localhost](http://localhost) 
  2. Also install the native MySQL driver for PHP `sudo apt-get install php5-mysqlnd`
  3. Install phpMyAdmin (select **Apache2** during install): `sudo apt-get install phpmyadmin`
3. Configure the database
  1. Go to [http://localhost/phpmyadmin](http://localhost/phpmyadmin)
  2. Import `solver2048/sql/solver2048.sql`
  3. Create a user (e.g. `solver2048_user`) with select, insert, update and **execute** privilege to the database `solver2048`
4. Adjust the database login info for PHP:
  1. `cd ~/solver2048/webclient/resources`
  2. Create a config file: `cp config-sample.php config.php`
  3. Edit `config.php` to match the login data for your database
5. Bring the webclient online:
  1. Remove the example html directory: `sudo rm -r /var/www/html`
  2. Create symlink: `sudo ln -s ~/solver2048/webclient/public /var/www/html`
  3. Test this by visiting [http://localhost](http://localhost), where you should see an empty 2048 board
6. Install the MySQL C development libraries: `sudo apt-get install libmysqlclient-dev`
7. Build the game engine using the make command listed below, with the following adjustments:
  1. Substitute username and password
  2. If necessary add `-DMYSQL_HOSTNAME="foo"`, `-DMYSQL_DATABASE="bar"` and/or `-DMYSQL_PORT=123` to `DEFS`. The defaults are "localhost", "solver2048" and 0, respectively.

  ```
  cd ~/solver2048/Solver2048
  make release ENABLE_MYSQL=1 DEFS='-DMYSQL_USERNAME="solver2048_user" -DMYSQL_PASSWORD="abc123"'
  ```

8. Let Apache compress its output:
  ```
  <IfModule mod_deflate.c>
    AddOutputFilterByType DEFLATE text/plain
    AddOutputFilterByType DEFLATE text/html
    AddOutputFilterByType DEFLATE text/css
    AddOutputFilterByType DEFLATE text/javascript
    AddOutputFilterByType DEFLATE application/javascript
    AddOutputFilterByType DEFLATE application/json
    AddOutputFilterByType DEFLATE image/svg+xml
  </IfModule>
  ```

9. Run `~/solver2048/Solver2048/Release/Solver2048`, go to [http://localhost](http://localhost), sit back, and relax!
