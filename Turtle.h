#pragma once
#include "common.h"
#include "structs.h"




class Turtle
{

public:

	Turtle(TurtleState initialState, std::stack<char> lsystemStack, int gridHeight, int gridWidth, int stepLength = 1);


public:

	void Generate();
	// Return the grid representation
	std::vector<int> returnGridVector();
	void printVector(const std::vector<int>& vec);

	

	bool isInBounds(int x, int y);

private:

	void initializeGrid();
	void updateState();

private:

	void moveForward();
	void turnLeft();
	void turnRight();
	void pushStack();
	void popStack();
	void path(int c);

private:

	TurtleState m_CurrentState;
	TurtleState m_PreviousState;

private:

	std::vector<int> m_Grid;

private:



	static const std::pair<int, int> directionVectors[4];


	int m_StepLength;
	int m_CurrentDirection;
	int m_GridHeight;
	int m_GridWidth;

	std::stack<char> m_Lsystem;

	std::stack<TurtleState> m_LIFO;


};