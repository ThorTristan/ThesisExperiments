#include "fitnessFunction.h"
#include "queue"
#include "turtle.h"
#include "Testing.h"
#include <sstream>
#include <fstream>
#include <random>
#include <cmath>
#include <limits>
#include <unordered_set>

extern Testing testing;

constexpr std::pair<int, int> directionVectors[] =
{
    {0, -1}, {1, 0}, {0, 1}, {-1, 0},  // Cardinal
    {1, -1}, {1, 1}, {-1, 1}, {-1, -1} // Diagonal
};

int FitnessFunction::AreaFunction(std::stack<char> lsystem, TurtleState initialState, int gridWidth)
{
    Turtle turtle{ initialState, lsystem, gridWidth, gridWidth };
    const auto& gene = turtle.returnGridVector();

    return std::count_if(gene.begin(), gene.end(), [](int cell) {
        return cell != EMPTY;
        });
}

std::vector<Checkpoint> FitnessFunction::CreateCheckpoints(int width, int height, CheckpointPattern pattern, int numCheckpoints)
{
    static std::mt19937 gen(0);
    std::vector<Checkpoint> checkpoints;
    int midRow = height / 2, midCol = width / 2;

    switch (pattern)
    {
    case RANDOM: {
        std::uniform_int_distribution<int> distX(0, width - 1), distY(0, height - 1);
        for (int i = 0; i < numCheckpoints; ++i)
            checkpoints.emplace_back(distX(gen), distY(gen));
        break;
    }
    case SQUARE: {
        int step = std::sqrt(numCheckpoints);
        for (int i = 0; i < step; ++i)
            for (int j = 0; j < step; ++j)
                checkpoints.emplace_back(i * width / step, j * height / step);
        break;
    }
    case CLUSTERED: {
        int clusterX = midCol + (rand() % (width / 4)) - (width / 8);
        int clusterY = midRow + (rand() % (height / 4)) - (height / 8);
        std::uniform_int_distribution<int> offset(-10, 10);
        for (int i = 0; i < numCheckpoints; ++i)
            checkpoints.emplace_back(clusterX + offset(gen), clusterY + offset(gen));
        break;
    }
    case CIRCULAR: {
        float radius = std::min(width, height) / 3.0f;
        float angleStep = 2 * M_PI / numCheckpoints;
        for (int i = 0; i < numCheckpoints; ++i) {
            float angle = i * angleStep;
            checkpoints.emplace_back(
                static_cast<int>(midCol + radius * cos(angle)),
                static_cast<int>(midRow + radius * sin(angle))
            );
        }
        break;
    }
    case LINEAR: {
        int stepX = width / (numCheckpoints + 1);
        for (int i = 0; i < numCheckpoints; ++i)
            checkpoints.emplace_back((i + 1) * stepX, midRow);
        break;
    }
    }

    return checkpoints;
}

void FitnessFunction::setEnableLogging(bool b) { enableLogging = b; }

float FitnessFunction::CheckpointDistanceFitness(
    std::vector<Checkpoint>& checkpoints,
    std::vector<std::vector<int>>& constraintMatrix,
    TurtleState initialState,
    std::stack<char> lsystem,
    int width, int height)
{
    Turtle turtle{ initialState, lsystem, width, height };
    auto grid = turtle.returnGridVector();

    float maxSum = 0.0f, minSum = 0.0f;
    if (enableLogging) distanceLogs.clear();

    for (size_t i = 0; i < checkpoints.size(); ++i)
    {
        for (size_t j = i + 1; j < checkpoints.size(); ++j)
        {
            int constraint = constraintMatrix[i][j];
            if (constraint == 0) continue;

            Point start = { checkpoints[i].x, checkpoints[i].y };
            Point end = { checkpoints[j].x, checkpoints[j].y };
            int distance = ComputeShortestPath(grid, width, height, start, end);

            if (enableLogging) {
                distanceLogs.push_back({ start, end, constraint, distance });
            }

            if (constraint == 1) minSum += distance;
            else if (constraint == 2) maxSum += distance;
        }
    }

    return ((maxSum + 1) / (minSum + 1)) * 100;
}

void FitnessFunction::ExportDistancesToCSV(const std::string& filename, int runIndex)
{
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << " for writing.\n";
        return;
    }

    file << "Run Index,Checkpoint A (x),Checkpoint A (y),Checkpoint B (x),Checkpoint B (y),Constraint,Distance\n";
    for (const auto& entry : distanceLogs) {
        file << runIndex << ","
            << entry.checkpointA.x << "," << entry.checkpointA.y << ","
            << entry.checkpointB.x << "," << entry.checkpointB.y << ","
            << (entry.constraintType == 1 ? "Minimise" : "Maximise") << ","
            << entry.distance << "\n";
    }
}

int FitnessFunction::ComputeShortestPath(std::vector<int>& grid, int width, int height, Point start, Point end)
{
    std::queue<std::pair<Point, int>> q;
    std::vector<bool> visited(width * height, false);

    q.push({ start, 0 });
    visited[start.y * width + start.x] = true;

    while (!q.empty()) {
        auto [current, dist] = q.front();
        q.pop();

        // Instead of direct comparison to 'end', check if we're next to it
        for (const auto& [dx, dy] : directionVectors) {
            if (current.x + dx == end.x && current.y + dy == end.y)
                return dist + 1;
        }

        for (const auto& [dx, dy] : directionVectors) {
            int nx = current.x + dx;
            int ny = current.y + dy;
            int index = ny * width + nx;

            if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                !visited[index] && grid[index] == PATH) {
                visited[index] = true;
                q.push({ {nx, ny}, dist + 1 });
            }
        }
    }

    return -1; // No path to any neighbor of the endpoint
}


int FitnessFunction::EvaluateCheckpointFitness(
    std::vector<Checkpoint>& checkpoints,
    TurtleState initialState,
    std::stack<char> lsystem,
    int width, int height)
{
    Turtle turtle{ initialState, lsystem, width, height };
    auto grid = turtle.returnGridVector();

    int totalFitness = 0;
    std::unordered_set<int> visited;

    while (visited.size() < checkpoints.size()) {
        int closestIndex = -1;
        int minDistance = INT_MAX;

        for (size_t i = 0; i < checkpoints.size(); ++i) {
            if (visited.count(i)) continue;
            int distance = ComputeClosestPathDistance(grid, width, height, checkpoints[i]);
            if (distance < minDistance) {
                minDistance = distance;
                closestIndex = static_cast<int>(i);
            }
        }

        if (closestIndex == -1) break;

        totalFitness += (minDistance == 0) ? 500 : (1000 / (minDistance + 1));
        visited.insert(closestIndex);
    }

    return totalFitness;
}

int FitnessFunction::ComputeClosestPathDistance(std::vector<int>& grid, int width, int height, Checkpoint& checkpoint)
{
    std::queue<std::pair<Point, int>> q;
    std::vector<bool> visited(width * height, false);

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            if (grid[y * width + x] == PATH) {
                q.push({ {x, y}, 0 });
                visited[y * width + x] = true;
            }

    while (!q.empty()) {
        auto [current, dist] = q.front(); q.pop();
        if (current.x == checkpoint.x && current.y == checkpoint.y)
            return dist;

        for (const auto& [dx, dy] : directionVectors) {
            int nx = current.x + dx, ny = current.y + dy;
            int index = ny * width + nx;

            if (nx >= 0 && nx < width && ny >= 0 && ny < height && !visited[index]) {
                visited[index] = true;
                q.push({ {nx, ny}, dist + 1 });
            }
        }
    }

    return INT_MAX;
}
