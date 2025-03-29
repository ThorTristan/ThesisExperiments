#include "fitnessFunction.h"
#include "queue"
#include "turtle.h"


std::pair<int, int> directionVectorss[] =
{
    {0, -1},  // Up
    {1, 0},   // Right
    {0, 1},   // Down
    {-1, 0},  // Left
    {1, -1},  // Up-Right
    {1, 1},   // Down-Right
    {-1, 1},  // Down-Left
    {-1, -1}  // Up-Left
};

int FitnessFunction::AreaFunction(std::stack<char> lsystem, TurtleState initialState, int gridWidth)
{

    Turtle turtle{ initialState,lsystem,gridWidth,gridWidth };
    std::vector<int> gene = turtle.returnGridVector();

    int pathCounter = 0;

    for (int num : gene)
    {
        if (num != EMPTY)
        {
            pathCounter++;
        }

    }


    return pathCounter;


}

std::vector<Checkpoint> FitnessFunction::CreateCheckpoints(int width, int height)
{
    std::vector<Checkpoint> checkpoints;

    int midRow = height / 2;
    int midCol = width / 2;

    checkpoints.push_back(Checkpoint{ midCol - midCol / 4, midRow + midRow / 3 }); // Slightly left of center, lower
    checkpoints.push_back(Checkpoint{ midCol + midCol / 4, midRow + midRow / 3 }); // Slightly right of center, lower
    checkpoints.push_back(Checkpoint{ midCol - midCol / 6, midRow + midRow / 2 }); // Closer to center, even lower
    checkpoints.push_back(Checkpoint{ midCol + midCol / 6, midRow + midRow / 2 }); // Closer to center, even lower

    return checkpoints;
}


float FitnessFunction::CheckpointDistanceFitness(std::vector<Checkpoint> checkpoints,std::vector<std::vector<int>> constraintMatrix,TurtleState initialState,
    std::stack<char> lsystem,
    int width,
    int height)
{


    Turtle turtle{ initialState, lsystem, width,height };
    std::vector<int> grid = turtle.returnGridVector();


    //std::vector<int> grid(width * height, PATH);

    float maxSum = 0.0f;
    float minSum = 0.0f;


    for (size_t i = 0; i < checkpoints.size(); ++i)
    {
        for (size_t j = i + 1; j < checkpoints.size(); ++j)
        {
            int constraint = constraintMatrix[i][j];  // 0 = Ignore, 1 = Minimize, 2 = Maximize

            if (constraint == 0) continue;

            Point start = { checkpoints[i].x, checkpoints[i].y };
            Point end = { checkpoints[j].x, checkpoints[j].y };




            int distance = ComputeShortestPath(grid, width, height, start, end);

            //if (distance < 100000)
            //{
            //    std::cout << "Distance from Checkpoint " << i << " to " << j << ": " << distance << std::endl;

            //}

            if (constraint == 1) // Minimise
                minSum += distance;
            else if (constraint == 2) // Maximise
                maxSum += distance;
        }
    }

    return (maxSum + 1) / (minSum + 1);
}


int FitnessFunction::ComputeShortestPath(std::vector<int>& grid, int width, int height, Point start, Point end)
{
    std::queue<std::pair<Point, int>> q;
    std::vector<bool> visited(width * height, false);

    q.push({ start, 0 });
    visited[start.y * width + start.x] = true;

    while (!q.empty())
    {
        auto [current, dist] = q.front();
        q.pop();

        if (current.x == end.x && current.y == end.y)
            return dist;

        for (const auto& dir : directionVectorss)
        {
            int nx = current.x + dir.first;
            int ny = current.y + dir.second;
            int newIndex = ny * width + nx;

            if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                !visited[newIndex] && grid[newIndex] == 1)
            {
                visited[newIndex] = true;
                q.push({ {nx, ny}, dist + 1 });
            }
        }
    }

    return 0;
}