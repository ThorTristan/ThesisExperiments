#pragma once
#include "individual.h"
#include "gridManager.h"
#include "renderManager.h"
//#include "SceneManager.h"

class Evolution
{

public:

	Evolution(int popSize, std::string startingWord, int ruleIterations, TurtleState initialState, int numGenerations, FitnessType chosenFitness, MutationType chosenMutation);

	void InitialisePopulation();
	void Selection();
	void Mutation();
	void Evaluation();
	void Run();


public:

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


private:

	std::vector<Single> m_Population;
	Individual m_BestIndividual;


};