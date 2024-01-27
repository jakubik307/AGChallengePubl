#pragma once

#include "Optimizer.h"

Individual::Individual(CLFLnetEvaluator& evaluator, mt19937& rand_engine)
	:evaluator(evaluator), rand_engine(rand_engine)
{
	genotype.resize((size_t)evaluator.iGetNumberOfBits());

	fitness = -1;
	level = 1;
	update_fitness = true;
}

Individual::Individual(const Individual& other)
	: fitness(other.fitness),
	update_fitness(other.update_fitness),
	evaluator(other.evaluator),
	rand_engine(other.rand_engine),
	genotype(other.genotype),
	level(other.level),
	order(other.level)
{
}

Individual& Individual::operator=(const Individual& other)
{
	if (this != &other) // self-assignment check
	{
		fitness = other.fitness;
		update_fitness = other.update_fitness;
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
	random_shuffle(new_order.begin(), new_order.end());
	order = new_order;
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

void Individual::crossover(Individual* other_parent, Individual* child1, Individual* child2)
{
	uniform_real_distribution<double> prob_distribution(0.0, 1.0);
	double crossover_prob = prob_distribution(rand_engine);

	int crossover_point;
	
	if (crossover_prob < CROSS_PROB) {
		// Randomly select the crossover point
		uniform_int_distribution<int> crossover_point_distribution(1, genotype.size() - 1);
		crossover_point = crossover_point_distribution(rand_engine);
	}
	else {
		crossover_point = 0;
	}

	// Randomly select the crossover point
	uniform_int_distribution<int> crossover_point_distribution(1, genotype.size() - 1);
	crossover_point = crossover_point_distribution(rand_engine);

	child1->genotype.clear();
	child1->genotype.resize((size_t)evaluator.iGetNumberOfBits());
	child1->update_fitness = true;

	child2->genotype.clear();
	child2->genotype.resize((size_t)evaluator.iGetNumberOfBits());
	child2->update_fitness = true;

	// Perform crossover for child 1
	for (int i = 0; i < crossover_point; i++) {
		child1->genotype[i] = genotype[i];
	}
	for (int i = crossover_point; i < genotype.size(); i++) {
		child1->genotype[i] = other_parent->genotype[i];
	}

	// Perform crossover for child 2
	for (int i = 0; i < crossover_point; i++) {
		child2->genotype[i] = other_parent->genotype[i];
	}
	for (int i = crossover_point; i < genotype.size(); i++) {
		child2->genotype[i] = genotype[i];
	}
}