#pragma once
#include "common.h"
#include "structs.h"

struct DistanceLogEntry
{
	Point checkpointA;
	Point checkpointB;
	int constraintType; // 1 = Minimise, 2 = Maximise
	int distance;
};



class FitnessFunction
{


public:

	float CheckpointDistanceFitness(std::vector<Checkpoint>& checkpoints,std::vector<std::vector<int>>& constraintMatrix, TurtleState initialState, std::stack<char> lsystem, int width, int height);

	int ComputeShortestPath(std::vector<int>& grid, int width, int height, Point start, Point end);
	int AreaFunction(std::stack<char> lsystem, TurtleState initialState, int gridWidth);

	int EvaluateCheckpointFitness(std::vector<Checkpoint>& checkpoints,
		TurtleState initialState, std::stack<char> lsystem,
		int width,
		int height);

	int ComputeClosestPathDistance(std::vector<int>& grid, int width, int height, Checkpoint& checkpoint);

	std::vector<Checkpoint> CreateCheckpoints(int width, int height, CheckpointPattern pattern, int numCheckpoints);

	void setEnableLogging(bool b);
	void ExportDistancesToCSV(const std::string& filename, int runIndex);
private:

	bool enableLogging = true;
	std::vector<DistanceLogEntry> distanceLogs;
};