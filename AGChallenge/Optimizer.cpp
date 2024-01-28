#include "Optimizer.h"

COptimizer::COptimizer(CLFLnetEvaluator& cEvaluator)
	: c_evaluator(cEvaluator)
{
	random_device c_seed_generator;
	c_rand_engine.seed(c_seed_generator());

	d_current_best_fitness = 0;

	//population.resize((size_t)POP_SIZE);
}

COptimizer::~COptimizer()
{
	for (int i = 0; i < population.size(); i++) {
		delete population.at(i);
	}
}
//COptimizer::COptimizer(CEvaluator &cEvaluator)

void COptimizer::vInitialize()
{
	d_current_best_fitness = -DBL_MAX;
	v_current_best.clear();
	
	for (int i = 0; i < POP_SIZE; i++)
	{
		population.push_back(new Individual(c_evaluator, c_rand_engine));
		population.at(i)->fillRandomly();
	}
}//void COptimizer::vInitialize()

void COptimizer::vRunIteration()
{
	vector<Individual*> new_population;

	// Perform selection, crossover, mutation, and replacement
	while (new_population.size() < POP_SIZE)
	{
		Individual* parent1 = tournament();
		Individual* parent2 = tournament();

		while (parent1->genotype == parent2->genotype)
		{
			delete parent2;
			parent2 = tournament();
		}

		// Perform crossover
		Individual* child1 = new Individual(c_evaluator, c_rand_engine);
		Individual* child2 = new Individual(c_evaluator, c_rand_engine);
		parent1->crossover(parent2, child1, child2);

		delete parent1;
		delete parent2;

		// Perform mutation on children
		child1->mutate();
		child2->mutate();

		// Add children to the new population
		new_population.push_back(child1);
		new_population.push_back(child2);
	}

	// Swap new population with the current one
	for (int i = 0; i < population.size(); i++) {
		delete population.at(i);
	}
	population.swap(new_population);

	// Update the current best solution
	for (int i = 0; i < population.size(); i++)
	{
		if (population.at(i)->getFitness() > d_current_best_fitness)
		{
			v_current_best = population.at(i)->genotype;
			d_current_best_fitness = population.at(i)->getFitness();

			cout << d_current_best_fitness << endl;
		}
	}
}

Individual* COptimizer::get_best_individual()
{
	Individual* best_individual;

	for (int i = 0; i < population.size(); i++)
	{
		if (population.at(i)->getFitness() > d_current_best_fitness)
		{
			best_individual = population.at(i);
			v_current_best = best_individual->genotype;
			d_current_best_fitness = best_individual->getFitness();
		}
	}

	return best_individual;
}

Individual* COptimizer::tournament()
{
	uniform_int_distribution<int> parent_distribution(0, population.size() - 1);

	int parent1_index = parent_distribution(c_rand_engine);
	int parent2_index = parent_distribution(c_rand_engine);

	// Ensure parent2 is different from parent1
	while (parent2_index == parent1_index)
	{
		parent2_index = parent_distribution(c_rand_engine);
	}

	double parent1_fitness = population.at(parent1_index)->getFitness();
	double parent2_fitness = population.at(parent2_index)->getFitness();

	if (parent1_fitness > parent2_fitness)
	{
		return new Individual(*population.at(parent1_index));
	}
	else
	{
		return new Individual(*population.at(parent2_index));
	}
}

void COptimizer::simpleGreedyOptimalization(Individual* optimized_individual, vector<int>& order) {
	bool at_least_one_optimized = true;

	while (at_least_one_optimized) {
		at_least_one_optimized = false;

		for (int gene_offset : order) {
			int available_gene_values = c_evaluator.iGetNumberOfValues(gene_offset);
			double best_fitness = optimized_individual->getFitness();
			int best_fitness_gene_value = optimized_individual->genotype.at(gene_offset);
			for (int i = 0; i < available_gene_values; i++) {
				optimized_individual->genotype.at(gene_offset) = i;
				double current_fitness = c_evaluator.dEvaluate(&optimized_individual->genotype);
				if (current_fitness > best_fitness) {
					best_fitness_gene_value = i;
					at_least_one_optimized = true;
				}
			}
			optimized_individual->genotype.at(gene_offset) = best_fitness_gene_value;
		}
	}
}

vector<vector<bool> > COptimizer::linkageDiscovery(Individual* base_individual, Individual* other, vector<int>& order)
{
	vector<vector<bool> > linkage_scraps;

	simpleGreedyOptimalization(base_individual, order);
	Individual* buffer_individual = base_individual;

	for (int gene_offset : order) {
		if (base_individual->genotype.at(gene_offset) != other->genotype.at(gene_offset)) {
			base_individual->genotype.at(gene_offset) = other->genotype.at(gene_offset);
			simpleGreedyOptimalization(base_individual, order);
			linkage_scraps.push_back(createScraps(buffer_individual, base_individual));
			delete base_individual;
			base_individual = buffer_individual;

		}
	}

	delete buffer_individual;
	return linkage_scraps;
}

vector<bool> COptimizer::createScraps(Individual* individual1, Individual* individual2)
{
	vector<bool> result;
	vector<int> genotype1 = individual1->genotype;
	vector<int> genotype2 = individual2->genotype;

	for (int i = 0; i < genotype1.size(); i++) {
		result.push_back(genotype1.at(i) != genotype1.at(i));
	}
}

void COptimizer::LOaFuN()
{
	Individual* new_individual = new Individual(c_evaluator, c_rand_engine);
	new_individual->fillRandomly();
	new_individual->generateRandomOrder();
	simpleGreedyOptimalization(new_individual, new_individual->order);
	population.push_back(new_individual);
	Individual* old_best = get_best_individual();

	if (population.size() == 1) {
		Individual* new_individual2 = new Individual(c_evaluator, c_rand_engine);
		new_individual2->fillRandomly();
		new_individual2->generateRandomOrder();
		simpleGreedyOptimalization(new_individual2, new_individual2->order);

		vector<vector<bool> > linkage_scraps;
		if (new_individual2->getFitness() < new_individual->getFitness()) {
			linkage_scraps = linkageDiscovery(new_individual, new_individual2, new_individual->order);
		} 
		else {
			linkage_scraps = linkageDiscovery(new_individual2, new_individual, new_individual2->order);
		}


	}
}

void COptimizer::runForLevel()
{
}


vector<vector<double>> createDSM(vector<vector<bool>>& linkage_scraps, mt19937& rand_engine)
{
	int genes_num = linkage_scraps[0].size();

	vector<vector<double> > dsm(genes_num, vector<double>(genes_num, 0));

	uniform_real_distribution<double> distribution(0.0, 0.1);

	for (vector<bool>& linkage_scrap : linkage_scraps) {
		for (int i = 0; i < linkage_scrap.size(); i++) {
			for (int j = i + 1; j < linkage_scrap.size(); j++) {
				if (linkage_scrap.at(i) && linkage_scrap.at(j)) {
					dsm[i][j]++;
					dsm[j][i]++;
				}
			}
		}
	}

	for (int i = 0; i < genes_num; i++) {
		for (int j = 0; j < genes_num; j++) {
			if (i != j) {
				double randomValue = distribution(rand_engine);
				dsm[i][j] += randomValue;
				dsm[j][i] += randomValue;
			}
		}
	}

	return dsm;
}

vector<vector<int>> findMasks(vector<vector<double>>& dsm, mt19937& rand_engine)
{
	int genes_num = dsm.size();
	vector<vector<int> > masks;
	vector<int> current_mask;
	vector<int> used_genes;

	uniform_int_distribution<int> distribution(0, genes_num - 1);
	int current_gene = distribution(rand_engine);

	used_genes.push_back(current_gene);
	current_mask.push_back(current_gene);
	masks.push_back(current_mask);

	while (used_genes.size() != genes_num)
	{
		double max_value = -DBL_MAX;
		int max_value_gene = -1;

		for (int i = 0; i < genes_num; i++)
		{
			if (dsm[current_gene][i] > max_value && find(used_genes.begin(), used_genes.end(), i) == used_genes.end())
			{
				max_value = dsm[current_gene][i];
				max_value_gene = i;
			}
		}

		if (max_value_gene != -1)
		{
			current_mask.push_back(max_value_gene);
			used_genes.push_back(max_value_gene);
		}
		else {
			return masks;
		}

		masks.push_back(current_mask);
	}

	return masks;
}