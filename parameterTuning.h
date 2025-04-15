#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include "individual.h"
#include "fitnessFunction.h"
#include "evolutionRM.h"

class PTuning3
{
private:
    int iterations;
    std::ofstream logFile;
    FitnessFunction FF;

    int totalRuns = 0;
    int currentRun = 0;
    int runIndex = 0;

    std::vector<std::vector<char>> symbolSets =
    {
        {'F'},
        {'F', '+', '-'},
        {'F', '+', '-', '[', ']'},
        {'F', '[', ']'}
    };

    std::vector<std::vector<int>> mutationChances =
    {
        {60, 20, 20},
        {25, 50, 25},
        {50, 25, 25},
        {25, 25, 50},
        {20, 20, 60},
        {34, 33, 33}
    };

    std::vector<int> expansionSizes = { 1, 3, 5, 7 };

    std::vector<std::vector<int>> constraintMatrix =
    {
        {0, 0, 1, 1},
        {0, 0, 1, 0},
        {1, 1, 0, 0},
        {1, 0, 0, 0}
    };

public:
    PTuning3(int iter) : iterations(iter)
    {
        logFile.open("fitness_log.csv", std::ios::app);
        if (!logFile)
        {
            std::cerr << "Failed to open log file!" << std::endl;
        }

        std::ofstream clearCSV("best_individuals_log.csv", std::ios::trunc);
        clearCSV << "Run Index,Generation,Best Fitness,Genetic Code\n";

        std::ofstream clearJSON("genetic_log_run_" + std::to_string(runIndex) + ".json", std::ios::trunc);
        clearJSON << "{}";

        std::ofstream clearFile("fitness_log.csv", std::ios::trunc);
        clearFile.close();

        totalRuns = symbolSets.size() * expansionSizes.size() * mutationChances.size() * 2;

        logFile << "Symbol Set,Expansion Size,Mutation Chance,Mutation Type,Best Fitness,Time Taken (s)\n";
    }

    ~PTuning3()
    {
        if (logFile.is_open())
        {
            logFile.close();
        }
    }

    void Run() {
        std::vector<MutationType> mutationTypes = { WORD, RULE };
        auto experimentStartTime = std::chrono::high_resolution_clock::now();

        for (int iteration = 0; iteration < iterations; ++iteration)
        {
            std::cout << "Starting repetition " << iteration + 1 << " of " << iterations << std::endl;

            for (MutationType mutationType : mutationTypes) {
                for (const auto& symbolSet : symbolSets) {
                    for (int expansionSize : expansionSizes) {
                        for (const auto& mutationChance : mutationChances) {
                            RunTest(symbolSet, expansionSize, mutationChance, mutationType, experimentStartTime);
                            runIndex++;
                        }
                    }
                }
            }
        }
    }

    void RunTest(const std::vector<char>& symbolSet, int expansionSize, const std::vector<int>& mutationChance,
        MutationType mutationType, const std::chrono::high_resolution_clock::time_point& experimentStartTime)
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        std::string startingWord = GenerateRandomSequence(symbolSet, 5);
        TurtleState initialState{ 25, 49, N };
        int popSize = 50;
        int generations = 50;
        int ruleIterations = 3;

        Evolution evolution(popSize, startingWord, ruleIterations, initialState, generations, CHECKPOINT_DISTANCE, mutationType, CIRCULAR, constraintMatrix);
        evolution.SetMutationParams(symbolSet, mutationChance, expansionSize);

        evolution.Run(runIndex);
        float fitnessScore = evolution.GetBestIndividual().Fitness;

        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = endTime - startTime;

        LogResults(symbolSet, expansionSize, mutationChance, mutationType, fitnessScore, elapsed.count(), startingWord);

        // ETA Calculation
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> totalElapsed = now - experimentStartTime;
        double avgTimePerRun = totalElapsed.count() / (currentRun + 1);
        int remainingRuns = totalRuns - currentRun - 1;
        double estimatedSecondsLeft = avgTimePerRun * remainingRuns;

        int minutesLeft = static_cast<int>(estimatedSecondsLeft / 60);
        int secondsLeft = static_cast<int>(estimatedSecondsLeft) % 60;

        std::cout << "  Running experiment " << currentRun + 1 << " of " << totalRuns
            << " (" << remainingRuns << " left) — "
            << "ETA: " << minutesLeft << "m " << secondsLeft << "s\n";

        currentRun++;
    }

    std::string GenerateRandomSequence(const std::vector<char>& symbols, int length)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, symbols.size() - 1);

        std::string sequence;
        for (int i = 0; i < length; ++i)
        {
            sequence += symbols[dist(gen)];
        }
        return sequence;
    }

    void LogResults(const std::vector<char>& symbolSet, int expansionSize, const std::vector<int>& mutationChance,
        MutationType mutationType, float fitness, double timeTaken, std::string startingWord)
    {
        if (!logFile.is_open()) {
            std::cerr << "Log file is not open!" << std::endl;
            return;
        }

        logFile << "\"";
        for (char c : symbolSet) logFile << c << " ";
        logFile << "\",";

        logFile << expansionSize << ",";
        logFile << mutationChance[0] << "," << mutationChance[1] << "," << mutationChance[2] << ",";
        logFile << (mutationType == RULE ? "RULE" : "WORD") << ",";
        logFile << fitness << ",";
        logFile << timeTaken << "\n";

        logFile << startingWord << "\n";
        logFile.flush();
    }
};
