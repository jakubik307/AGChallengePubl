#include "Optimizer.h"

COptimizer::COptimizer(CLFLnetEvaluator& cEvaluator)
	: c_evaluator(cEvaluator)
{
	random_device c_seed_generator;
	c_rand_engine.seed(c_seed_generator());

	d_current_best_fitness = 0;

	//population.resize((size_t)POP_SIZE);
}//COptimizer::COptimizer(CEvaluator &cEvaluator)

void COptimizer::vInitialize()
{
	d_current_best_fitness = -DBL_MAX;
	v_current_best.clear();

	for (int i = 0; i < POP_SIZE; i++)
	{
		population.push_back(Individual(c_evaluator, c_rand_engine));
		population.at(i).fill_randomly();
	}
}//void COptimizer::vInitialize()

void COptimizer::vRunIteration()
{
	vector<Individual> new_population;

	// Perform selection, crossover, mutation, and replacement
	while (new_population.size() < POP_SIZE)
	{
		Individual parent1 = tournament();
		Individual parent2 = tournament();

		while (parent1.genotype == parent2.genotype)
		{
			parent2 = tournament();
		}

		// Perform crossover
		Individual child1(c_evaluator, c_rand_engine);
		Individual child2(c_evaluator, c_rand_engine);
		parent1.crossover(parent2, child1, child2);

		uniform_real_distribution<double> mutation_distibution(0.0, 1.0);
		double mutation_prob;

		mutation_prob = mutation_distibution(c_rand_engine);
		if (mutation_prob < MUT_PROB) {
			vector<Individual> child1_mutation = child1.modification_mutate();
			for (int i = 0; i < child1_mutation.size(); i++) {
				new_population.push_back(child1_mutation.at(i));
			}
		}
		
		mutation_prob = mutation_distibution(c_rand_engine);
		if (mutation_prob < MUT_PROB) {
			vector<Individual> child2_mutation = child2.modification_mutate();
			for (int i = 0; i < child2_mutation.size(); i++) {
				new_population.push_back(child2_mutation.at(i));
			}
		}
	}

	// Use move semantics to transfer ownership of the new population
	population = move(new_population);

	// Update the current best solution
	for (int i = 0; i < population.size(); i++)
	{
		if (population.at(i).getFitness() > d_current_best_fitness)
		{
			v_current_best = population.at(i).genotype;
			d_current_best_fitness = population.at(i).getFitness();

			cout << d_current_best_fitness << endl;
		}
	}
}

void COptimizer::vRunAlgorithm()
{
	int current_iteration = 0;

	while (current_iteration++ < ITERATIONS)
	{
		vRunIteration();
	}
}

Individual COptimizer::tournament()
{
	uniform_int_distribution<int> parent_distribution(0, population.size() - 1);

	int parent1_index = parent_distribution(c_rand_engine);
	int parent2_index = parent_distribution(c_rand_engine);

	// Ensure parent2 is different from parent1
	while (parent2_index == parent1_index)
	{
		parent2_index = parent_distribution(c_rand_engine);
	}

	double parent1_fitness = population.at(parent1_index).getFitness();
	double parent2_fitness = population.at(parent2_index).getFitness();

	if (parent1_fitness > parent2_fitness)
	{
		return population.at(parent1_index);
	}
	else
	{
		return population.at(parent2_index);
	}
}
