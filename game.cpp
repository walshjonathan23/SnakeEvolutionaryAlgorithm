/*
 * game.cpp
 * This file contains the function implementations for the state and game classes
 */
#include <iostream>
#include "game.h"
using namespace std;

state::state()
{
	score = 0;
	turn = 0;
	loss = false;
	direction_modifier = coordinate(1,0);
	for(int i = 0; i < START_SIZE; i++)
	{
		snake.push_back(coordinate(i,0));
	}
	place_food();
}

//Randomly generates a new coordinate to host the food which is not
//currently occupied by any piece of the snake
void state::place_food()
{
	int random_x;
	int random_y;
	coordinate new_coord;
	do
	{
		random_x = rand() % MAP_X_LIMIT;
		random_y = rand() % MAP_Y_LIMIT;
		new_coord = coordinate(random_x, random_y);
	}while(in_snake(new_coord));
	food = new_coord;
}

//returns a list of all the possible actions the user can take
//from a given state.
//The result is deterministic based on the current direction
//of movement.
vector<coordinate> state::actions()
{
	//Possible actions are unit vectors for
	//the cardinal directions
	const coordinate UP = coordinate(0,-1);
	const coordinate DOWN = coordinate(0,1);
	const coordinate LEFT = coordinate(-1,0);
	const coordinate RIGHT = coordinate(1,0);

	vector<coordinate> possible_actions;

	//The possible actions are turning left, right, or
	//continuing in the current direction
	if(direction_modifier == UP)
	{
		possible_actions.push_back(UP);
		possible_actions.push_back(LEFT);
		possible_actions.push_back(RIGHT);
	}
	else if(direction_modifier == DOWN)
	{
		possible_actions.push_back(DOWN);
		possible_actions.push_back(LEFT);
		possible_actions.push_back(RIGHT);
	}
	else if(direction_modifier == LEFT)
	{
		possible_actions.push_back(UP);
		possible_actions.push_back(LEFT);
		possible_actions.push_back(DOWN);
	}
	else if(direction_modifier == RIGHT)
	{
		possible_actions.push_back(UP);
		possible_actions.push_back(DOWN);
		possible_actions.push_back(RIGHT);
	}

	return possible_actions;
}

//returns the current state after it has taken the provided action
state state::result(coordinate action)
{
	//Create a new state
	state new_state;
	new_state = *this;
	//Update the direction based on the action taken
	new_state.direction_modifier = action;
	//Increment the turn counter
	new_state.turn++;
	//Locate the snake's new head position based on the unit vector of the direction modifier
	coordinate new_position = coordinate(snake.back().x + action.x, snake.back().y + action.y);
	//If the snake finds food, increase its size by 1, increment the score, and replace the food
	if(new_state.food == new_position)
	{
		new_state.snake.push_back(new_position);
		new_state.score++;
		new_state.place_food();
	}
	//If the snake's new position exits the map, it is a loss state
	else if(new_position.x >= MAP_X_LIMIT || new_position.y >= MAP_Y_LIMIT ||
			new_position.x < 0 || new_position.y < 0)
	{
		new_state.loss = true;
	}
	//If the snake's new position collides with a snake body segment, it is a loss state
	else if(new_state.in_snake(new_position))
	{
		new_state.loss = true;
	}
	//Without unusual circumstances, the snake's head proceeds and the back body segment
	//is removed to maintain the body length
	else
	{
		new_state.snake.push_back(new_position);
		new_state.snake.erase(new_state.snake.begin());
	}
	//returns the independent new state
	return new_state;
}

//prints the current grid with marked snake and food positions
//to the standard output
void state::display()
{
	for(int i = 0; i < MAP_Y_LIMIT; i++)
	{
		for(int j = 0; j < MAP_X_LIMIT; j++)
		{
			coordinate current_coord = coordinate(j,i);
			//Print snake body segments
			if(in_snake(current_coord))
			{
				cout << "S";
			}
			//Print the food tile
			else if(current_coord == food)
			{
				cout << "F";
			}
			//Print empty spaces
			else
			{
				cout << "-";
			}
		}
		cout << endl;
	}
	cout << endl;
	if(loss)
	{
		cout << "Game Over!" << endl;
		cout << "Score: " << score << endl;
	}
	return;
}

//returns true if the provided coordinate is part of the snake
//returns false otherwise
bool state::in_snake(coordinate& c)
{
	for(unsigned int i = 0; i < snake.size(); i++)
	{
		if(c == snake[i])
			return true;
	}
	return false;
}

void state::operator=(const state& s)
{
	direction_modifier = s.direction_modifier;
	food = s.food;
	score = s.score;
	loss = s.loss;

	snake.clear();
	for(unsigned int i = 0; i < s.snake.size(); i++)
	{
		snake.push_back(s.snake[i]);
	}
}

//updates the game's current state by taking the provided action
//incrementing the turn counter and determining what direction the snake
//is going
void game::update(coordinate action)
{
	current_state.direction_modifier = action;
	current_state = current_state.result(action);
	turn++;
}
