#pragma once
#include "common.h"
#include "structs.h"
#include "evolutionRM.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>


//testing different constraint matrices
// first i need to test the FF2
// so i need to find an l system that covers all checkpoints
// then start testing

std::vector<std::vector<std::vector<int>>> constraintMatrices = {
    { // First constraint matrix
        {0, 1, 2, 1},
        {1, 0, 1, 2},
        {2, 0, 0, 1},
        {1, 2, 1, 0}
    }
    //,
    //{ // Second constraint matrix
    //    {0, 1, 0, 1},
    //    {1, 0, 1, 0},
    //    {0, 1, 0, 1},
    //    {1, 0, 1, 0}
    //},
    //{ // Third constraint matrix
    //    {0, 2, 1, 2},
    //    {2, 0, 1, 1},
    //    {1, 1, 0, 2},
    //    {2, 1, 2, 0}
    //},
    //{ // Fourth constraint matrix
    //    {0, 0, 1, 1},
    //    {0, 0, 1, 0},
    //    {1, 1, 0, 0},
    //    {1, 0, 0, 0}
    //}
};

std::vector<std::vector<int>> constraintMatrix =
{
    {0, 0, 1, 1},
    {0, 0, 1, 0},
    {1, 1, 0, 0},
    {1, 0, 0, 0}
};





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
        int popSize = 25;
        int iterations = 3;
        int generations = 100;

        TurtleState initialState{ 25, 49, N };

        for (MutationType mutation : {WORD})
        {
            for (size_t matrixIndex = 0; matrixIndex < constraintMatrices.size(); ++matrixIndex)
            {
                const auto& matrix = constraintMatrices[matrixIndex];

                for (int i = 0; i < runsPerCombination; ++i)
                {
                    auto startTime = std::chrono::high_resolution_clock::now();

                    Evolution evolution(popSize, "F-F+F", iterations, initialState, generations, CHECKPOINT_DISTANCE, mutation,CIRCULAR, matrix);

                    evolution.SetMutationParams({ 'F','+','-','[',']' }, { 80,10,10 }, 5);
                    evolution.Run(bestEachGeneration, i);
                    float bestFitness = evolution.GetBestIndividual().Fitness;
                    Individual ind = evolution.GetBestIndividual();
                    
                    if (bestFitness > 2000)
                    {
                        evolution.PrintIndividual(ind.individual);
                    }

                    auto endTime = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> elapsed = endTime - startTime;

                    LogResults(mutation, matrixIndex, bestFitness, elapsed.count());
                }
            }
        }
    }

    void RunSingleTest()
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        // Choose specific parameters for the test
        std::vector<char> symbolSet = { 'F', '+', '-', '[', ']' };
        int expansionSize = 5;
        std::vector<int> mutationChance = { 60, 30, 10 };
        MutationType mutationType = WORD;
        std::string startingWord = "F-F+F";  // or GenerateRandomSequence(symbolSet, 5);

        TurtleState initialState{ 25, 49, N };
        int popSize = 25;
        int generations = 100;
        int ruleIterations = 3;

        Evolution evolution(popSize, startingWord, ruleIterations, initialState, generations, CHECKPOINT_DISTANCE, mutationType, CIRCULAR, constraintMatrix);
        evolution.SetMutationParams(symbolSet, mutationChance, expansionSize);

        // Optional: collect best-of-generation if needed later
        // std::vector<std::vector<Individual>> bestEachGeneration;
        // evolution.Run(bestEachGeneration, 0);

        // Just get one fitness value from initial or best individual
        float fitnessScore = evolution.GetBestIndividual().Fitness;
        Individual ind = evolution.GetBestIndividual();

        std::cout << "Single Test Fitness: " << fitnessScore << std::endl;
        std::cout << "Best Individual L-System:\n";
        //evolution.PrintIndividual(ind.individual);

        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = endTime - startTime;

        std::cout << "Time Taken: " << elapsed.count() << " seconds" << std::endl;
    }


private:
    int runsPerCombination;
    const std::vector<std::vector<std::vector<int>>>& constraintMatrices;
    std::ofstream logFile;

    std::vector<std::vector<Individual>> bestEachGeneration;


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
