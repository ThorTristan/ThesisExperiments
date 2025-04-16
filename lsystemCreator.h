#pragma once
#include "common.h"



class LsystemGenerator
{


public:

	void Generate(std::string& initialWord, int iterations, std::unordered_map<char, std::vector<std::pair<std::string, float>>>& rules);

	std::stack<char> ReturnFinalStack() const;

	void PrintStack(std::stack<char> stack);

private:


	void InitialWord(std::string& word);

	void Iterate(std::unordered_map<char, std::vector<std::pair<std::string, float>>>& rules);

	void ApplyRule(std::unordered_map<char, std::vector<std::pair<std::string, float>>>& rules);



private:

	std::stack<char> m_Stack;
	std::stack<char> m_FinalWord;

	std::stack<char> m_NewStack;
	std::stack<char> m_OldStack;

	int m_Iterations;



};



