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

//std::vector<Checkpoint> FitnessFunction::CreateCheckpoints(int width, int height)
//{
//    std::vector<Checkpoint> checkpoints;
//
//    int midRow = height / 2;
//    int midCol = width / 2;
//
//    checkpoints.push_back(Checkpoint{ midCol - midCol / 4, midRow + midRow / 3 }); // Slightly left of center, lower
//    checkpoints.push_back(Checkpoint{ midCol + midCol / 4, midRow + midRow / 3 }); // Slightly right of center, lower
//    checkpoints.push_back(Checkpoint{ midCol - midCol / 6, midRow + midRow / 2 }); // Closer to center, even lower
//    checkpoints.push_back(Checkpoint{ midCol + midCol / 6, midRow + midRow / 2 }); // Closer to center, even lower
//
//    return checkpoints;
//}



std::vector<Checkpoint> FitnessFunction::CreateCheckpoints(int width, int height, CheckpointPattern pattern, int numCheckpoints) 
{
    unsigned int seed = 0;
    std::vector<Checkpoint> checkpoints;
    int midRow = height / 2;
    int midCol = width / 2;
    std::mt19937 gen(seed);  // Use the provided seed for reproducibility

    if (pattern == RANDOM) {
        std::uniform_int_distribution<int> distX(0, width);
        std::uniform_int_distribution<int> distY(0, height);

        for (int i = 0; i < numCheckpoints; ++i) {
            checkpoints.push_back(Checkpoint{ distX(gen), distY(gen) });
        }
    }
    else if (pattern == SQUARE) {
        int step = std::sqrt(numCheckpoints);
        for (int i = 0; i < step; ++i) {
            for (int j = 0; j < step; ++j) {
                checkpoints.push_back(Checkpoint{ i * (width / step), j * (height / step) });
            }
        }
    }
    else if (pattern == CLUSTERED) {
        int clusterX = midCol + (rand() % (width / 4)) - (width / 8);
        int clusterY = midRow + (rand() % (height / 4)) - (height / 8);

        std::uniform_int_distribution<int> offset(-10, 10);
        for (int i = 0; i < numCheckpoints; ++i) {
            checkpoints.push_back(Checkpoint{ clusterX + offset(gen), clusterY + offset(gen) });
        }
    }
    else if (pattern == CIRCULAR) {
        float radius = std::min(width, height) / 3.0f;
        float angleStep = 2 * M_PI / numCheckpoints;

        for (int i = 0; i < numCheckpoints; ++i) {
            float angle = i * angleStep;
            checkpoints.push_back(Checkpoint{
                static_cast<int>(midCol + radius * cos(angle)),
                static_cast<int>(midRow + radius * sin(angle))
                });
        }
    }
    else if (pattern == LINEAR) {
        // Create a linear distribution of checkpoints from left to right (horizontally)
        int stepX = width / (numCheckpoints + 1); // Spacing between checkpoints

        for (int i = 0; i < numCheckpoints; ++i) {
            checkpoints.push_back(Checkpoint{ (i + 1) * stepX, midRow });
        }
    }

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



int FitnessFunction::EvaluateCheckpointFitness(const std::vector<Checkpoint>& checkpoints, TurtleState initialState, std::stack<char> lsystem, int width, int height)
{
    Turtle turtle{ initialState, lsystem, width, height };
    std::vector<int> grid = turtle.returnGridVector();

    int totalFitness = 0;
    std::unordered_set<int> visitedCheckpoints;  // Store visited checkpoint indices

    while (visitedCheckpoints.size() < checkpoints.size()) {
        int closestIndex = -1;
        int minDistance = INT_MAX;

        // Find the closest unvisited checkpoint
        for (size_t i = 0; i < checkpoints.size(); ++i) {
            if (visitedCheckpoints.count(i)) continue;  // Skip visited checkpoints

            int distance = ComputeClosestPathDistance(grid, width, height, checkpoints[i]);

            if (distance < minDistance) {
                minDistance = distance;
                closestIndex = static_cast<int>(i);
            }
        }

        if (closestIndex == -1) break;  // No more reachable checkpoints

        // Score based on distance (lower distance = higher score)
        if (minDistance == 0) {
            totalFitness += 500; // Bonus for checkpoint being on the path
        }
        else {
            totalFitness += (1000 / (minDistance + 1)); // Avoid division by zero
        }

        // Mark checkpoint as visited
        visitedCheckpoints.insert(closestIndex);
    }

    return totalFitness;
}

// Helper function to compute distance to the closest path tile
int FitnessFunction::ComputeClosestPathDistance(std::vector<int>& grid, int width, int height, const Checkpoint& checkpoint)
{
    std::queue<std::pair<Point, int>> q;
    std::vector<bool> visited(width * height, false);

    // Add all path tiles as starting points
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (grid[y * width + x] == PATH) {
                q.push({ {x, y}, 0 });
                visited[y * width + x] = true;
            }
        }
    }

    // BFS to find the shortest distance to the checkpoint
    while (!q.empty()) {
        auto [current, dist] = q.front();
        q.pop();

        if (current.x == checkpoint.x && current.y == checkpoint.y) {
            return dist;
        }

        for (const auto& dir : directionVectorss) {
            int nx = current.x + dir.first;
            int ny = current.y + dir.second;
            int newIndex = ny * width + nx;

            if (nx >= 0 && nx < width && ny >= 0 && ny < height && !visited[newIndex]) {
                visited[newIndex] = true;
                q.push({ {nx, ny}, dist + 1 });
            }
        }
    }

    return INT_MAX; // Unreachable checkpoint
}
