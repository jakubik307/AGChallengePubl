#pragma once

#include "Evaluator.h"

#include <random>
#include <vector>
#include <cfloat>
#include <iostream>
#include <windows.h>

using namespace std;

const int ITERATIONS = 10000;

const int POP_SIZE = 20;
const double CROSS_PROB = 0.1;
const double MUT_PROB = 0.005;

class Individual
{
	friend class COptimizer;

public:
	Individual(CLFLnetEvaluator& evaluator, mt19937& rand_engine);

	double getFitness();
	void mutate();
	void crossover(Individual& other_parent, Individual& child1, Individual& child2);

private:
	void fill_randomly();

	double fitness;
	bool update_fitness;

	CLFLnetEvaluator& evaluator;
	mt19937& rand_engine;
	vector<int> genotype;
};

class COptimizer
{
public:
	COptimizer(CLFLnetEvaluator& cEvaluator);

	void vInitialize();
	void vRunIteration();
	void vRunAlgorithm();

	vector<int>* pvGetCurrentBest() { return &v_current_best; }

private:
	CLFLnetEvaluator& c_evaluator;

	double d_current_best_fitness;
	vector<int> v_current_best;

	mt19937 c_rand_engine;

	vector<Individual> population;
};//class COptimizer