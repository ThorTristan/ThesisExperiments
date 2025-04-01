#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include "individual.h"
#include "fitnessFunction.h"
#include "evolutionRM.h"

class Experiment2 {
private:
    int iterations;
    std::ofstream logFile;  // Output file for logging results
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
        // Open logFile in append mode for CSV output
        logFile.open("fitness_log.csv", std::ios::app);
        if (!logFile) {
            std::cerr << "Failed to open log file!" << std::endl;
        }

        // Clear the file first (truncate mode)
        std::ofstream clearFile("fitness_log.csv", std::ios::trunc);
        clearFile.close(); // Close immediately after clearing

        // Write header to the CSV file (this only happens once)
        logFile << "Symbol Set,Expansion Size,Mutation Chance,Mutation Type,Best Fitness,Time Taken (s)\n";
    }

    ~Experiment2() {
        // Ensure the file is closed properly
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void Run() {
        std::vector<MutationType> mutationTypes = { RULE, WORD }; // Define once before looping

        for (MutationType mutationType : mutationTypes) {  // First loop over mutation types
            for (const auto& symbolSet : symbolSets) {
                for (int expansionSize : expansionSizes) {
                    for (const auto& mutationChance : mutationChances) {
                        RunTest(symbolSet, expansionSize, mutationChance, mutationType);
                    }
                }
            }
        }
    }

    void RunTest(const std::vector<char>& symbolSet, int expansionSize, const std::vector<int>& mutationChance, MutationType mutationType) {
        auto startTime = std::chrono::high_resolution_clock::now(); // Start time

        std::string startingWord = GenerateRandomSequence(symbolSet, 5);
        TurtleState initialState{ 25, 49, N };
        int popSize = 5;
        int generations = 5;
        int ruleIterations = 3;

        Evolution evolution(popSize, startingWord, ruleIterations, initialState, generations, CHECKPOINT_DISTANCE, mutationType, CIRCULAR);
        evolution.SetMutationParams(symbolSet, mutationChance, expansionSize);

        evolution.Run();
        float fitnessScore = evolution.GetBestIndividual().Fitness;

        auto endTime = std::chrono::high_resolution_clock::now(); // End time
        std::chrono::duration<double> elapsed = endTime - startTime; // Compute duration

        LogResults(symbolSet, expansionSize, mutationChance, mutationType, fitnessScore, elapsed.count()); // Pass elapsed time
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

    void LogResults(const std::vector<char>& symbolSet, int expansionSize, const std::vector<int>& mutationChance,
        MutationType mutationType, float fitness, double timeTaken) {

        // Ensure file is open before writing
        if (!logFile.is_open()) {
            std::cerr << "Log file is not open!" << std::endl;
            return;
        }

        // Write results in CSV format
        logFile << "\"";
        for (char c : symbolSet) logFile << c << " ";  // Symbol Set
        logFile << "\",";
        logFile << expansionSize << ",";  // Expansion Size
        logFile << mutationChance[0] << "," << mutationChance[1] << "," << mutationChance[2] << ",";  // Mutation Chance
        logFile << (mutationType == RULE ? "RULE" : "WORD") << ",";  // Mutation Type
        logFile << fitness << ",";  // Best Fitness
        logFile << timeTaken << "\n";  // Time Taken

        logFile.flush(); // Ensure data is written to file immediately
    }
};
