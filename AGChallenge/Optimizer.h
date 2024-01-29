#pragma once

#include "Evaluator.h"

#include <random>
#include <vector>
#include <cfloat>
#include <unordered_map>
#include <iostream>
#include <windows.h>
#include <algorithm>

using namespace std;

const int POP_SIZE = 100;
const double CROSS_PROB = 0.6;
const double MUT_PROB = 0.0001;
const int GREEDY_INDIVIDUALS = 10;
const int CACHE_MAX_LIVETIME = 3;

template <>
struct hash<std::vector<int>> {
	size_t operator()(const std::vector<int>& vec) const {
		size_t hash = 0;
		for (int i : vec) {
			// Combine the hash with each element in the vector
			hash ^= std::hash<int>{}(i)+0x9e3779b9 + (hash << 6) + (hash >> 2);
		}
		return hash;
	}
};

class Individual
{
	friend class COptimizer;

public:
	Individual(CLFLnetEvaluator& evaluator, mt19937& rand_engine);
	Individual(const Individual& other);
	
	Individual& operator=(const Individual& other);

	double updateFitness(COptimizer& optimizer, int eval_index);
	void mutate();
	void crossover(Individual* other_parent, Individual* child1, Individual* child2);

private:
	void fillRandomly();

	double fitness;

	CLFLnetEvaluator& evaluator;
	mt19937& rand_engine;
	vector<int> genotype;
};

class COptimizer
{
	friend class Individual;

public:
	COptimizer(CLFLnetEvaluator& cEvaluator);
	~COptimizer();

	void vInitialize();
	void vRunIteration();

	vector<int>* pvGetCurrentBest() { return &v_current_best; }

private:
	CLFLnetEvaluator& c_evaluator;
	mt19937 c_rand_engine;

	double d_current_best_fitness;
	vector<int> v_current_best;

	vector<CLFLnetEvaluator*> evaluators;

	vector<Individual*> population;
	unordered_map<vector<int>, double> fitnessCache;

	Individual* tournament();
	void simpleGreedyOptimization(Individual* optimized_individual, int eval_index);
	vector<int> generateRandomOrder();
};//class COptimizer
