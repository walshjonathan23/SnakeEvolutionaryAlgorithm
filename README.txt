The SnakeEvolution project was compiled using the Cygwin 
gcc/g++ 64-bit compiler on a windows machine. 

The main function is found in SnakeEvolutionaryAlgorithm.cpp

The genomeExamples folder contains two example genomes which
were the fittest member of a final generation with the
parameters in their titles. There are two comment blocks in
the SnakeEvolutionaryAlgorithm.cpp main function that contain
code segments to load the last run's best genome or one of
the two example genomes. These genomes are loaded using the
included function called load_from_file which takes the file
path to the example as a string. These examples do not require
the time investment of evolving a new set of generations.
