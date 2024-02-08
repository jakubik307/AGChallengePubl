#include "MaskOptimizer.h"

Mask::Mask(int length, mt19937& rand_engine)
	:length(length), rand_engine(rand_engine)
{
	genotype.resize(length);
	fitness = -1;
}

Mask::Mask(const Mask& other)
	:fitness(other.fitness),
	rand_engine(other.rand_engine),
	genotype(other.genotype)
{
}

Mask& Mask::operator=(const Mask& other)
{
	if (this != &other)
	{
		fitness = other.fitness;
		rand_engine = other.rand_engine;
		genotype = other.genotype;
	}
	return *this;
}

double Mask::updateFitness(MaskOptimizer& optimizer)
{
	//TODO

	if (fitness > optimizer.d_current_best_fitness) {
#pragma omp critical 
		{
			optimizer.d_current_best_fitness = fitness;
			optimizer.v_current_best = genotype;
			cout << optimizer.d_current_best_fitness << endl;
		}
	}

	return fitness;
}

void Mask::mutate()
{
	uniform_real_distribution<double> prob_distribution(0.0, 1.0);

	for (int i = 0; i < length; ++i)
	{
		double mutation_prob = prob_distribution(rand_engine);

		if (mutation_prob < MUT_PROB)
		{
			uniform_int_distribution<int> gene_distribution(0, 1);
			int mutated_value = gene_distribution(rand_engine);

			genotype[i] = mutated_value;
		}
	}
}

void Mask::crossover(Mask* other_parent, Mask* child1, Mask* child2)
{
	uniform_real_distribution<double> prob_distribution(0.0, 1.0);
	double crossover_prob = prob_distribution(rand_engine);

	vector<bool> crossover_mask(length, false);

	if (crossover_prob < CROSS_PROB) {
		uniform_int_distribution<int> mask_length_distribution(1, length - 1);
		int mask_length = mask_length_distribution(rand_engine);

		for (int i = 0; i < mask_length; i++) {
			crossover_mask[i] = true;
		}

		shuffle(crossover_mask.begin(), crossover_mask.end(), rand_engine);
	}

	child1->genotype.clear();
	child1->genotype.resize(length);

	child2->genotype.clear();
	child2->genotype.resize(length);

	// Perform crossover for child 1
	for (int i = 0; i < length; i++) {
		child1->genotype[i] = crossover_mask[i] ? genotype[i] : other_parent->genotype[i];
	}

	// Perform crossover for child 2
	for (int i = 0; i < length; i++) {
		child2->genotype[i] = crossover_mask[i] ? other_parent->genotype[i] : genotype[i];
	}
}

void Mask::fillRandomly()
{
	for (int i = 0; i < length; i++)
	{
		uniform_int_distribution<int> gene_distribution(0, 1);
		genotype.at(i) = gene_distribution(rand_engine);
	}
}
