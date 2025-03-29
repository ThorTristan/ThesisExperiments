#pragma once
#include "common.h"
#include "structs.h"

class GridManager
{


public:

	void PrintGrid(std::vector<int> grid);

	void RenderGrid(std::vector<int> grid, int GridPosX, int GridPosY, int GridWidth, int GridHeight, int rows = 50, int cols = 50);


private:

	//void RenderCheckpoints(std::vector<Checkpoint> checkpointsLocation, int GridPosX, int GridPosY, int GridWidth, int cellWidth, int cellHeight);




};