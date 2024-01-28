#pragma once

#include "Optimizer.h"

Individual::Individual(CLFLnetEvaluator& evaluator, mt19937& rand_engine)
	:evaluator(evaluator), rand_engine(rand_engine)
{
	genotype.resize((size_t)evaluator.iGetNumberOfBits());

	fitness = -1;
	level = 1;
}

Individual::Individual(const Individual& other)
	: fitness(other.fitness),
	evaluator(other.evaluator),
	rand_engine(other.rand_engine),
	genotype(other.genotype),
	level(other.level),
	order(other.order)
{
}

Individual& Individual::operator=(const Individual& other)
{
	if (this != &other) // self-assignment check
	{
		fitness = other.fitness;
		evaluator = other.evaluator;
		rand_engine = other.rand_engine;
		genotype = other.genotype;
		level = other.level;
		order = other.order;
	}
	return *this;
}

void Individual::fillRandomly()
{
	update_fitness = true;

	for (int i = 0; i < genotype.size(); i++)
	{
		uniform_int_distribution<int> gene_distribution(0, evaluator.iGetNumberOfValues(i) - 1);
		genotype.at(i) = gene_distribution(rand_engine);
	}
}

void Individual::generateRandomOrder()
{
	vector<int> new_order;
	for (int i = 0; i < genotype.size(); i++) {
		new_order.push_back(i);
	}
	shuffle(new_order.begin(), new_order.end(), rand_engine);
	order = new_order;
}

double Individual::updateFitness(COptimizer& optimizer)
{
	fitness = evaluator.dEvaluate(&genotype);
	if (fitness > optimizer.d_current_best_fitness) {
		optimizer.d_current_best_fitness = fitness;
		optimizer.v_current_best = genotype;
		cout << optimizer.d_current_best_fitness << endl;
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
