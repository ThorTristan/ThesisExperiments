#pragma once
#include "individual.h"
#include "gridManager.h"
#include "renderManager.h"
//#include "SceneManager.h"

class Evolution
{

public:

	Evolution(int popSize, std::string startingWord, int ruleIterations, TurtleState initialState, int numGenerations, FitnessType chosenFitness, MutationType chosenMutation, CheckpointPattern chosenPatern);

	void InitialisePopulation();
	void Selection();
	void Mutation();
	float Evaluation();
	void Run();


public:

	void SetMutationParams(std::vector<char> symbolSet, std::vector<int> mutationParams, int expansionSize);

	std::vector<Single> GetPopulation();
	Individual GetBestIndividual();
	void PrintGrid(Individual individual);
	void PrintRuleSet(Individual individual);

private:

	void SortPopulation();
	void RenderIndividual(Individual individual);
	void RenderFitness(Individual individual);
	std::vector<int> GrammarToGrid(Individual individual);

private:


	int m_PopSize;
	int m_NumberOfGenerations;
	std::string m_StartingWord;
	int m_RuleIterations;
	TurtleState m_InitialState;
	FitnessType m_ChosenFitness;
	bool m_Complete;

	MutationType m_MutationChoice;

	std::vector<char> m_SymbolSet;
	std::vector<int> m_MutationChances;
	int m_ExpansionSize;


private:

	std::vector<Single> m_Population;
	Individual m_BestIndividual;
	CheckpointPattern m_Pattern;

};