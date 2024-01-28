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
		delete population[i];
	}
}
//COptimizer::COptimizer(CEvaluator &cEvaluator)

void COptimizer::vInitialize()
{
	d_current_best_fitness = -DBL_MAX;
	v_current_best.clear();
	max_level = 1;
}//void COptimizer::vInitialize()

void COptimizer::vRunIteration()
{
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
	vector<vector<bool> > linkage_scraps;

	if (population.size() == 1) {
		Individual* new_individual2 = new Individual(c_evaluator, c_rand_engine);
		new_individual2->fillRandomly();
		new_individual2->generateRandomOrder();
		simpleGreedyOptimalization(new_individual2, new_individual2->order);

		if (new_individual2->getFitness() < new_individual->getFitness()) {
			linkage_scraps = linkageDiscovery(new_individual, new_individual2, new_individual->order);
		} 
		else {
			linkage_scraps = linkageDiscovery(new_individual2, new_individual, new_individual2->order);
		}
		linkage_set = findMasks(createDSM(linkage_scraps, c_rand_engine), c_rand_engine);
	}

	for (int i = 1; i < max_level; i++) {
		runForLevel(i, new_individual);
	}
	simpleGreedyOptimalization(new_individual, new_individual->order);
	if (old_best->getFitness() < new_individual->getFitness()) {
		linkage_scraps = linkageDiscovery(new_individual, old_best, new_individual->order);
		linkage_set = findMasks(createDSM(linkage_scraps, c_rand_engine), c_rand_engine);
	}
}

void COptimizer::runForLevel(int level, Individual* new_individual)
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