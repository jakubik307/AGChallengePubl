#pragma once

#include "Evaluator.h"
#include "Optimizer.h"

#include <random>
#include <vector>
#include <cfloat>
#include <iostream>
#include <windows.h>

const int POP_SIZE = 20;
const double CROSS_PROB = 0.8;
const double MUT_PROB = 0.0001;

class Mask
{
	friend class MaskOptimizer;

public:
	Mask(int length, mt19937& rand_engine);
	Mask(const Mask& other);
	Mask& operator=(const Mask& other);

	double updateFitness(MaskOptimizer& optimizer);
	void mutate();
	void crossover(Mask* other_parent, Mask* child1, Mask* child2);

private:
	void fillRandomly();

	int length;
	double fitness;

	mt19937& rand_engine;
	vector<bool> genotype;
};

class MaskOptimizer
{
	friend class Mask;

public:
	MaskOptimizer(COptimizer optimizer);
	~MaskOptimizer();

	void vInitialize();
	void vRunIteration();

	vector<bool>* pvGetCurrentBest() { return &v_current_best; }

private:
	COptimizer& optimizer;
	mt19937 c_rand_engine;

	double d_current_best_fitness;
	vector<bool> v_current_best;

	vector<Mask*> population;

	Mask* tournament();
};