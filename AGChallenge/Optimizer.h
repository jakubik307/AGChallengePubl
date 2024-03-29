#pragma once

#include "Evaluator.h"

#include <random>
#include <vector>
#include <cfloat>
#include <iostream>
#include <windows.h>
#include <algorithm>

using namespace std;

const int POP_SIZE = 200;
const double CROSS_PROB = 0.9;
const double MUT_PROB = 0.001;
const int GREEDY_INDIVIDUALS = 10;
const int GREEDY_OPTIMIZATION = 1000;

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

	int greedy_counter;

	double d_current_best_fitness;
	vector<int> v_current_best;

	vector<CLFLnetEvaluator*> evaluators;

	vector<Individual*> population;

	Individual* tournament();
	void simpleGreedyOptimization(Individual* optimized_individual, int eval_index);
	vector<int> generateRandomOrder();
};//class COptimizer
