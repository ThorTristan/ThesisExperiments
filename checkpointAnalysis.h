#include "common.h"
#include "structs.h"
#include "evolutionRM.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

class Experiment1 
{
public:
    Experiment1(int runsPerCombination) : runsPerCombination(runsPerCombination)
    {
        
        logFile.open("experiment1_results.csv", std::ios::app);
        if (!logFile) 
        {
            std::cerr << "Failed to open log file!" << std::endl;
        }

        
        logFile << "Mutation Type,Checkpoint Pattern,Best Fitness,Time Taken (s)\n";
    }

    ~Experiment1() 
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
            for (CheckpointPattern pattern : {LINEAR, CLUSTERED, SQUARE, CIRCULAR, RANDOM, NONE}) 
            {
                for (int i = 0; i < runsPerCombination; ++i) 
                {
                    auto startTime = std::chrono::high_resolution_clock::now();  

                    Evolution evolution(popSize, "F-F+F", iterations, initialState, generations, CHECKPOINT_DISTANCE, mutation, pattern);
                    evolution.Run();
                    float bestFitness = evolution.GetBestIndividual().Fitness;

                    auto endTime = std::chrono::high_resolution_clock::now();  
                    std::chrono::duration<double> elapsed = endTime - startTime; 

                    LogResults(mutation, pattern, bestFitness, elapsed.count());  
                }
            }
        }
    }

private:
    int runsPerCombination;
    std::ofstream logFile;  

    void LogResults(MutationType mutation, CheckpointPattern pattern, float bestFitness, double timeTaken) 
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
        logFile << timeTaken << "\n";  
        logFile.flush(); 
    }
};
