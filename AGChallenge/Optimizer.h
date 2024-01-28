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

const int MAX_LINKAGE_COLLECTED = 25;

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

	double updateFitness(COptimizer& optimizer);

private:
	void fillRandomly();
	void generateRandomOrder();

	double fitness;
	bool update_fitness;
	int level;
	vector<int> order;

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

	unordered_map<vector<int>, double> fitnessCache;

	double d_current_best_fitness;
	vector<int> v_current_best;
	Individual* get_best_individual();

	vector<Individual*> population;
	vector<vector<int> > linkage_set;
	int max_level;
	

	void simpleGreedyOptimization(Individual* optimized_individual, vector<int>& order);
	vector<vector<bool> > linkageDiscovery(Individual* base_individual, Individual* other, vector<int>& order);
	vector<bool> createScraps(Individual* individual1, Individual* individual2);
	void LOaFuN();
	void runForLevel(int level, Individual* individual);
	void optimalMixing(Individual* individual, Individual* donor);

	//DSM
	vector<vector<double> > createDSM(vector<vector<bool> >& linkage_scraps);
	vector<vector<int> > findMasks(vector<vector<double> >& dsm);
};//class COptimizer

