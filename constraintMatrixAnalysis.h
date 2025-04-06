#pragma once
#include "common.h"
#include "structs.h"
#include "evolutionRM.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

class ExperimentConstraints
{
public:
    ExperimentConstraints(int runsPerCombination, const std::vector<std::vector<std::vector<int>>>& constraintMatrices)
        : runsPerCombination(runsPerCombination), constraintMatrices(constraintMatrices)
    {
        logFile.open("experiment_constraints_results.csv", std::ios::app);
        if (!logFile)
        {
            std::cerr << "Failed to open log file!" << std::endl;
        }

        logFile << "Mutation Type,Matrix Index,Best Fitness,Time Taken (s)\n";
    }

    ~ExperimentConstraints()
    {
        if (logFile.is_open())
        {
            logFile.close();
        }
    }

    void Run() {
        int popSize = 20;
        int iterations = 3;
        int generations = 5;

        TurtleState initialState{ 25, 49, N };

        for (MutationType mutation : {WORD, RULE})
        {
            for (size_t matrixIndex = 0; matrixIndex < constraintMatrices.size(); ++matrixIndex)
            {
                const auto& matrix = constraintMatrices[matrixIndex];

                for (int i = 0; i < runsPerCombination; ++i)
                {
                    auto startTime = std::chrono::high_resolution_clock::now();

                    Evolution evolution(popSize, "F-F+F", iterations, initialState, generations, CHECKPOINT_DISTANCE, mutation,LINEAR, matrix);
                    evolution.Run();
                    float bestFitness = evolution.GetBestIndividual().Fitness;

                    auto endTime = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> elapsed = endTime - startTime;

                    LogResults(mutation, matrixIndex, bestFitness, elapsed.count());
                }
            }
        }
    }

private:
    int runsPerCombination;
    const std::vector<std::vector<std::vector<int>>>& constraintMatrices;
    std::ofstream logFile;

    void LogResults(MutationType mutation, size_t matrixIndex, float bestFitness, double timeTaken)
    {
        if (!logFile.is_open())
        {
            std::cerr << "Log file is not open!" << std::endl;
            return;
        }

        logFile << (mutation == WORD ? "WORD" : "RULE") << ",";
        logFile << matrixIndex << ",";
        logFile << bestFitness << ",";
        logFile << timeTaken << "\n";
        logFile.flush();
    }
};
