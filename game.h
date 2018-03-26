/*
 * game.h
 * This file contains the header information for classes that maintain the
 * snake game's state
 */

#include <iostream>
#include <vector>
using namespace std;

#ifndef GAME_H_
#define GAME_H_

//stores a coordinate pair in the Cartesian plane
class coordinate
{
	public:
		int x;
		int y;

		coordinate(int new_x=0, int new_y=0){x = new_x; y = new_y;}
		bool operator==(const coordinate s){return x == s.x && y == s.y;}
};

//Stores the game map including the snake and food positions
class state
{
	public:
		//Size of the snake to start the game
		const int START_SIZE = 5;
		//Map limits
		const int MAP_X_LIMIT = 25;
		const int MAP_Y_LIMIT = 15;
		//Determines if the game has ended
		bool loss = false;
		//Direction modifier is a unit vector determining the direction
		//the snake is currently heading in
		coordinate direction_modifier;
		//The location of the food tile
		coordinate food;
		//A container with all the body segments of the snake
		vector<coordinate> snake;
		//Score and turn values stored over the course of the game
		int score = 0;
		int turn = 0;

		state();
		//Functions used to determine possible actions and how the state reacts
		//to those changes
		void place_food();
		vector<coordinate> actions();
		state result(coordinate action);

		//Prints the state to the standard output
		void display();

		//Helper function to determine if a coordinate contains a snake
		//body segment
		bool in_snake(coordinate& c);

		void operator=(const state& s);
};

//The game class keeps track of the current state of the game as well as the turn counter
//The game class can update based on interactions with the agent's selected actions
class game
{
	public:
		state current_state;
		int turn = 0;

		void update(coordinate action);
};

#endif /* GAME_H_ */
