#pragma once
#include "common.h"

std::vector<char> StackToVector(std::stack<char> s)
{
	std::vector<char> v;
	while (!s.empty())
	{
		v.push_back(s.top());
		s.pop();
	}
	std::reverse(v.begin(), v.end());
	return v;
}


std::stack<char> VectorToStack(std::vector<char> v)
{
	std::stack<char> s;

	for (char c : v)
	{
		s.push(c);
	}

	return s;

}
