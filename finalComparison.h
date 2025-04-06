#pragma once
#include "common.h"
#include "structs.h"
#include "evolutionRM.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

class FinalComparisonExperiment
{
public:
    FinalComparisonExperiment(CheckpointPattern bestPatternWord,
        const std::vector<std::vector<int>>& bestMatrixWord,
        CheckpointPattern bestPatternRule,
        const std::vector<std::vector<int>>& bestMatrixRule)
        : bestPatternWord(bestPatternWord),
        bestMatrixWord(bestMatrixWord),
        bestPatternRule(bestPatternRule),
        bestMatrixRule(bestMatrixRule)
    {
        logFile.open("final_comparison_results.csv", std::ios::app);
        if (!logFile)
        {
            std::cerr << "Failed to open log file!" << std::endl;
        }

        logFile << "Mutation Type,Run Index,Best Fitness,Time Taken (s)\n";
    }

    ~FinalComparisonExperiment()
    {
        if (logFile.is_open())
        {
            logFile.close();
        }
    }

    void Run(const std::vector<char>& symbolSet, int expansionSize, const std::vector<int>& mutationChance)
    {
        RunWithMutation(symbolSet, expansionSize, mutationChance, WORD, bestPatternWord, bestMatrixWord);
        RunWithMutation(symbolSet, expansionSize, mutationChance, RULE, bestPatternRule, bestMatrixRule);
    }

private:
    CheckpointPattern bestPatternWord;
    const std::vector<std::vector<int>>& bestMatrixWord;
    CheckpointPattern bestPatternRule;
    const std::vector<std::vector<int>>& bestMatrixRule;

    std::ofstream logFile;

    void RunWithMutation(const std::vector<char>& symbolSet, int expansionSize, const std::vector<int>& mutationChance,
        MutationType mutationType, CheckpointPattern pattern, const std::vector<std::vector<int>>& matrix)
    {
        int popSize = 20;
        int iterations = 3;
        int generations = 5;
        TurtleState initialState{ 25, 49, N };

        for (int i = 0; i < 100; ++i)
        {
            auto startTime = std::chrono::high_resolution_clock::now();

            Evolution evolution(popSize, "F-F+F", iterations, initialState, generations, CHECKPOINT_DISTANCE, mutationType, pattern, matrix);
            evolution.SetMutationParams(symbolSet, mutationChance, expansionSize);

            evolution.Run();
            float bestFitness = evolution.GetBestIndividual().Fitness;

            auto endTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = endTime - startTime;

            LogResult(mutationType, i, bestFitness, elapsed.count());
        }
    }

    void LogResult(MutationType mutation, int runIndex, float bestFitness, double timeTaken)
    {
        if (!logFile.is_open())
        {
            std::cerr << "Log file is not open!" << std::endl;
            return;
        }

        logFile << (mutation == WORD ? "WORD" : "RULE") << ",";
        logFile << runIndex << ",";
        logFile << bestFitness << ",";
        logFile << timeTaken << "\n";
        logFile.flush();
    }
};
