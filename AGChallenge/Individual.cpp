#pragma once

#include "Optimizer.h"

Individual::Individual(CLFLnetEvaluator& evaluator, mt19937& rand_engine)
	:evaluator(evaluator), rand_engine(rand_engine)
{
	genotype.resize((size_t)evaluator.iGetNumberOfBits());

	fitness = -1;
	update_fitness = true;
}

void Individual::fill_randomly()
{
	update_fitness = true;

	for (int i = 0; i < genotype.size(); i++)
	{
		uniform_int_distribution<int> gene_distribution(0, evaluator.iGetNumberOfValues(i) - 1);
		genotype.at(i) = gene_distribution(rand_engine);
	}
}

double Individual::getFitness()
{
	if (update_fitness)
	{
		update_fitness = false;
		fitness = evaluator.dEvaluate(&genotype);
	}
	return fitness;
}

void Individual::mutate()
{
	update_fitness = true;

	uniform_real_distribution<double> prob_distribution(0.0, 1.0);

	for (int i = 0; i < genotype.size(); ++i)
	{
		double mutation_prob = prob_distribution(rand_engine);

		if (mutation_prob < MUT_PROB)
		{
			uniform_int_distribution<int> gene_distribution(0, evaluator.iGetNumberOfValues(i) - 1);
			int mutated_value = gene_distribution(rand_engine);

			genotype[i] = mutated_value;
		}
	}
}

void Individual::crossover(Individual& other_parent, Individual& child1, Individual& child2)
{
	uniform_real_distribution<double> distribution(0.0, 1.0);

	// Randomly select the crossover point
	uniform_int_distribution<int> crossover_point_distribution(1, genotype.size() - 1);
	int crossover_point = crossover_point_distribution(rand_engine);

	child1.genotype.clear();
	child1.genotype.resize((size_t)evaluator.iGetNumberOfBits());

	child2.genotype.clear();
	child2.genotype.resize((size_t)evaluator.iGetNumberOfBits());

	// Perform crossover for child 1
	for (int i = 0; i < crossover_point; i++) {
		child1.genotype[i] = genotype[i];
	}
	for (int i = crossover_point; i < genotype.size(); i++) {
		child1.genotype[i] = other_parent.genotype[i];
	}

	// Perform crossover for child 2
	for (int i = 0; i < crossover_point; i++) {
		child2.genotype[i] = other_parent.genotype[i];
	}
	for (int i = crossover_point; i < genotype.size(); i++) {
		child2.genotype[i] = genotype[i];
	}
}
