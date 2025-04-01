#include "individual.h"


//extern RandomLsystem RL;
extern FitnessFunction FF;

std::unordered_map<char, std::vector<std::pair<std::string, float>>> testRules =
{
	{'F', {{"F+F[+FF][-FF]", 1.0}}},
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
	m_Individual.rule = testRules;
	//m_Individual.rule = RL.GenerateSemiRandomRules();
	//m_Individual.rule = RL.GenerateSemiPOERules();

}

void Single::InitialiseLsystem()
{
	LsystemGenerator LS = LsystemGenerator();

	LS.Generate(m_StartingWord, m_RuleIterations, m_Individual.rule);

	std::stack<char> finalStack = LS.ReturnFinalStack();
	m_Individual.individual = finalStack;
}

// idea: swap two neighbouring symbols

void Single::MutateRule()
{
	std::vector<char> options = { '+', '-', 'F', '[', ']' };
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> charDist(0, options.size() - 1);
	std::uniform_int_distribution<int> posDist(0, m_Individual.rule['F'][0].first.size());
	std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);

	float mutationChance = chanceDist(gen);

	if (mutationChance < 0.75f)
	{
		char randomChar = options[charDist(gen)];
		int randomPos = posDist(gen);
		m_Individual.rule['F'][0].first.insert(randomPos, 1, randomChar);
		//std::cout << "Inserted '" << randomChar << "' at position " << randomPos << std::endl;
	}
	else if (!m_Individual.rule['F'][0].first.empty())
	{
		int randomPos = posDist(gen);
		char deletedChar = m_Individual.rule['F'][0].first[randomPos];
		m_Individual.rule['F'][0].first.erase(randomPos, 1);
		//std::cout << "Deleted '" << deletedChar << "' from position " << randomPos << std::endl;
	}
	else if (m_Individual.rule['F'][0].first.size() > 1) {
		int pos1 = posDist(gen);
		int pos2 = posDist(gen);
		std::swap(m_Individual.rule['F'][0].first[pos1], m_Individual.rule['F'][0].first[pos2]);
	}

	//std::cout << "Modified 'F' rule: " << m_Individual.rule['F'][0].first << std::endl;

	UpdateIndividual();
}



void Single::MutateWord()
{
	std::stack<char> tempStack = m_Individual.individual;
	std::vector<int> fIndices;
	std::vector<char> symbols;

	while (!tempStack.empty())
	{
		symbols.push_back(tempStack.top());
		tempStack.pop();
	}
	std::reverse(symbols.begin(), symbols.end());

	for (int i = 0; i < symbols.size(); ++i)
	{
		if (symbols[i] == 'F')
		{
			fIndices.push_back(i);
		}
	}

	if (fIndices.empty())
	{
		std::cout << "No 'F' symbols found to expand.\n";
		return;
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, fIndices.size() - 1);
	int randomIndex = fIndices[dist(gen)];

	std::vector<char> possibleSymbols = { '[', ']', '+', '-', 'F' };
	std::uniform_int_distribution<> lengthDist(3, 8); // Random expansion length
	int expansionSize = lengthDist(gen);

	std::vector<char> randomExpansion;
	for (int i = 0; i < expansionSize; ++i)
	{
		std::uniform_int_distribution<> symbolDist(0, possibleSymbols.size() - 1);
		randomExpansion.push_back(possibleSymbols[symbolDist(gen)]);
	}

	symbols[randomIndex] = 'F';
	symbols.insert(symbols.begin() + randomIndex + 1, randomExpansion.begin(), randomExpansion.end());

	while (!m_Individual.individual.empty())
	{
		m_Individual.individual.pop();
	}

	for (auto it = symbols.rbegin(); it != symbols.rend(); ++it)
	{
		m_Individual.individual.push(*it);
	}

	std::stack<char> reversedStack;
	while (!m_Individual.individual.empty())
	{
		reversedStack.push(m_Individual.individual.top());
		m_Individual.individual.pop();
	}

	m_Individual.individual = reversedStack;
}




void Single::Evaluate(FitnessType chosenFitness)
{
	m_Individual.Fitness = 0;
	std::vector<std::vector<int>> constraintMatrix = {
{0, 2, 0, 0},
{2, 0, 0, 0},
{0, 0, 0, 1},
{0, 0, 1, 0}
	};



	switch (chosenFitness)
	{
	case AREA:
		m_Individual.Fitness = FF.AreaFunction(m_Individual.individual, m_InitialState, 50);
		break;
	case MIN_MAX_CHECKPOINTS:
		m_Individual.Fitness = (FF.CheckpointDistanceFitness(m_CheckpointLocations, constraintMatrix, m_InitialState, m_Individual.individual, 50, 50)) * 100 + FF.AreaFunction(m_Individual.individual, m_InitialState, 50);
		break;
	case CHECKPOINT_DISTANCE:
		m_Individual.Fitness = FF.EvaluateCheckpointFitness(m_CheckpointLocations,m_InitialState,m_Individual.individual,50,50 );
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


	std::stack<char> finalStack = LS.ReturnFinalStack(); // problem is here
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











