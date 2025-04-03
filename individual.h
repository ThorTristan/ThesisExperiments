#pragma once
#include "LsystemCreator.h"
#include "fitnessFunction.h"
#include "turtle.h"


enum FitnessType
{


	AREA,
	CHECKPOINT,
	AREA_CHECKPOINT,
	MIN_MAX_CHECKPOINTS,
	CHECKPOINT_DISTANCE


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

	Single(std::string startingWord, int ruleIterations, TurtleState initialState, CheckpointPattern pattern);

	void MutateRule(std::vector<char> symbolSet, std::vector<int> mutationParams);

	void MutateWord(std::vector<char> symbolSet, int expansionSize);

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




