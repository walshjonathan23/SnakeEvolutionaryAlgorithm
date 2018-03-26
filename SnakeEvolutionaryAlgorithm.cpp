//============================================================================
// Name        : SnakeEvolutionaryAlgorithm.cpp
// Author      : Jonathan Walsh
//============================================================================

//This file contains the main function which executes the evolution of the
//genome generations.

/*
 * The SnakeEvolutionaryAlgorithm program defines an environment that can
 * play the classic game of snake. This game has a snake which moves its head
 * forward each turn with each body segment following the previous positions
 * of the head. There are pieces of food located on the map grid which increase
 * the length of the snake by one segment when the head moves onto them. The
 * game is over when the snake head collides with an edge of the map or one
 * of its own body segments. The overall score is determined by the number of
 * food pieces that the snake collects. The player controls the direction
 * of the snake's movement.
 *
 * The program also defines an evolutionary framework for evolving artificial
 * intelligence agents to learn how to play the game of snake. The first generation
 * only understands the rules of the game and composite functions that convert
 * portions of the game states into other measurements. Decisions are made randomly
 * but the generations breed the best performers to improve the overall performance
 * of later generations to move away from random move selection. With several
 * generations of selection, crossover, and mutation, the algorithm is able to
 * create AI agents that can survive many turns in the game and collect several
 * pieces of food to increase their scores.
 */

#include <iostream>
#include "game.h"
#include "evolutionaryframework.h"
using namespace std;

int main()
{
	//Note: Larger values for turn cutoffs improve genome performance
	//over time but take longer to process generations

	//The number of turns at which the fitness test cuts off
	//a genome's game
	const int TEST_TURNS = 500;
	//The number of turns the fittest genome is allowed to
	//play while the game is displayed after the last evolution
	const int END_TURNS = 1000;
	//The number of milliseconds each state of the fittest
	//genome's game is displayed for after the last evolution
	const int DISPLAY_DELAY = 100;

	//srand seeded at a constant to create deterministic testing conditions
	srand(5);

	/*//Begin by loading the fittest genome from the previous run and viewing a game
	genome test_g;
	test_g.load_from_file();
	test_g.play_game(true, END_TURNS, DISPLAY_DELAY);
	test_g.display();
	cin.ignore();
	*/

	/*//Begin by loading the fittest genome from a saved example run and viewing a game
	genome test_g;
	const char* EXAMPLE_1 = "genomeExamples/genome_1500turns_30pop_40gen.txt";
	const char* EXAMPLE_2 = "genomeExamples/genome_2000turns_30pop_20gen.txt";
	test_g.load_from_file(EXAMPLE_2);
	test_g.play_game(true, END_TURNS, DISPLAY_DELAY);
	test_g.display();
	cin.ignore();
	*/

	evolution test_e;

	test_e.initialize();

	//Spawn the number of generations and test them
	for(int i = 0; i < test_e.GENERATION_LIMIT; i++)
	{
		test_e.fitness_test(false, TEST_TURNS);
		test_e.spawn_next_generation();
	}

	//Play a game using the fittest genome from the last generation
	//And display the results
	test_e.fitness_test(false, TEST_TURNS);
	test_e.generation.back().play_game(true, END_TURNS, DISPLAY_DELAY);
	test_e.generation.back().display();

	//Save the characteristics of the fittest genome in a file
	test_e.generation.back().save_to_file();

	return 0;
}
