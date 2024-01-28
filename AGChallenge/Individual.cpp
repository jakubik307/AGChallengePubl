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

double Individual::updateFitness(COptimizer& optimizer) {
	// Check if the genotype is already in the cache
	unordered_map<vector<int>, double>::iterator cacheIt = optimizer.fitnessCache.find(genotype);
	if (cacheIt != optimizer.fitnessCache.end()) {
		// If found, return the cached fitness value
		fitness = cacheIt->second;
	}
	else {
		// If not found, calculate the fitness and store in the cache
		fitness = evaluator.dEvaluate(&genotype);
		optimizer.fitnessCache[genotype] = fitness;

		if (fitness > optimizer.d_current_best_fitness) {
			optimizer.d_current_best_fitness = fitness;
			optimizer.v_current_best = genotype;
			std::cout << optimizer.d_current_best_fitness << std::endl;
		}
	}

	return fitness;
}
