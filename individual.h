#pragma once
//#include "RandomRuleProgram.h"
//#include "LSystemEvo.h"

#include "LsystemCreator.h"
#include "fitnessFunction.h"
#include "turtle.h"


enum FitnessType
{


	AREA,
	CHECKPOINT,
	AREA_CHECKPOINT,
	DEAD_END,
	BRANCHING,
	MIN_MAX_CHECKPOINTS


};


struct Individual
{

	std::stack<char> individual;
	float Fitness;
	std::unordered_map<char, std::vector<std::pair<std::string, float>>> rule;


};

class Single
{

public:

	Single(std::string startingWord, int ruleIterations, TurtleState initialState);

	void Mutate();

	void MutateWord();

	void Evaluate(FitnessType chosenFitness);


public:

	Individual GetIndividual();
	TurtleState GetInitialState();

	void PrintRule();
	void PrintLS();
	void PrintFF();



private:
	void GenerateRule();
	void InitialiseLsystem();
	void UpdateIndividual();



private:

	Individual m_Individual;

	TurtleState m_InitialState;
	std::vector<Checkpoint> m_CheckpointLocations;
	std::string m_StartingWord;
	int m_RuleIterations;




};




