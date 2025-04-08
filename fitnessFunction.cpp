#include "fitnessFunction.h"
#include "queue"
#include "turtle.h"
#include "Testing.h"

extern Testing testing;

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




std::vector<Checkpoint> FitnessFunction::CreateCheckpoints(int width, int height, CheckpointPattern pattern, int numCheckpoints) 
{
    unsigned int seed = 0;
    std::vector<Checkpoint> checkpoints;
    int midRow = height / 2;
    int midCol = width / 2;
    std::mt19937 gen(seed);  

    if (pattern == RANDOM) 
    {
        std::uniform_int_distribution<int> distX(0, width);
        std::uniform_int_distribution<int> distY(0, height);

        for (int i = 0; i < numCheckpoints; ++i) 
        {
            checkpoints.push_back(Checkpoint{ distX(gen), distY(gen) });
        }
    }
    else if (pattern == SQUARE) 
    {
        int step = std::sqrt(numCheckpoints);
        for (int i = 0; i < step; ++i) 
        {
            for (int j = 0; j < step; ++j) 
            {
                checkpoints.push_back(Checkpoint{ i * (width / step), j * (height / step) });
            }
        }
    }
    else if (pattern == CLUSTERED) 
    {
        int clusterX = midCol + (rand() % (width / 4)) - (width / 8);
        int clusterY = midRow + (rand() % (height / 4)) - (height / 8);

        std::uniform_int_distribution<int> offset(-10, 10);
        for (int i = 0; i < numCheckpoints; ++i) 
        {
            checkpoints.push_back(Checkpoint{ clusterX + offset(gen), clusterY + offset(gen) });
        }
    }
    else if (pattern == CIRCULAR)
    {
        float radius = std::min(width, height) / 3.0f;
        float angleStep = 2 * M_PI / numCheckpoints;

        for (int i = 0; i < numCheckpoints; ++i)
        {
            float angle = i * angleStep;
            int x = static_cast<int>(midCol + radius * cos(angle));
            int y = static_cast<int>(midRow + radius * sin(angle));

            // Print the checkpoint coordinates
            //std::cout << "Checkpoint " << i << ": (" << x << ", " << y << ")\n";

            checkpoints.push_back(Checkpoint{ x, y });
        }
    }
    else if (pattern == LINEAR) 
    {

        int stepX = width / (numCheckpoints + 1); 

        for (int i = 0; i < numCheckpoints; ++i) 
        {
            checkpoints.push_back(Checkpoint{ (i + 1) * stepX, midRow });
        }
    }

    return checkpoints;
}



// min max
float FitnessFunction::CheckpointDistanceFitness(std::vector<Checkpoint> checkpoints, std::vector<std::vector<int>> constraintMatrix, TurtleState initialState, std::stack<char> lsystem, int width, int height)
{
    Turtle turtle{ initialState, lsystem, width, height };
    std::vector<int> grid = turtle.returnGridVector();

    float maxSum = 0.0f;
    float minSum = 0.0f;

    for (size_t i = 0; i < checkpoints.size(); ++i)
    {
        for (size_t j = i + 1; j < checkpoints.size(); ++j)
        {
            int constraint = constraintMatrix[i][j];  // 0 = Ignore, 1 = Minimise, 2 = Maximise

            if (constraint == 0) continue;  // Skip if no constraint is specified

            Point start = { checkpoints[i].x, checkpoints[i].y };
            Point end = { checkpoints[j].x, checkpoints[j].y };

            std::cout << "Checkpoint Pair: (" << checkpoints[i].x << ", " << checkpoints[i].y << ") and ("
                << checkpoints[j].x << ", " << checkpoints[j].y << ")\n";

            // Print the type of constraint
            if (constraint == 1)
                std::cout << "Constraint: Minimise the distance\n";
            else if (constraint == 2)
                std::cout << "Constraint: Maximise the distance\n";

            int distance = ComputeShortestPath(grid, width, height, start, end);

            std::cout << "Distance: " << distance << "\n";

            if (constraint == 1) // Minimise
                minSum += distance;
            else if (constraint == 2) // Maximise
                maxSum += distance;
        }
    }

    std::cout << "---------------------\n";
    std::cout << "Minimise Sum: " << minSum << "\n";
    std::cout << "Maximise Sum: " << maxSum << "\n";
    std::cout << "---------------------\n";

    return ((maxSum + 1) / (minSum + 1)) * 100; // Return a ratio, adjusting to avoid division by zero
}



int FitnessFunction::ComputeShortestPath(std::vector<int>& grid, int width, int height, Point start, Point end)
{
    std::queue<std::pair<Point, int>> q;
    std::vector<bool> visited(width * height, false);

    q.push({ start, 0 });
    visited[start.y * width + start.x] = true;

    // Define the 3x3 neighborhood of the checkpoint
    std::vector<std::pair<int, int>> neighborOffsets = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1,  0}, {0,  0}, {1,  0},
        {-1,  1}, {0,  1}, {1,  1}
    };

    while (!q.empty())
    {
        auto [current, dist] = q.front();
        q.pop();

        // Check if the current point is within the 3x3 neighborhood of the actual checkpoint
        for (const auto& offset : neighborOffsets)
        {
            int neighborX = end.x + offset.first;
            int neighborY = end.y + offset.second;

            if (current.x == neighborX && current.y == neighborY)
            {
                return dist;
            }
        }

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

    return -1;
}


//2000 one

int FitnessFunction::EvaluateCheckpointFitness(const std::vector<Checkpoint>& checkpoints, TurtleState initialState, std::stack<char> lsystem, int width, int height)
{
    Turtle turtle{ initialState, lsystem, width, height };
    std::vector<int> grid = turtle.returnGridVector();

    int totalFitness = 0;
    std::unordered_set<int> visitedCheckpoints;

    while (visitedCheckpoints.size() < checkpoints.size())
    {
        int closestIndex = -1;
        int minDistance = INT_MAX;

        // Find the closest reachable checkpoint
        for (size_t i = 0; i < checkpoints.size(); ++i)
        {
            if (visitedCheckpoints.count(i)) continue;

            int distance = ComputeClosestPathDistance(grid, width, height, checkpoints[i]);

            if (distance < minDistance) {
                minDistance = distance;
                closestIndex = static_cast<int>(i);
            }
        }

        // If no reachable checkpoint is found, break
        if (closestIndex == -1) break;

        // If the closest checkpoint is reachable (distance != INT_MAX)
        if (minDistance != INT_MAX)
        {
            if (minDistance == 0)
            {
                totalFitness += 500;  // Reward for immediate proximity
            }
            else
            {
                totalFitness += (1000 / (minDistance + 1));  // Distance-based reward
            }

            // Mark the checkpoint as visited
            visitedCheckpoints.insert(closestIndex);
        }
    }

    return totalFitness;
}

int FitnessFunction::ComputeClosestPathDistance(std::vector<int>& grid, int width, int height, const Checkpoint& checkpoint)
{
    std::queue<std::pair<Point, int>> q;
    std::vector<bool> visited(width * height, false);

    // Initialize the queue with all 'PATH' cells
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (grid[y * width + x] == PATH)
            {
                q.push({ {x, y}, 0 });
                visited[y * width + x] = true;
            }
        }
    }

    // Perform BFS to find the shortest distance to the checkpoint
    while (!q.empty())
    {
        auto [current, dist] = q.front();
        q.pop();

        // If we reach the checkpoint, return the distance
        if (current.x == checkpoint.x && current.y == checkpoint.y)
        {
            return dist;
        }

        // Explore neighbors
        for (const auto& dir : directionVectorss)
        {
            int nx = current.x + dir.first;
            int ny = current.y + dir.second;
            int newIndex = ny * width + nx;

            // Ensure the neighbor is within bounds and not visited yet
            if (nx >= 0 && nx < width && ny >= 0 && ny < height && !visited[newIndex])
            {
                visited[newIndex] = true;
                q.push({ {nx, ny}, dist + 1 });
            }
        }
    }

    // Return INT_MAX if the checkpoint is not reachable
    return INT_MAX;
}

