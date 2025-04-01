#pragma once
#include "individual.h"
#include "fitnessFunction.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <random>
#include "evolutionRM.h"


class Experiment2 
{
private:
    int iterations;
    std::ofstream logFile;
    FitnessFunction FF;

    std::vector<std::vector<char>> symbolSets = {
        {'F'},
        {'F', '+', '-'},
        {'F', '+', '-', '[', ']'},
        {'F', '[', ']'}
    };

    std::vector<std::vector<int>> mutationChances = {
        {60, 30, 10},
        {40, 30, 30},
        {50, 40, 10},
        {100, 0, 0}
    };

    std::vector<int> expansionSizes = { 1, 3, 5, 7 };

public:
    Experiment2(int iter) : iterations(iter) {
        logFile.open("experiment_results.txt");
        if (!logFile) {
            std::cerr << "Failed to open log file!" << std::endl;
        }
    }

    ~Experiment2() {
        logFile.close();
    }

    void Run() {
        for (const auto& symbolSet : symbolSets) {
            for (int expansionSize : expansionSizes) {
                for (const auto& mutationChance : mutationChances) {
                    RunTest(symbolSet, expansionSize, mutationChance);
                }
            }
        }
    }

    void RunTest(const std::vector<char>& symbolSet, int expansionSize, const std::vector<int>& mutationChance) {
        std::string startingWord = GenerateRandomSequence(symbolSet, 5);
        TurtleState initialState{ 25,49,N };
        int popSize = 100;
        int generations = 50;
        int ruleIterations = 3;
        Evolution evolution(popSize, startingWord, ruleIterations, initialState, generations, CHECKPOINT_DISTANCE, WORD, CIRCULAR);
        evolution.SetMutationParams(symbolSet,  mutationChance, expansionSize);

        //float fitnessScore = evolution.Run();
        evolution.Run();
        float fitnessScore = evolution.GetBestIndividual().Fitness;
        
        LogResults(symbolSet, expansionSize, mutationChance, fitnessScore);
    }

    std::string GenerateRandomSequence(const std::vector<char>& symbols, int length) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, symbols.size() - 1);

        std::string sequence;
        for (int i = 0; i < length; ++i) {
            sequence += symbols[dist(gen)];
        }
        return sequence;
    }

    void LogResults(const std::vector<char>& symbolSet, int expansionSize, const std::vector<int>& mutationChance, float fitness) {
        logFile << "Symbol Set: ";
        for (char c : symbolSet) logFile << c << " ";
        logFile << " | Expansion Size: " << expansionSize;
        logFile << " | Mutation Chance: {" << mutationChance[0] << ", " << mutationChance[1] << ", " << mutationChance[2] << "}";
        logFile << " | Fitness: " << fitness << std::endl;
    }
};
