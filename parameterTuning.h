#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include "individual.h"
#include "fitnessFunction.h"
#include "evolutionRM.h"

class Experiment2 
{
private:
    int iterations;
    std::ofstream logFile;  
    FitnessFunction FF;

    std::vector<std::vector<char>> symbolSets = 
    {
        {'F'},
        {'F', '+', '-'},
        {'F', '+', '-', '[', ']'},
        {'F', '[', ']'}
    };

    std::vector<std::vector<int>> mutationChances = 
    {
        {60, 30, 10},
        {40, 30, 30},
        {50, 40, 10},
        {100, 0, 0}
    };

    std::vector<int> expansionSizes = { 1, 3, 5, 7 };

    std::vector<std::vector<int>> constraintMatrix = {
{0, 1, 2, 1},
{1, 0, 1, 2},
{2, 0, 0, 1},
{1, 2, 1, 0}
    };

public:
    Experiment2(int iter) : iterations(iter) 
    {
        
        logFile.open("fitness_log.csv", std::ios::app);
        if (!logFile) 
        {
            std::cerr << "Failed to open log file!" << std::endl;
        }

        
        std::ofstream clearFile("fitness_log.csv", std::ios::trunc);
        clearFile.close(); 

        
        logFile << "Symbol Set,Expansion Size,Mutation Chance,Mutation Type,Best Fitness,Time Taken (s)\n";
    }

    ~Experiment2() 
    {
        
        if (logFile.is_open()) 
        {
            logFile.close();
        }
    }

    void Run() {
        std::vector<MutationType> mutationTypes = { RULE, WORD }; 

        for (MutationType mutationType : mutationTypes) 
        {  
            for (const auto& symbolSet : symbolSets) 
            {
                for (int expansionSize : expansionSizes) 
                {
                    for (const auto& mutationChance : mutationChances) 
                    {
                        RunTest(symbolSet, expansionSize, mutationChance, mutationType);
                    }
                }
            }
        }
    }

    void RunTest(const std::vector<char>& symbolSet, int expansionSize, const std::vector<int>& mutationChance, MutationType mutationType) 
    {
        auto startTime = std::chrono::high_resolution_clock::now(); 

        std::string startingWord = GenerateRandomSequence(symbolSet, 5);
        TurtleState initialState{ 25, 49, N };
        int popSize = 5;
        int generations = 10;
        int ruleIterations = 3;

        Evolution evolution(popSize, startingWord, ruleIterations, initialState, generations, CHECKPOINT_DISTANCE, mutationType, LINEAR, constraintMatrix);
        evolution.SetMutationParams(symbolSet, mutationChance, expansionSize);

        //evolution.Run();
        float fitnessScore = evolution.GetBestIndividual().Fitness;

        auto endTime = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = endTime - startTime; 

        LogResults(symbolSet, expansionSize, mutationChance, mutationType, fitnessScore, elapsed.count());
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
        MutationType mutationType, float fitness, double timeTaken) 
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

        logFile.flush(); 
    }
};
