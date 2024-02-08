#include "MaskOptimizer.h"

MaskOptimizer::MaskOptimizer(COptimizer optimizer)
	:optimizer(optimizer)
{
	random_device c_seed_generator;
	c_rand_engine.seed(c_seed_generator());

	d_current_best_fitness = 0;
}

MaskOptimizer::~MaskOptimizer()
{
	for (int i = 0; i < population.size(); i++) {
		delete population[i];
	}
}

void MaskOptimizer::vInitialize()
{
	d_current_best_fitness = -DBL_MAX;
	v_current_best.clear();

	for (int i = 0; i < POP_SIZE; i++)
	{
		population.push_back(new Mask(optimizer.c_evaluator.iGetNumberOfBits(), c_rand_engine));
		population[i]->fillRandomly();
	}
}

void MaskOptimizer::vRunIteration()
{
	vector<Mask*> new_population;

	// Perform selection, crossover, mutation, and replacement
	while (new_population.size() < POP_SIZE)
	{
		Mask* parent1 = tournament();
		Mask* parent2 = tournament();

		while (parent1->genotype == parent2->genotype)
		{
			delete parent2;
			parent2 = tournament();
		}

		// Perform crossover
		Mask* child1 = new Mask(*parent1);
		Mask* child2 = new Mask(*parent2);
		parent1->crossover(parent2, child1, child2);

		delete parent1;
		delete parent2;

		// Perform mutation on children
		child1->mutate();
		child2->mutate();

		// Add children to the new population
		new_population.push_back(child1);
		new_population.push_back(child2);
	}

	// Swap new population with the current one
	for (int i = 0; i < population.size(); i++) {
		delete population[i];
	}
	population.swap(new_population);

	// Set size to the constant value
	while (population.size() > POP_SIZE) {
		delete population[population.size() - 1];
		population.pop_back();
	}

	// Update the current best solution
#pragma omp parallel for
	for (int i = 0; i < population.size(); i++)
	{
		double fitness = population[i]->updateFitness(*this);
	}
}

Mask* MaskOptimizer::tournament()
{
	uniform_int_distribution<int> parent_distribution(0, population.size() - 1);

	int parent1_index = parent_distribution(c_rand_engine);
	int parent2_index = parent_distribution(c_rand_engine);

	// Ensure parent2 is different from parent1
	while (parent2_index == parent1_index)
	{
		parent2_index = parent_distribution(c_rand_engine);
	}

	double parent1_fitness = population[parent1_index]->updateFitness(*this);
	double parent2_fitness = population[parent2_index]->updateFitness(*this);

	if (parent1_fitness > parent2_fitness)
	{
		return new Mask(*population[parent1_index]);
	}
	else
	{
		return new Mask(*population[parent2_index]);
	}
}


