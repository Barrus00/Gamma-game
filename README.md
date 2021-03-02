# The Gamma game
## Introduction
This project was written as a part of Individual Programming Project class in summer semester of academic year 2019/2020.
## Description
The game takes place on a rectangle-shaped board that consists of square fields. 
Two fields are adjacent to each other if they are in the same row or column of the board.
The game is designed for one or more players. Each player can take a field by placing their token on it. At the beginning of the game, all fields are untaken. 
A group of fields that belong to the same player and are adjacent to any other field from this group is called an area. 
Formally, if we consider an undirected graph with vertices that represent every field belonging to the player and edges between adjacent fields, 
every connected component is an area. A single field is also an area.

Players have an upper bound of owned areas. 
Every player can take a field if it is untaken and taking this field wouldn't increase number of areas owned by the player over given bound.

Once per game, every player can make a golden move. The golden move is a move on a taken field. 
A player takes a field that belongs to other player. However, golden move can't cause **any** player to have more areas than the upper bound of areas.

The game can be played in batch mode and interactive mode. 
In batch mode, the game is played by writing commands. 
In interactive mode, players take turns that consist of one move (including the golden move). The game ends when every player is unable to make a move.

More information about batch and interactive modes can be found in section Usage.

## Installation
The game can be installed only on Linux. To install the game, download the repository, then change directory to your local version of the repository. 
Then use commands:
```
mkdir game
cd game
cmake ..
make
```
You can also use commands `make test` to create a test of game's engine (example tests are shown in `gamma_test.c`, if you want to add your own tests, 
run cmake command with flag ```TEST_FILE=directory```, where `directory` is a directory to your file with tests) and `make doc` to make documentation for the project.

## Documentation
If you already installed the game, in order to automatically generate documentation you have to change directory to your local version of the repository and type ```make doc```. The documentation is generated using Doxygen.
Unfortunately, the documentation is currently available only in Polish.

## Usage
To launch the game, use command ```./gamma``` in the directory you used command ```make```. 

### Batch mode
To play the game in batch mode, type command:```B width height players areas``` where width, height, players and areas should be replaced with respective numbers. For example, command ```B 10 20 2 5``` will create a game in batch mode, with 10x20 board, 2 players and 5 maximum areas owned by one player. Then, you can play the game using the following commands: 
  
```m player x y``` – tries to perform a move by player with number player on field with coordinates (x, y). Prints 1 if succeeded and 0 otherwise.

```g player x y``` – tries to perform a golden move by player with number player on field with coordinates (x, y). Prints 1 if succeeded and 0 otherwise.

```b player``` – prints the number of fields taken by specified player.

```f player``` – prints the number of fields that specified player can obtain.

```q player``` – checks, whether specified player can make a golden move. Prints 1 if the move is possible and 0 otherwise.

```p``` – prints the board.

If a command is wrong, ```ERROR line```is printed, where line is the number of line with the wrong command.

### Interactive mode

To play the game in interactive mode, type command ```I width height players areas``` where width, height, players and areas should be replaced by respective numbers. For example, command ```I 10 20 2 5``` will create a game in Interactive mode, with 10x20 board, 2 players and 5 maximum areas owned by one player. Then, you can play the game using your keyboard. 

You can move the marker using arrow keys. The game automatically chooses, which player should move (starting from player one and omiting players that can't make a move). To place your token on a field, press ```space``` when marker is on your desired field. To make a golden move on a field, press ```G``` when your desired field is marked. You can skip your turn by pressing ```C```. The game ends when every player is unable to move. You can also end the game early by pressing ```CTRL+D```. After finishing the game, final state of board is displayed along with the results for every player.

This is the recommended game mode if you want to actually play the game.
