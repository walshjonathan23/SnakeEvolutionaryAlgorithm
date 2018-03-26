/*
 * evolutionaryframework.h
 * This file contains the header information for the genome and evolution
 * classes related to the AI agents and their improvement
 */

#include <iostream>
#include "game.h"
using namespace std;

#ifndef EVOLUTIONARYFRAMEWORK_H_
#define EVOLUTIONARYFRAMEWORK_H_

//A genome contains several genes and is evolved over time
class genome
{
	public:
		//The search depth is how many turns ahead a genome can look
		//while playing a game
		static const int SEARCH_DEPTH = 2;

		//These are the characteristics that the genome uses to
		//make decisions in a game

		//Each gene is a weight value which provides a positive weight
		//to increase the reward a factor provides the heuristic and
		//a negative weight to increase the penalty a factor provides.
		int id;
		//The number of turns passed
		float gene_turn_count;
		//The score (number of food tiles collected)
		float gene_score;
		//The Manhattan Distance from the snake head to the next food tile
		float gene_distance_to_food;
		//The linear distance from the snake head to the top edge of the map
		float gene_distance_to_top_edge;
		//The linear distance from the snake head to the bottom edge of the map
		float gene_distance_to_bottom_edge;
		//The linear distance from the snake head to the left edge of the map
		float gene_distance_to_left_edge;
		//The linear distance from the snake head to the right edge of the map
		float gene_distance_to_right_edge;
		//The linear distance from the snake head to the nearest body segment or edge searching upwards
		float gene_distance_to_up_body;
		//The linear distance from the snake head to the nearest body segment or edge searching downwards
		float gene_distance_to_down_body;
		//The linear distance from the snake head to the nearest body segment or edge searching leftwards
		float gene_distance_to_left_body;
		//The linear distance from the snake head to the nearest body segment or edge searching rightwards
		float gene_distance_to_right_body;

		//The fitness of the genome
		int fitness_value = 0;

		//Function to initialize a random genome
		void randomize();

		//Functions to play a game and select an action
		int play_game(const bool display = false, const int turn_limit = 500, const int display_delay = 0);
		coordinate optimize_action(state s);
		int optimize_heuristic_at_depth(state s, int depth);
		int heuristic(state s);

		//Evaluates the effectiveness of the genome after playing a game
		int fitness(state s, int turn);

		//Prints a genome's genes to the standard output
		void display();

		//Gene calculation helper functions
		int heur_distance_to_food(state s);
		int heur_distance_to_top_edge(state s);
		int heur_distance_to_bottom_edge(state s);
		int heur_distance_to_left_edge(state s);
		int heur_distance_to_right_edge(state s);
		int heur_distance_to_up_body(state s);
		int heur_distance_to_down_body(state s);
		int heur_distance_to_left_body(state s);
		int heur_distance_to_right_body(state s);

		//Long Term Storage Saving/Loading
		void save_to_file(const char* file_name = "last_best_genome.txt");
		void load_from_file(const char* file_name = "last_best_genome.txt");
};

//The evolution class maintains and updates generations of genomes
//to evolve them over time
class evolution
{
	public:
		//Fixed values to influence how extensive the evolution process is
		const int POPULATION_SIZE = 30;
		const int GENERATION_LIMIT = 40;
		//Mutation chance determines the percentage chance that a new gene is mutated
		const float MUTATION_CHANCE = 0.05;
		//The mutation step determines the range a mutation can take from the
		//original value +/- the step
		const float MUTATION_STEP = 0.2;
		//Determines the slope of the probability vector linear adjustment if selected
		//as the method of vector creation
		const float ELITE_PROBABILITY_SLOPE = 0.01;

		//Labels and containers for generation storage
		int next_genome_id = 0;
		int generation_number = 0;
		vector<genome> generation;
		vector<vector<genome>> previous_generations;

		//A vector which stores the cumulative representation of the probabilities
		//for selecting parents from the elites of a given generation. It is updated
		//in the choose_parents function.
		vector<float> elite_probability_vector;

		//Functions for testing genome fitness
		void initialize();
		void fitness_test(const bool display = false, const int turn_limit = 500);

		//Functions using the results of fitness testing to determine the evolution
		//of the next generation from the best previous genomes.
		void sort_generation();
		void spawn_next_generation();
		vector<genome> choose_parents(vector<genome> elites);
		int probability_vector_index_identify(float random_num);
		genome spawn_child(genome parent_a, genome parent_b);
		genome mutate_child(genome child);
};

//Determines that sorting genome should occur based on fitness values
inline bool operator<(genome a, genome b){return a.fitness_value < b.fitness_value;}

#endif /* EVOLUTIONARYFRAMEWORK_H_ */
