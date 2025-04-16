#include "individual.h"
#include <algorithm>
#include <random>

extern FitnessFunction FF;

namespace {
	std::unordered_map<char, std::vector<std::pair<std::string, float>>> defaultRules = {
		{'F', {{"F-F[[F-F]F[-FF]", 1.0f}}},
		{'-', {{"-", 1.0f}}},
		{'+', {{"+", 1.0f}}},
		{'[', {{"[", 1.0f}}},
		{']', {{"]", 1.0f}}}
	};

	std::random_device rd;
	std::mt19937 gen(rd());

	void BalanceBrackets(std::vector<char>& symbols) {
		int openCount = std::count(symbols.begin(), symbols.end(), '[');
		int closeCount = std::count(symbols.begin(), symbols.end(), ']');
		char missing = openCount > closeCount ? ']' : '[';
		int diff = std::abs(openCount - closeCount);
		symbols.insert(symbols.end(), diff, missing);
	}
}

Single::Single(std::string startingWord, int ruleIterations, TurtleState initialState, CheckpointPattern pattern)
	: m_StartingWord(std::move(startingWord)),
	m_RuleIterations(ruleIterations),
	m_InitialState(initialState)
{
	m_CheckpointLocations = FF.CreateCheckpoints(50, 50, pattern, 4);
	GenerateRule();
	InitialiseLsystem();
}

void Single::GenerateRule() {
	std::unordered_map<char, std::vector<std::pair<std::string, float>>> rules = {
		{'-', {{"-", 1.0f}}},
		{'+', {{"+", 1.0f}}},
		{'[', {{"[", 1.0f}}},
		{']', {{"]", 1.0f}}}
	};

	std::vector<char> symbols = { 'F', '+', '-', '[', ']', 'F', 'F' };
	while (symbols.size() < 14) {
		char next = (rand() % 2 == 0) ? 'F' : ((rand() % 2 == 0) ? '+' : '-');
		symbols.push_back(next);
	}

	BalanceBrackets(symbols);
	std::shuffle(symbols.begin(), symbols.end(), gen);

	std::string replacement(symbols.begin(), symbols.begin() + std::min((size_t)14, symbols.size()));
	if (replacement.size() < 14) {
		replacement += std::string(14 - replacement.size(), 'F');
	}

	rules['F'] = { {replacement, 1.0f} };
	m_Individual.rule = std::move(rules);
}

void Single::InitialiseLsystem() {
	LsystemGenerator LS;
	LS.Generate(m_StartingWord, m_RuleIterations, m_Individual.rule);
	m_Individual.StartingRule = m_Individual.rule;
	m_Individual.individual = LS.ReturnFinalStack();
}

void Single::MutateRule(std::vector<char> symbolSet, std::vector<int> mutationParams) {
	auto& ruleStr = m_Individual.rule['F'][0].first;

	std::uniform_int_distribution<int> charDist(0, symbolSet.size() - 1);
	std::uniform_int_distribution<int> posDist(0, std::max(0, (int)ruleStr.size() - 1));
	std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);

	int addChance = mutationParams[0];
	int deleteChance = mutationParams[1];
	int swapChance = mutationParams[2];

	float mutationChance = chanceDist(gen);
	float thresholds[3] = {
		addChance / 100.0f,
		(addChance + deleteChance) / 100.0f,
		(addChance + deleteChance + swapChance) / 100.0f
	};

	if (mutationChance < thresholds[0]) {
		ruleStr.insert(posDist(gen), 1, symbolSet[charDist(gen)]);
	}
	else if (mutationChance < thresholds[1] && !ruleStr.empty()) {
		ruleStr.erase(posDist(gen), 1);
	}
	else if (mutationChance < thresholds[2] && ruleStr.size() > 1) {
		int i = posDist(gen), j = posDist(gen);
		if (i != j) std::swap(ruleStr[i], ruleStr[j]);
	}

	UpdateIndividual();
}

void Single::MutateWord(std::vector<char> symbolSet, int expansionSize, std::vector<int> mutationParams) {
	std::vector<char> symbols;
	while (!m_Individual.individual.empty()) {
		symbols.push_back(m_Individual.individual.top());
		m_Individual.individual.pop();
	}
	std::reverse(symbols.begin(), symbols.end());

	std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
	std::uniform_int_distribution<int> posDist(0, std::max(0, (int)symbols.size() - 1));
	std::uniform_int_distribution<int> charDist(0, symbolSet.size() - 1);

	int addChance = mutationParams[0];
	int deleteChance = mutationParams[1];
	int swapChance = mutationParams[2];

	float mutationChance = chanceDist(gen);
	float thresholds[3] = {
		addChance / 100.0f,
		(addChance + deleteChance) / 100.0f,
		(addChance + deleteChance + swapChance) / 100.0f
	};

	if (mutationChance < thresholds[0]) {
		symbols.insert(symbols.begin() + posDist(gen), symbolSet[charDist(gen)]);
	}
	else if (mutationChance < thresholds[1] && !symbols.empty()) {
		symbols.erase(symbols.begin() + posDist(gen));
	}
	else if (mutationChance < thresholds[2] && symbols.size() > 1) {
		int i = posDist(gen), j = posDist(gen);
		while (i == j) j = posDist(gen);
		std::swap(symbols[i], symbols[j]);
	}
	else {
		std::vector<int> fIndices;
		for (int i = 0; i < symbols.size(); ++i)
			if (symbols[i] == 'F') fIndices.push_back(i);

		if (!fIndices.empty()) {
			std::uniform_int_distribution<int> idxDist(0, fIndices.size() - 1);
			int insertPos = fIndices[idxDist(gen)] + 1;
			for (int i = 0; i < expansionSize; ++i)
				symbols.insert(symbols.begin() + insertPos++, symbolSet[charDist(gen)]);
		}
	}

	// Rebuild stack
	for (auto it = symbols.rbegin(); it != symbols.rend(); ++it)
		m_Individual.individual.push(*it);
}

void Single::Evaluate(FitnessType chosenFitness, std::vector<std::vector<int>> constraintMatrix) {
	m_Individual.Fitness = 0;
	int baseFitness = FF.EvaluateCheckpointFitness(m_CheckpointLocations, m_InitialState, m_Individual.individual, 50, 50);

	switch (chosenFitness) {
	case AREA:
		m_Individual.Fitness = FF.AreaFunction(m_Individual.individual, m_InitialState, 50);
		break;
	case MIN_MAX_CHECKPOINTS:
		m_Individual.Fitness = FF.CheckpointDistanceFitness(m_CheckpointLocations, constraintMatrix, m_InitialState, m_Individual.individual, 50, 50) * 100
			+ FF.AreaFunction(m_Individual.individual, m_InitialState, 50);
		break;
	case CHECKPOINT_DISTANCE:
		m_Individual.Fitness = baseFitness < 2000 ? baseFitness
			: baseFitness + FF.CheckpointDistanceFitness(m_CheckpointLocations, constraintMatrix, m_InitialState, m_Individual.individual, 50, 50);
		break;
	default:
		std::cerr << "Unknown fitness type!" << std::endl;
		break;
	}
}

void Single::UpdateIndividual() {
	while (!m_Individual.individual.empty()) m_Individual.individual.pop();
	LsystemGenerator LS;
	LS.Generate(m_StartingWord, m_RuleIterations, m_Individual.rule);
	m_Individual.individual = LS.ReturnFinalStack();
}

Individual Single::GetIndividual() { return m_Individual; }
TurtleState Single::GetInitialState() { return m_InitialState; }

void Single::PrintRule() {
	for (const auto& rule : m_Individual.rule) {
		std::cout << rule.first << " -> ";
		for (const auto& replacement : rule.second)
			std::cout << "{" << replacement.first << ", " << replacement.second << "} ";
		std::cout << "\n";
	}
}

void Single::PrintLS() {
	auto temp = m_Individual.individual;
	while (!temp.empty()) {
		std::cout << temp.top();
		temp.pop();
	}
	std::cout << std::endl;
}

void Single::PrintFF() {
	std::cout << m_Individual.Fitness << std::endl;
}
