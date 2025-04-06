#include "common.h"
#include "structs.h"
#include "evolutionRM.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>


// now i need to render it better
// set the parameters
// then i can start the run :DDD


class Experiment1
{
public:
    Experiment1(int runsPerCombination) : runsPerCombination(runsPerCombination)
    {

        std::ofstream clearFile("experiment1_results.csv", std::ios::trunc);
        clearFile.close();

        logFile.open("experiment1_results.csv", std::ios::app);
        if (!logFile)
        {
            std::cerr << "Failed to open log file!" << std::endl;
        }



        logFile << "Mutation Type,Checkpoint Pattern,Best Fitness,Genetic Code,Rule,Starting Rule,Time Taken (s)\n";

        bestEachGeneration.resize(runsPerCombination);
    }

    ~Experiment1()
    {
        if (logFile.is_open())
        {
            logFile.close();
        }
    }

    void Run()
    {
        int popSize = 100;
        int iterations = 3;
        int generations = 20;

        TurtleState initialState{ 25, 49, N };

        std::vector<std::vector<int>> constraintMatrix = {
            {0, 1, 2, 1},
            {1, 0, 1, 2},
            {2, 0, 0, 1},
            {1, 2, 1, 0}
        };

        for (MutationType mutation : {WORD, RULE})
        {
            for (CheckpointPattern pattern : {LINEAR, SQUARE, CIRCULAR, NONE})
            {
                for (int i = 0; i < runsPerCombination; ++i)
                {
                    auto startTime = std::chrono::high_resolution_clock::now();

                    Evolution evolution(popSize, "F-F+F", iterations, initialState, generations, CHECKPOINT_DISTANCE, mutation, pattern, constraintMatrix);
                    
                    evolution.SetMutationParams({ 'F','+','-','[',']' }, { 60,20,20 }, 5);
                    evolution.Run(bestEachGeneration, i);

                    float bestFitness = evolution.GetBestIndividual().Fitness;

                    // Convert best individual stack to string
                    std::string bestIndividualWord = evolution.StackToString(evolution.GetBestIndividual().individual);

                    // Serialize rules and starting rules
                    std::string bestIndividualRule = evolution.SerializeRules(evolution.GetBestIndividual().rule);
                    std::string bestIndividualStartingRule = evolution.SerializeRules(evolution.GetBestIndividual().StartingRule);

                    auto endTime = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> elapsed = endTime - startTime;

                    // Log all the results
                    LogResults(mutation, pattern, bestFitness, bestIndividualWord, bestIndividualRule, bestIndividualStartingRule, elapsed.count());
                }
            }
        }
    }

private:
    int runsPerCombination;
    std::ofstream logFile;

    std::vector<std::vector<Individual>> bestEachGeneration;

    // Updated LogResults function to log all required data
    void LogResults(MutationType mutation, CheckpointPattern pattern, float bestFitness,
        const std::string& bestIndividualWord, const std::string& bestIndividualRule,
        const std::string& bestIndividualStartingRule, double timeTaken)
    {
        if (!logFile.is_open())
        {
            std::cerr << "Log file is not open!" << std::endl;
            return;
        }

        // Write to log file, including the new data
        logFile << (mutation == WORD ? "WORD" : "RULE") << ",";
        logFile << (pattern == LINEAR ? "LINEAR" :
            pattern == CLUSTERED ? "CLUSTERED" :
            pattern == SQUARE ? "SQUARE" :
            pattern == CIRCULAR ? "CIRCULAR" :
            pattern == RANDOM ? "RANDOM" : "NONE") << ",";
        logFile << bestFitness << ",";
        //logFile << bestIndividualWord << ",";  // Log best individual genetic code
        //logFile << bestIndividualRule << ",";  // Log rule
        //logFile << bestIndividualStartingRule << ",";  // Log starting rule
        logFile << timeTaken << "\n";
        logFile.flush();
        std::cout << "Evolution complete. Best individuals data saved to experiment1_results.csv" << std::endl;

    }
};


