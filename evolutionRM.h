#pragma once
#include "individual.h"
#include "gridManager.h"
#include "renderManager.h"
//#include "SceneManager.h"
#include "map"
#include "json.hpp"

using json = nlohmann::json;
class Evolution
{

public:

	Evolution(int popSize, std::string startingWord, int ruleIterations, TurtleState initialState, int numGenerations, FitnessType chosenFitness, MutationType chosenMutation, CheckpointPattern chosenPatern, std::vector<std::vector<int>> matrix);

	void InitialisePopulation();
	void Selection();
	void Mutation();
	float Evaluation();
	void Run(int runIndex);
	std::string SerializeRules(const std::unordered_map<char, std::vector<std::pair<std::string, float>>>& ruleMap);
	std::string StackToString(const std::stack<char>& s);
	std::string ConvertStackToString(std::stack<char> stack);
	json ConvertRuleMap(const std::unordered_map<char, std::vector<std::pair<std::string, float>>>& ruleMap);

public:

	void SetMutationParams(std::vector<char> symbolSet, std::vector<int> mutationParams, int expansionSize);

	std::vector<Single> GetPopulation();
	Individual GetBestIndividual();
	void PrintGrid(Individual individual);
	void PrintRuleSet(Individual individual);
	void PrintIndividual(std::stack<char> ind);

private:

	void SortPopulation();
	void RenderIndividual(Individual individual);
	void RenderFitness(Individual individual, int xPos, int yPos);
	void RenderText(const std::string& numberText, int xPos, int yPos, const std::string& text);
	std::vector<int> GrammarToGrid(Individual individual);

private:


	int m_PopSize;
	int m_NumberOfGenerations;
	std::string m_StartingWord;
	int m_RuleIterations;
	TurtleState m_InitialState;
	FitnessType m_ChosenFitness;

	std::vector<std::vector<int>> m_ChosenMatrix;

	MutationType m_MutationChoice;
	std::vector<char> m_SymbolSet;
	std::vector<int> m_MutationChances;
	int m_ExpansionSize;

	bool m_Complete;


	float m_AverageFitness;
	int m_CurrentGeneration;

private:

	std::vector<Single> m_Population;
	Individual m_BestIndividual;
	CheckpointPattern m_Pattern;



};