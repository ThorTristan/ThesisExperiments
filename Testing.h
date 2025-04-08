#pragma once
#include "common.h"


class Testing
{

public:

	void TestFitnessFunction();
	void TestCSP();

	std::stack<char> GenerateLsystem();

	std::vector<int> CreateTestGrid(int width, int height, std::stack<char> lsystem);

	std::stack<char> ConvertToStack(const std::string& input);

	void PrintGrid(const std::vector<int>& grid, int width, int height);


};