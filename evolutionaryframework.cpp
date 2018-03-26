/*
 * evolutionaryframework.cpp
 * This file contains the function implementations for the genome and evolution classes
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>
#include "game.h"
#include "evolutionaryframework.h"
using namespace std;

//Assigns random values between -0.5 and 0.5 for all genes in the genome
void genome::randomize()
{
	gene_turn_count = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
	gene_turn_count = gene_turn_count - 0.5;

	gene_score = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
	gene_score = gene_score - 0.5;

	gene_distance_to_food = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
	gene_distance_to_food = gene_distance_to_food - 0.5;

	gene_distance_to_top_edge = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
	gene_distance_to_top_edge = gene_distance_to_top_edge - 0.5;

	gene_distance_to_bottom_edge = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
	gene_distance_to_bottom_edge = gene_distance_to_bottom_edge - 0.5;

	gene_distance_to_left_edge = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
	gene_distance_to_left_edge = gene_distance_to_left_edge - 0.5;

	gene_distance_to_right_edge = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
	gene_distance_to_right_edge = gene_distance_to_right_edge - 0.5;

	gene_distance_to_up_body = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
	gene_distance_to_up_body = gene_distance_to_up_body - 0.5;

	gene_distance_to_down_body = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
	gene_distance_to_down_body = gene_distance_to_down_body - 0.5;

	gene_distance_to_left_body = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
	gene_distance_to_left_body = gene_distance_to_left_body - 0.5;

	gene_distance_to_right_body = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
	gene_distance_to_right_body = gene_distance_to_right_body - 0.5;
}

//Creates a game and allows the genome to make all the decisions on actions until
//the end of the game
int genome::play_game(const bool display, const int turn_limit, const int display_delay)
{
	game test_game;
	//main game loop cut off by a turn_limit
	for(int i = 0; i < turn_limit; i++)
	{
		//display the board if the function is meant to do so
		if(display)
		{
			cout << "Current Genome ID: " << id << endl;
			cout << "Current Turn: " << test_game.turn << endl;
			test_game.current_state.display();
			//Delay before displaying the next state
			this_thread::sleep_for(chrono::milliseconds(display_delay));
		}

		//Determine the best action to take from the current state
		coordinate action = optimize_action(test_game.current_state);

		//Update the game by taking the selected action
		test_game.update(action);

		//Display loss data if the state is a premature loss
		if(test_game.current_state.loss)
		{
			if(display)
			{
				cout << "Current Genome ID: " << id << endl;
				cout << "Current Turn: " << test_game.turn << endl;
				test_game.current_state.display();
			}
			return fitness(test_game.current_state, test_game.turn);
		}
	}
	//If display settings are on print the final score the genome received
	if(display)
		cout << "Final Score: " << test_game.current_state.score << endl;
	//Calculate and return the fitness of the genome's performance
	return fitness(test_game.current_state, test_game.turn);
}

//Depth limited Depth First Search of the game tree returning the action
//that maximizes the heuristic value based on the genome's genes
coordinate genome::optimize_action(state s)
{
	vector<coordinate> best_action;
	coordinate current_action;
	int best_heuristic = -10000;
	int current_heuristic;

	//Determine possible actions
	vector<coordinate> actions = s.actions();

	for(unsigned int i = 0; i < actions.size(); i++)
	{
		//Create a state from taking a possible action
		state new_state = s.result(actions[i]);
		//Determine the best heuristic value at the search depth
		current_heuristic = optimize_heuristic_at_depth(new_state, SEARCH_DEPTH);
		//If the new heuristic matches the best one found, they are both options
		//to be chosen in the return
		if(current_heuristic == best_heuristic)
		{
			best_action.push_back(actions[i]);
		}
		//If the new heuristic is the best found, the list of best options
		//is emptied to only contain the new best action
		else if(current_heuristic > best_heuristic)
		{
			best_heuristic = current_heuristic;
			best_action.clear();
			best_action.push_back(actions[i]);
		}
	}

	//Selects randomly from the best actions if there is a tie
	return best_action[rand()%best_action.size()];
}

//Recursive function performing the depth limited depth first search
//Returns the optimized heuristic value of the provided tree
int genome::optimize_heuristic_at_depth(state s, int depth)
{
	//Terminates recursion at the search depth or the end of the game
	if(depth == 0 || s.loss)
	{
		return heuristic(s);
	}
	coordinate current_action;
	int best_heuristic = -100000;
	int current_heuristic;

	//Generate possible actions
	vector<coordinate> actions = s.actions();

	for(unsigned int i = 0; i < actions.size(); i++)
	{
		//Generate a new state from a possible action
		state new_state = s.result(actions[i]);
		//Determine the heuristic value at the next search depth recursively
		current_heuristic = optimize_heuristic_at_depth(new_state, depth-1);
		//Update best heuristic if a greater value is found
		if(current_heuristic > best_heuristic)
		{
			best_heuristic = current_heuristic;
		}
	}

	return best_heuristic;
}

//adjusts the heuristic value by using the genes as weighted values
//to adjust the importance of the helper functions related to each gene
int genome::heuristic(state s)
{
	int heuristic_sum = 0;

	heuristic_sum += gene_turn_count * s.turn;

	//The score is weighted also by the max Manhattan distance on the map
	//so that a new food positions increased distance does not outweigh
	//the gain in score. Otherwise, the AI prefers to circle the food to
	//avoid replacing it.
	heuristic_sum += (s.MAP_X_LIMIT + s.MAP_Y_LIMIT) * gene_score * s.score;

	heuristic_sum += gene_distance_to_food * heur_distance_to_food(s);

	heuristic_sum += gene_distance_to_top_edge * heur_distance_to_top_edge(s);

	heuristic_sum += gene_distance_to_bottom_edge * heur_distance_to_bottom_edge(s);

	heuristic_sum += gene_distance_to_left_edge * heur_distance_to_left_edge(s);

	heuristic_sum += gene_distance_to_right_edge * heur_distance_to_right_edge(s);

	heuristic_sum += gene_distance_to_up_body * heur_distance_to_up_body(s);

	heuristic_sum += gene_distance_to_down_body * heur_distance_to_down_body(s);

	heuristic_sum += gene_distance_to_left_body * heur_distance_to_left_body(s);

	heuristic_sum += gene_distance_to_right_body * heur_distance_to_right_body(s);

	//A losing state is weighted by the product of the map limits so a loss
	//is always weighted greater than what the other genes can produce
	heuristic_sum += s.loss * -(s.MAP_X_LIMIT * s.MAP_Y_LIMIT);

	return heuristic_sum;
}

//returns the Manhattan distance from the snakes head to the food coordinate
int genome::heur_distance_to_food(state s)
{
	return abs(s.snake.back().x - s.food.x) + abs(s.snake.back().y - s.food.y);
}

//returns the linear distance to the top edge from the snake's head
int genome::heur_distance_to_top_edge(state s)
{
	return s.snake.back().y;
}

//returns the linear distance to the bottom edge from the snake's head
int genome::heur_distance_to_bottom_edge(state s)
{
	return s.MAP_Y_LIMIT - 1 - s.snake.back().y;
}

//returns the linear distance to the left edge from the snake's head
int genome::heur_distance_to_left_edge(state s)
{
	return s.snake.back().x;
}

//returns the linear distance to the right edge from the snake's head
int genome::heur_distance_to_right_edge(state s)
{
	return s.MAP_X_LIMIT - 1 - s.snake.back().x;
}

//returns the linear distance to the nearest body segment or edge searching upwards
int genome::heur_distance_to_up_body(state s)
{
	if(s.direction_modifier == coordinate(0,1))
		return 0;
	coordinate inspection = s.snake.back();
	for(int i = 1; i < inspection.y + 1; i++)
	{
		inspection.y--;
		if(s.in_snake(inspection))
			return i;
	}
	return s.snake.back().y;
}

//returns the linear distance to the nearest body segment or edge searching downwards
int genome::heur_distance_to_down_body(state s)
{
	if(s.direction_modifier == coordinate(0,-1))
		return 0;
	coordinate inspection = s.snake.back();
	for(int i = 1; i < s.MAP_Y_LIMIT - inspection.y; i++)
	{
		inspection.y++;
		if(s.in_snake(inspection))
			return i;
	}
	return s.MAP_Y_LIMIT - 1 - s.snake.back().y;
}

//returns the linear distance to the nearest body segment or edge searching leftwards
int genome::heur_distance_to_left_body(state s)
{
	if(s.direction_modifier == coordinate(1,0))
		return 0;
	coordinate inspection = s.snake.back();
	for(int i = 1; i < inspection.x + 1; i++)
	{
		inspection.x--;
		if(s.in_snake(inspection))
			return i;
	}
	return s.snake.back().x;
}

//returns the linear distance to the nearest body segment or edge searching upwards
int genome::heur_distance_to_right_body(state s)
{
	if(s.direction_modifier == coordinate(-1,0))
		return 0;
	coordinate inspection = s.snake.back();
	for(int i = 1; i < s.MAP_X_LIMIT - inspection.x; i++)
	{
		inspection.x++;
		if(s.in_snake(inspection))
			return i;
	}
	return s.MAP_X_LIMIT - 1 - s.snake.back().x;
}

//updates and returns the fitness value of a genome's performance
int genome::fitness(state s, int turn)
{
	//Score is weighted greater than the highest weighted turn value to
	//isolate each factor's influence
	const int SCORE_WEIGHT = 1000;
	//The turn weight encourages generations to survive more turns
	const int TURN_WEIGHT = 1;

	//evaluate the weighted results of a game's results
	fitness_value = s.score * SCORE_WEIGHT + turn * TURN_WEIGHT;

	return s.score * SCORE_WEIGHT + turn * TURN_WEIGHT;
}

//Displays labelled values for the genes of a genome
void genome::display()
{
	cout << "Genome ID: " << id << endl;
	cout << "Turn Count: " << gene_turn_count << endl;
	cout << "Score: " << gene_score << endl;
	cout << "To Food: " << gene_distance_to_food << endl;
	cout << "To Top Edge: " << gene_distance_to_top_edge << endl;
	cout << "To Bottom Edge: " << gene_distance_to_bottom_edge << endl;
	cout << "To Left Edge: " << gene_distance_to_left_edge << endl;
	cout << "To Right Edge: " << gene_distance_to_right_edge << endl;
	cout << "To Up Body: " << gene_distance_to_up_body << endl;
	cout << "To Down Body: " << gene_distance_to_down_body << endl;
	cout << "To Left Body: " << gene_distance_to_left_body << endl;
	cout << "To Right Body: " << gene_distance_to_right_body << endl;
	cout << "Fitness: " << fitness_value << endl;
	return;
}

//Records the gene values of a genome at the file_name provided
void genome::save_to_file(const char* file_name)
{
	ofstream file;
	file.open(file_name);
	if(file.is_open())
	{
		file << id << endl;
		file << gene_turn_count << endl;
		file << gene_score << endl;
		file << gene_distance_to_food << endl;
		file << gene_distance_to_top_edge << endl;
		file << gene_distance_to_bottom_edge << endl;
		file << gene_distance_to_left_edge << endl;
		file << gene_distance_to_right_edge << endl;
		file << gene_distance_to_up_body << endl;
		file << gene_distance_to_down_body << endl;
		file << gene_distance_to_left_body << endl;
		file << gene_distance_to_right_body << endl;
		file << fitness_value << endl;
		file.close();
		cout << "Successfully saved genome to " << file_name << endl;
	}
	else cout << "Failed to save genome to " << file_name << endl;
}

//Copies the values found in a file to the genes of a genome
void genome::load_from_file(const char* file_name)
{
	ifstream file;
	file.open(file_name);
	if(file.is_open())
	{
		file >> id;
		file >> gene_turn_count;
		file >> gene_score;
		file >> gene_distance_to_food;
		file >> gene_distance_to_top_edge;
		file >> gene_distance_to_bottom_edge;
		file >> gene_distance_to_left_edge;
		file >> gene_distance_to_right_edge;
		file >> gene_distance_to_up_body;
		file >> gene_distance_to_down_body;
		file >> gene_distance_to_left_body;
		file >> gene_distance_to_right_body;
		file >> fitness_value;
		file.close();
		cout << "Successfully loaded genome in " << file_name << endl;
	}
	else cout << "Failed to load genome in " << file_name << endl;
}

//Spawns the number of genomes in a generation's population size
//and randomizes their gene values to create the zero generation
void evolution::initialize()
{
	genome new_genome;

	for(int i = 0; i < POPULATION_SIZE; i++)
	{
		new_genome.randomize();
		new_genome.id = next_genome_id;
		next_genome_id++;
		generation.push_back(new_genome);
	}
}

//Has each genome in a generation play a game using their genes to
//influence decisions. Then the generation is sorted by their fitness
//values from those trials and the results are displayed.
void evolution::fitness_test(const bool display, const int turn_limit)
{
	for(int i = 0; i < POPULATION_SIZE; i++)
	{
		generation[i].play_game(display, turn_limit);
	}
	sort_generation();
	cout << "Generation: " << generation_number << endl << "Sorted Fitness: ";
	for(int i = 0; i < POPULATION_SIZE; i++)
	{
		cout << generation[i].fitness_value << ",";
	}
	cout << endl;
}

//Sorts the generation vector by their fitness values
void evolution::sort_generation()
{
	sort(generation.begin(), generation.end());
}

//Stores the current generation in the archive of previous generations.
//Selects the top half of the generation based on fitness.
//Selects two parents randomly
//Spawns a child from the parents
//Mutates the child and adds it to the next generation
//This process repeats until a new generation is generated and labelled
void evolution::spawn_next_generation()
{
	//Record the current generation
	previous_generations.push_back(generation);
	vector<genome> elites = generation;

	//Erase the bottom half of the genomes from the elite
	for(int i = 0; i < (POPULATION_SIZE/2); i++)
	{
		elites.erase(elites.begin());
	}
	//Clear the previous generation to make room for new children
	generation.clear();

	//Display the exact genes of the top performing genome of the generation
	//elites.back().display();

	//Select parents, cross genes, mutate genes, and store the child for each
	//new genome
	for(int i = 0; i < POPULATION_SIZE; i++)
	{
		vector<genome> parents = choose_parents(elites);
		genome child = spawn_child(parents[0],parents[1]);
		child = mutate_child(child);
		generation.push_back(child);
	}
	generation_number++;
}

//Returns a vector containing 2 genome parents to be used for child spawning
vector<genome> evolution::choose_parents(vector<genome> elites)
{
	vector<genome> parents;

	//Performance Weighted Selection
	//This selection increases the probability of a given genome's selection
	//based on the ratio of its fitness compared to the total fitness of the elite class.
	//This method gives preference to higher performers and can reduce the possibility of
	//low outlier parents that make it into the elite class from having an equal influence
	//on the next generation
	elite_probability_vector.clear();

	float fitness_total = 0;
	//Calculate the total sum of all fitness
	for(unsigned int i = 0; i < elites.size(); i++)
	{
		fitness_total += elites[i].fitness_value;
	}
	//Create a probability vector containing each genome's ratio of fitness to the elite fitness total
	for(unsigned int i = 0; i < elites.size(); i++)
	{
		elite_probability_vector.push_back(static_cast<float>(elites[i].fitness_value)/fitness_total);
	}

	//Normalize the last probability to be the range of it's ratio to 1.
	elite_probability_vector.back() = 1 - elite_probability_vector.back();

	//Shift the probability vector so each genome takes up its ratio's portion of the number scale between 0 and 1
	//This is in the format of a cumulative probability table
	for(unsigned int i = elites.size() - 2; i > 0; i--)
	{
		elite_probability_vector[i] = elite_probability_vector[i+1] - elite_probability_vector[i];
	}

	//Set the first entry to zero to complete the spectrum
	elite_probability_vector[0] = 0;

	//Linear Probability Adjustment
	//This method linearly adjusts the probability vector to favor higher performers
	//bool even_shift = false;
	/*if(elite_probability_vector.size() == 0)
	{
		float normal_probability = 1/static_cast<float>(elites.size());
		for(unsigned int i = 0; i < elites.size(); i++)
		{
			float shifted_probability = ELITE_PROBABILITY_SLOPE * (i + even_shift - (elites.size()/2)) + normal_probability;
			if(shifted_probability == normal_probability)
			{
				if(elites.size() % 2 == 0)
				{
					shifted_probability += ELITE_PROBABILITY_SLOPE;
					even_shift = true;
				}
			}
			elite_probability_vector.push_back(shifted_probability);
		}
		for(unsigned int i = 1; i < elites.size(); i++)
		{
			elite_probability_vector[i] = elite_probability_vector[i-1];
		}
		elite_probability_vector[0] = 0;
	}*/

	//uses the probability vector just calculated to select parents randomly
	//based on their given probabilities
	for(int i = 0; i < 2; i++)
		parents.push_back(elites[probability_vector_index_identify(rand())]);

	return parents;
}

//returns the index of a genome in the elite class
//The random number is shifted to be between 0 and 1
//Then the threshold that the random number passes in the
//probability vector determines the index of the the genome that
//should be selected
int evolution::probability_vector_index_identify(float random_num)
{
	random_num = random_num/static_cast<float>(RAND_MAX);
	for(unsigned int i = 0; i < elite_probability_vector.size(); i++)
	{
		if(elite_probability_vector[i] > random_num)
		{
			return i - 1;
		}
	}
	return elite_probability_vector.size() - 1;
}

//Randomly select one of the parent's genes for each gene of
//the child. Returns the newly created child.
genome evolution::spawn_child(genome parent_a, genome parent_b)
{
	genome child;
	child.fitness_value = 0;
	child.id = next_genome_id;
	next_genome_id++;

	if(rand() % 2 == 0)
	{
		child.gene_turn_count = parent_a.gene_turn_count;
	}
	else
	{
		child.gene_turn_count = parent_b.gene_turn_count;
	}

	if(rand() % 2 == 0)
	{
		child.gene_score = parent_a.gene_score;
	}
	else
	{
		child.gene_score = parent_b.gene_score;
	}

	if(rand() % 2 == 0)
	{
		child.gene_distance_to_food = parent_a.gene_distance_to_food;
	}
	else
	{
		child.gene_distance_to_food = parent_b.gene_distance_to_food;
	}

	if(rand() % 2 == 0)
	{
		child.gene_distance_to_top_edge = parent_a.gene_distance_to_top_edge;
	}
	else
	{
		child.gene_distance_to_top_edge = parent_b.gene_distance_to_top_edge;
	}

	if(rand() % 2 == 0)
	{
		child.gene_distance_to_bottom_edge = parent_a.gene_distance_to_bottom_edge;
	}
	else
	{
		child.gene_distance_to_bottom_edge = parent_b.gene_distance_to_bottom_edge;
	}

	if(rand() % 2 == 0)
	{
		child.gene_distance_to_left_edge = parent_a.gene_distance_to_left_edge;
	}
	else
	{
		child.gene_distance_to_left_edge = parent_b.gene_distance_to_left_edge;
	}

	if(rand() % 2 == 0)
	{
		child.gene_distance_to_right_edge = parent_a.gene_distance_to_right_edge;
	}
	else
	{
		child.gene_distance_to_right_edge = parent_b.gene_distance_to_right_edge;
	}

	if(rand() % 2 == 0)
	{
		child.gene_distance_to_up_body = parent_a.gene_distance_to_up_body;
	}
	else
	{
		child.gene_distance_to_up_body = parent_b.gene_distance_to_up_body;
	}

	if(rand() % 2 == 0)
	{
		child.gene_distance_to_down_body = parent_a.gene_distance_to_down_body;
	}
	else
	{
		child.gene_distance_to_down_body = parent_b.gene_distance_to_down_body;
	}

	if(rand() % 2 == 0)
	{
		child.gene_distance_to_left_body = parent_a.gene_distance_to_left_body;
	}
	else
	{
		child.gene_distance_to_left_body = parent_b.gene_distance_to_left_body;
	}

	if(rand() % 2 == 0)
	{
		child.gene_distance_to_right_body = parent_a.gene_distance_to_right_body;
	}
	else
	{
		child.gene_distance_to_right_body = parent_b.gene_distance_to_right_body;
	}


	return child;
}

//Returns the child where each gene has a mutation chance to
//adjust its value to a random value between the original value +/-
//the mutation step. If the gene is not chosen to be mutated, it is
//unchanged.
genome evolution::mutate_child(genome child)
{
	if(static_cast<float>(rand())/static_cast<float>(RAND_MAX) < MUTATION_CHANCE)
	{
		child.gene_turn_count = child.gene_turn_count + static_cast<float>(rand())/static_cast<float>(RAND_MAX) * MUTATION_STEP * 2 - MUTATION_STEP;
	}

	if(static_cast<float>(rand())/static_cast<float>(RAND_MAX) < MUTATION_CHANCE)
	{
		child.gene_score = child.gene_score + static_cast<float>(rand())/static_cast<float>(RAND_MAX) * MUTATION_STEP * 2 - MUTATION_STEP;
	}

	if(static_cast<float>(rand())/static_cast<float>(RAND_MAX) < MUTATION_CHANCE)
	{
		child.gene_distance_to_food = child.gene_distance_to_food + static_cast<float>(rand())/static_cast<float>(RAND_MAX) * MUTATION_STEP * 2 - MUTATION_STEP;
	}

	if(static_cast<float>(rand())/static_cast<float>(RAND_MAX) < MUTATION_CHANCE)
	{
		child.gene_distance_to_top_edge = child.gene_distance_to_top_edge + static_cast<float>(rand())/static_cast<float>(RAND_MAX) * MUTATION_STEP * 2 - MUTATION_STEP;
	}

	if(static_cast<float>(rand())/static_cast<float>(RAND_MAX) < MUTATION_CHANCE)
	{
		child.gene_distance_to_bottom_edge = child.gene_distance_to_bottom_edge + static_cast<float>(rand())/static_cast<float>(RAND_MAX) * MUTATION_STEP * 2 - MUTATION_STEP;
	}

	if(static_cast<float>(rand())/static_cast<float>(RAND_MAX) < MUTATION_CHANCE)
	{
		child.gene_distance_to_left_edge = child.gene_distance_to_left_edge + static_cast<float>(rand())/static_cast<float>(RAND_MAX) * MUTATION_STEP * 2 - MUTATION_STEP;
	}

	if(static_cast<float>(rand())/static_cast<float>(RAND_MAX) < MUTATION_CHANCE)
	{
		child.gene_distance_to_right_edge = child.gene_distance_to_right_edge + static_cast<float>(rand())/static_cast<float>(RAND_MAX) * MUTATION_STEP * 2 - MUTATION_STEP;
	}

	if(static_cast<float>(rand())/static_cast<float>(RAND_MAX) < MUTATION_CHANCE)
	{
		child.gene_distance_to_up_body = child.gene_distance_to_up_body + static_cast<float>(rand())/static_cast<float>(RAND_MAX) * MUTATION_STEP * 2 - MUTATION_STEP;
	}

	if(static_cast<float>(rand())/static_cast<float>(RAND_MAX) < MUTATION_CHANCE)
	{
		child.gene_distance_to_down_body = child.gene_distance_to_down_body + static_cast<float>(rand())/static_cast<float>(RAND_MAX) * MUTATION_STEP * 2 - MUTATION_STEP;
	}

	if(static_cast<float>(rand())/static_cast<float>(RAND_MAX) < MUTATION_CHANCE)
	{
		child.gene_distance_to_left_body = child.gene_distance_to_left_body + static_cast<float>(rand())/static_cast<float>(RAND_MAX) * MUTATION_STEP * 2 - MUTATION_STEP;
	}

	if(static_cast<float>(rand())/static_cast<float>(RAND_MAX) < MUTATION_CHANCE)
	{
		child.gene_distance_to_right_body = child.gene_distance_to_right_body + static_cast<float>(rand())/static_cast<float>(RAND_MAX) * MUTATION_STEP * 2 - MUTATION_STEP;
	}

	return child;
}
