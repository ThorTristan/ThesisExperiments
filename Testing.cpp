#include "Testing.h"
#include "FitnessFunction.h"
#include "Individual.h"
#include "gridManager.h"


extern GridManager GM;
extern FitnessFunction FF;

// how to test it ?
// first ill test compute shortest path
// fill in a grid with checkpoints
// and then calculate the shortest path between each of the checkpoints

// csp works,
// now need to test the algorithm


// i need a maze that has all of the cp
// or just print all the stats for it
// with a cmd version so i can count 
// so one maze and keep giving me the fitness of it

//void Testing::TestCSP()
//{
//	int width = 50;
//	int height = 50;
//
//	// Step 1: Create a test grid (from your L-system Turtle)
//	std::vector<int> grid = CreateTestGrid(width, height);
//
//	// Step 2: Create checkpoints (feel free to hardcode for more control)
//	std::vector<Checkpoint> checkpoints = FF.CreateCheckpoints(width, height, CIRCULAR, 4);
//
//	// Step 3: Render the grid with the checkpoints (visual debug)
//	GM.RenderGrid(grid, checkpoints, 300, 100, 300, 300);
//
//	// Step 4: Call ComputeShortestPath on each pair and print distances
//	for (size_t i = 0; i < checkpoints.size(); ++i)
//	{
//		for (size_t j = i + 1; j < checkpoints.size(); ++j)
//		{
//			Point start{ checkpoints[i].x, checkpoints[i].y };
//			Point end{ checkpoints[j].x, checkpoints[j].y };
//
//			int dist = FF.ComputeShortestPath(grid, width, height, start, end);
//
//			std::cout << "Distance from Checkpoint " << i
//				<< " (" << start.x << "," << start.y << ") to Checkpoint " << j
//				<< " (" << end.x << "," << end.y << ") = " << dist << std::endl;
//		}
//	}
//}
//
//
//
//
//void Testing::TestFitnessFunction()
//{
//
//
//
//	int width = 50, height = 50;
//	//std::vector<int> grid(width * height, PATH);
//	//std::vector<int> grid = CreateTestGrid(width, height);
//
//	FitnessFunction fitnessFunction;
//	std::vector<Checkpoint> checkpoints = fitnessFunction.CreateCheckpoints(50, 50,CIRCULAR, 4);
//
//	std::vector<std::vector<int>> constraintMatrix = {
//	{0, 1, 2, 0},  // Minimize (1) distance from 0->1, Maximize (2) from 0->2
//	{1, 0, 0, 2},  // Minimize (1) from 1->0, Maximize (2) from 1->3
//	{2, 0, 0, 1},  // Maximize (2) from 2->0, Minimize (1) from 2->3
//	{0, 2, 1, 0}   // Maximize (2) from 3->1, Minimize (1) from 3->2
//	};
//
//	TurtleState initialState{ width / 2,width - 1,0 };
//
//
//	float fitness = fitnessFunction.CheckpointDistanceFitness(checkpoints, constraintMatrix, initialState, GenerateLsystem(), width, height);
//
//	std::cout << "Computed Fitness Score: " << fitness << std::endl;
//
//
//	//GM.RenderGrid(grid, checkpoints, 200, 200, 300, 300);
//	for (size_t i = 0; i < checkpoints.size(); ++i)
//	{
//		for (size_t j = i + 1; j < checkpoints.size(); ++j)
//		{
//			Point start = { checkpoints[i].x, checkpoints[i].y };
//			Point end = { checkpoints[j].x, checkpoints[j].y };
//			int distance = fitnessFunction.ComputeShortestPath(grid, width, height, start, end);
//			//std::cout << "Distance from Checkpoint " << i << " to " << j << ": " << distance << std::endl;
//		}
//	}
//
//
//}

std::vector<int> Testing::CreateTestGrid(int width, int height,std::stack<char> lsystem) {



	TurtleState initialState{ width / 2,width - 1,0 };
	Turtle turtle(initialState, lsystem, width, height);

	return turtle.returnGridVector();

}

#include <iomanip> // for std::setw

void Testing::PrintGrid(const std::vector<int>& grid, int width, int height)
{
    // Print X-axis numbers every 5 steps
    std::cout << "   "; // offset for Y-axis numbers
    for (int x = 0; x < width; ++x)
    {
        if (x % 5 == 0)
            std::cout << (x / 10) % 10; // tens
        else
            std::cout << " ";
    }
    std::cout << '\n';

    std::cout << "   ";
    for (int x = 0; x < width; ++x)
    {
        if (x % 5 == 0)
            std::cout << x % 10; // ones
        else
            std::cout << " ";
    }
    std::cout << '\n';

    // Print grid
    for (int y = 0; y < height; ++y)
    {
        std::cout << std::setw(2) << y << " "; // Y-axis
        for (int x = 0; x < width; ++x)
        {
            std::cout << (grid[y * width + x] == 0 ? '#' : '.');
        }
        std::cout << '\n';
    }
}



std::stack<char> Testing::GenerateLsystem()
{

	//LSystem LS = LSystem();
	int width = 50, height = 50;
	std::string startingWord = "FFFFF";
	int ruleIterations = 4;
	TurtleState initialState{ width / 2,width - 1,0 };

	Single S = Single(startingWord, ruleIterations, initialState,CIRCULAR);


	return S.GetIndividual().individual;
}

std::stack<char> Testing::ConvertToStack(const std::string& input) 
{
	std::stack<char> result;
	for (char ch : input) {
		result.push(ch);
	}
	return result;
}