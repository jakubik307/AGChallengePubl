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