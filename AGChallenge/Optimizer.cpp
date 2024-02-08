#include "Optimizer.h"

COptimizer::COptimizer(CLFLnetEvaluator& cEvaluator)
	: c_evaluator(cEvaluator)
{
	random_device c_seed_generator;
	c_rand_engine.seed(c_seed_generator());

	d_current_best_fitness = 0;

	//population.resize((size_t)POP_SIZE);
}//COptimizer::COptimizer(CEvaluator &cEvaluator)

COptimizer::~COptimizer()
{
	for (int i = 0; i < population.size(); i++) {
		delete population[i];
		delete evaluators[i];
	}
}
//COptimizer::COptimizer(CEvaluator &cEvaluator)

void COptimizer::vInitialize()
{
	d_current_best_fitness = -DBL_MAX;
	v_current_best.clear();

	for (int i = 0; i < POP_SIZE; i++)
	{
		population.push_back(new Individual(c_evaluator, c_rand_engine));
		population[i]->fillRandomly();

		evaluators.push_back(new CLFLnetEvaluator());
		evaluators[i]->bConfigure(c_evaluator.sGetNetName());
	}
}//void COptimizer::vInitialize()

void COptimizer::vRunIteration()
{
	vector<Individual*> new_population;

	// Perform selection, crossover, mutation, and replacement
	while (new_population.size() < POP_SIZE)
	{
		Individual* parent1 = tournament();
		Individual* parent2 = tournament();

		while (parent1->genotype == parent2->genotype)
		{
			delete parent2;
			parent2 = tournament();
		}

		// Perform crossover
		Individual* child1 = new Individual(*parent1);
		Individual* child2 = new Individual(*parent2);
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
		double fitness = population[i]->updateFitness(*this, i);
	}
}


Individual* COptimizer::tournament()
{
	uniform_int_distribution<int> parent_distribution(0, population.size() - 1);

	int parent1_index = parent_distribution(c_rand_engine);
	int parent2_index = parent_distribution(c_rand_engine);

	// Ensure parent2 is different from parent1
	while (parent2_index == parent1_index)
	{
		parent2_index = parent_distribution(c_rand_engine);
	}

	double parent1_fitness = population[parent1_index]->updateFitness(*this, 0);
	double parent2_fitness = population[parent2_index]->updateFitness(*this, 1);

	if (parent1_fitness > parent2_fitness)
	{
		return new Individual(*population[parent1_index]);
	}
	else
	{
		return new Individual(*population[parent2_index]);
	}
}
