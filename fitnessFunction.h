#pragma once
#include "common.h"
#include "structs.h"




class FitnessFunction
{


public:

	float CheckpointDistanceFitness(std::vector<Checkpoint> checkpoints, std::vector<std::vector<int>> constraintMatrix, TurtleState initialState, std::stack<char> lsystem, int width, int height);

	int ComputeShortestPath(std::vector<int>& grid, int width, int height, Point start, Point end);
	int AreaFunction(std::stack<char> lsystem, TurtleState initialState, int gridWidth);


	std::vector<Checkpoint> CreateCheckpoints(int width, int height);
private:


};