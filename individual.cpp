#include "individual.h"

extern FitnessFunction FF;

std::unordered_map<char, std::vector<std::pair<std::string, float>>> testRules =
{
	{'F', {{"F-F[[F-F]F[-FF]", 1.0}}},
	{'-', {{"-", 1.0}}},
	{'+', {{"+", 1.0}}},
	{'[', {{"[", 1.0}}},
	{']', {{"]", 1.0}}}
};




Single::Single(std::string startingWord, int ruleIterations, TurtleState initialState, CheckpointPattern pattern)
{
	m_StartingWord = startingWord;
	m_RuleIterations = ruleIterations;
	m_InitialState = initialState;

	int numCheckpoints = 4;
	m_CheckpointLocations = FF.CreateCheckpoints(50, 50,pattern,numCheckpoints);


	GenerateRule();
	InitialiseLsystem();


}


void Single::GenerateRule()
{
	std::unordered_map<char, std::vector<std::pair<std::string, float>>> rules;


	rules['-'] = { {"-", 1.0f} };
	rules['+'] = { {"+", 1.0f} };
	rules['['] = { {"[", 1.0f} };
	rules[']'] = { {"]", 1.0f} };

	std::random_device rd;
	std::mt19937 gen(rd());

	std::string replacement;


	std::vector<char> symbols = { 'F', '+', '-', '[', ']', 'F', 'F' };


	while (symbols.size() < 14)
	{
		symbols.push_back(rand() % 2 == 0 ? 'F' : (rand() % 2 == 0 ? '+' : '-'));
	}


	std::vector<char> openBrackets, closeBrackets;
	for (char c : symbols)
	{
		if (c == '[')
		{
			openBrackets.push_back(c);
		}
		else if (c == ']')
		{
			closeBrackets.push_back(c);
		}
	}


	if (openBrackets.size() > closeBrackets.size())
	{
		int diff = openBrackets.size() - closeBrackets.size();
		for (int i = 0; i < diff; ++i)
		{
			symbols.push_back(']');
		}
	}
	else if (closeBrackets.size() > openBrackets.size())
	{
		int diff = closeBrackets.size() - openBrackets.size();
		for (int i = 0; i < diff; ++i)
		{
			symbols.push_back('[');
		}
	}


	std::shuffle(symbols.begin(), symbols.end(), gen);

	for (char c : symbols)
	{
		replacement += c;
	}


	if (replacement.size() > 14)
	{
		replacement = replacement.substr(0, 14);
	}
	else if (replacement.size() < 14)
	{
		replacement += std::string(14 - replacement.size(), 'F');
	}


	rules['F'] = { {replacement, 1.0f} };

	m_Individual.rule = rules;
	//m_Individual.rule = testRules;

}




void Single::InitialiseLsystem()
{
	LsystemGenerator LS = LsystemGenerator();

	LS.Generate(m_StartingWord, m_RuleIterations, m_Individual.rule);
	m_Individual.StartingRule = m_Individual.rule;
	std::stack<char> finalStack = LS.ReturnFinalStack();
	m_Individual.individual = finalStack;

	
}

// idea: swap two neighbouring symbols

void Single::MutateRule(std::vector<char> symbolSet, std::vector<int> mutationParams)
{
	std::vector<char> options = symbolSet;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> charDist(0, options.size() - 1);
	std::uniform_int_distribution<int> posDist(0, m_Individual.rule['F'][0].first.size());
	std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);


	int addChance = mutationParams[0];  
	int deleteChance = mutationParams[1];  
	int swapChance = mutationParams[2];  


	float addRange = addChance / 100.0f;
	float deleteRange = addRange + (deleteChance / 100.0f);
	float swapRange = deleteRange + (swapChance / 100.0f);


	float mutationChance = chanceDist(gen);

	// addition
	if (mutationChance < addRange) 
	{
		char randomChar = options[charDist(gen)];
		int randomPos = posDist(gen);
		m_Individual.rule['F'][0].first.insert(randomPos, 1, randomChar);
		//std::cout << "Inserted '" << randomChar << "' at position " << randomPos << std::endl;
	}
	else if (mutationChance < deleteRange && !m_Individual.rule['F'][0].first.empty()) 
	{
		// deletion
		int randomPos = posDist(gen);
		char deletedChar = m_Individual.rule['F'][0].first[randomPos];
		m_Individual.rule['F'][0].first.erase(randomPos, 1);
		//std::cout << "Deleted '" << deletedChar << "' from position " << randomPos << std::endl;
	}
	else if (mutationChance < swapRange && m_Individual.rule['F'][0].first.size() > 1) 
	{
		// Swap mutaion
		int size = m_Individual.rule['F'][0].first.size();

		int pos1 = posDist(gen) % size;  
		int pos2 = posDist(gen) % size;  


		if (pos1 >= 0 && pos1 < size && pos2 >= 0 && pos2 < size) 
		{
			

			if (pos1 != pos2) 
			{
				std::swap(m_Individual.rule['F'][0].first[pos1], m_Individual.rule['F'][0].first[pos2]);

			}

		}
		else 
		{
			std::cout << "Invalid positions: pos1 = " << pos1 << ", pos2 = " << pos2 << std::endl;
		}
	}


	UpdateIndividual();
}



// add deletion and swapping
void Single::MutateWord(std::vector<char> symbolSet, int expansionSize, std::vector<int> mutationParams)
{
	std::stack<char> tempStack = m_Individual.individual;
	std::vector<char> symbols;

	// Convert stack to vector
	while (!tempStack.empty())
	{
		symbols.push_back(tempStack.top());
		tempStack.pop();
	}
	std::reverse(symbols.begin(), symbols.end());

	// Setup random generators
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
	std::uniform_int_distribution<int> posDist(0, std::max(0, (int)symbols.size() - 1));
	std::uniform_int_distribution<int> charDist(0, symbolSet.size() - 1);

	// Mutation chance settings (percentages)
	int addChance = mutationParams[0];
	int deleteChance = mutationParams[1];
	int swapChance = mutationParams[2];

	float addRange = addChance / 100.0f;
	float deleteRange = addRange + (deleteChance / 100.0f);
	float swapRange = deleteRange + (swapChance / 100.0f);

	float mutationChance = chanceDist(gen);

	if (mutationChance < addRange)
	{
		// ADDITION
		char randomChar = symbolSet[charDist(gen)];
		int randomPos = posDist(gen);
		symbols.insert(symbols.begin() + randomPos, randomChar);
	}
	else if (mutationChance < deleteRange && !symbols.empty())
	{
		// DELETION
		int randomPos = posDist(gen);
		symbols.erase(symbols.begin() + randomPos);
	}
	else if (mutationChance < swapRange && symbols.size() > 1)
	{
		// SWAPPING
		int pos1 = posDist(gen);
		int pos2 = posDist(gen);
		while (pos2 == pos1) pos2 = posDist(gen); // ensure distinct

		std::swap(symbols[pos1], symbols[pos2]);
	}
	else
	{
		// If no mutation applied, apply fallback expansion logic
		std::vector<int> fIndices;
		for (int i = 0; i < symbols.size(); ++i)
			if (symbols[i] == 'F')
				fIndices.push_back(i);

		if (!fIndices.empty())
		{
			std::uniform_int_distribution<> dist(0, fIndices.size() - 1);
			int randomIndex = fIndices[dist(gen)];

			std::vector<char> randomExpansion;
			for (int i = 0; i < expansionSize; ++i)
			{
				std::uniform_int_distribution<> symbolDist(0, symbolSet.size() - 1);
				randomExpansion.push_back(symbolSet[symbolDist(gen)]);
			}

			symbols.insert(symbols.begin() + randomIndex + 1, randomExpansion.begin(), randomExpansion.end());
		}
	}

	// Push modified vector back into the stack
	while (!m_Individual.individual.empty())
		m_Individual.individual.pop();

	for (auto it = symbols.rbegin(); it != symbols.rend(); ++it)
		m_Individual.individual.push(*it);
}



void Single::Evaluate(FitnessType chosenFitness, std::vector<std::vector<int>> constraintMatrix)
{
	m_Individual.Fitness = 0;

	int preFitness = FF.EvaluateCheckpointFitness(m_CheckpointLocations, m_InitialState, m_Individual.individual, 50, 50);

	switch (chosenFitness)
	{
	case AREA:
		m_Individual.Fitness = FF.AreaFunction(m_Individual.individual, m_InitialState, 50);
		break;
	case MIN_MAX_CHECKPOINTS:
		m_Individual.Fitness = (FF.CheckpointDistanceFitness(m_CheckpointLocations, constraintMatrix, m_InitialState, m_Individual.individual, 50, 50)) * 100 + FF.AreaFunction(m_Individual.individual, m_InitialState, 50);
		break;
	case CHECKPOINT_DISTANCE:

		if (preFitness < 2000)
		{
			m_Individual.Fitness = preFitness;
			break;
		}
		else
		{
			m_Individual.Fitness = preFitness +(FF.CheckpointDistanceFitness(m_CheckpointLocations, constraintMatrix, m_InitialState, m_Individual.individual, 50, 50));
			break;
		}
		break;
	default:
		std::cerr << "Unknown fitness type!" << std::endl;
		break;
	}
}

void Single::UpdateIndividual()
{

	while (!m_Individual.individual.empty())
	{
		m_Individual.individual.pop();
	}

	LsystemGenerator LS = LsystemGenerator();

	LS.Generate(m_StartingWord, m_RuleIterations, m_Individual.rule);


	std::stack<char> finalStack = LS.ReturnFinalStack(); 
	m_Individual.individual = finalStack;


}

Individual Single::GetIndividual() { return m_Individual; }
TurtleState Single::GetInitialState() { return m_InitialState; }


void Single::PrintRule()
{

	for (const auto& rule : m_Individual.rule)
	{
		std::cout << rule.first << " -> ";
		for (const auto& replacement : rule.second)
		{
			std::cout << "{" << replacement.first << ", " << replacement.second << "} ";
		}
		std::cout << std::endl;
	}

}
void Single::PrintLS()
{
	std::stack<char> temp = m_Individual.individual;
	while (!temp.empty())
	{
		std::cout << temp.top();
		temp.pop();
	}
	std::cout << std::endl;


}
void Single::PrintFF()
{

	std::cout << m_Individual.Fitness << std::endl;

}











