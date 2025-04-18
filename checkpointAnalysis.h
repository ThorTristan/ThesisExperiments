#include "common.h"
#include "structs.h"
#include "evolutionRM.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>


// Now get rid of none and maybe do a random one ?


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
        int popSize = 50;
        int iterations = 3;
        int generations = 50;

        TurtleState initialState{ 25, 49, N };

        std::vector<std::vector<int>> constraintMatrix = {
            {0, 1, 2, 1},
            {1, 0, 1, 2},
            {2, 0, 0, 1},
            {1, 2, 1, 0}
        };

        std::vector<MutationType> mutationTypes = { WORD, RULE };
        std::vector<CheckpointPattern> patterns = { LINEAR, SQUARE, CIRCULAR, NONE };

        int totalRuns = mutationTypes.size() * patterns.size() * runsPerCombination;
        int currentRun = 1;

        for (MutationType mutation : mutationTypes)
        {
            for (CheckpointPattern pattern : patterns)
            {
                for (int i = 0; i < runsPerCombination; ++i)
                {
                    auto startTime = std::chrono::high_resolution_clock::now();

                    Evolution evolution(popSize, "F-F+F", iterations, initialState, generations, CHECKPOINT_DISTANCE, mutation, pattern, constraintMatrix);
                    
                    evolution.SetMutationParams({ 'F','+','-','[',']' }, { 60,20,20 }, 5);
                    evolution.Run(i);

                    float bestFitness = evolution.GetBestIndividual().Fitness;


                    std::string bestIndividualWord = evolution.StackToString(evolution.GetBestIndividual().individual);


                    std::string bestIndividualRule = evolution.SerializeRules(evolution.GetBestIndividual().rule);
                    std::string bestIndividualStartingRule = evolution.SerializeRules(evolution.GetBestIndividual().StartingRule);

                    auto endTime = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> elapsed = endTime - startTime;


                    LogResults(mutation, pattern, bestFitness, bestIndividualWord, bestIndividualRule, bestIndividualStartingRule, elapsed.count());

                    std::cout << "Completed run " << currentRun << " / " << totalRuns << "\n";
                    ++currentRun;
                }
            }
        }
    }

private:
    int runsPerCombination;
    std::ofstream logFile;

    std::vector<std::vector<Individual>> bestEachGeneration;


    void LogResults(MutationType mutation, CheckpointPattern pattern, float bestFitness,
        const std::string& bestIndividualWord, const std::string& bestIndividualRule,
        const std::string& bestIndividualStartingRule, double timeTaken)
    {
        if (!logFile.is_open())
        {
            std::cerr << "Log file is not open!" << std::endl;
            return;
        }


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


