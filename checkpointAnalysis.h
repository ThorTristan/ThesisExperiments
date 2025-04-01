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
    Experiment1(int runsPerCombination) : runsPerCombination(runsPerCombination) {
        // Open the log file in append mode (create or add to it)
        logFile.open("experiment1_results.csv", std::ios::app);
        if (!logFile) {
            std::cerr << "Failed to open log file!" << std::endl;
        }

        // Write the header row to the CSV file only once (if it's empty)
        logFile << "Mutation Type,Checkpoint Pattern,Best Fitness,Time Taken (s)\n";
    }

    ~Experiment1() {
        // Close the log file properly when done
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void Run() {
        int popSize = 20;
        int iterations = 3;
        int generations = 5;

        TurtleState initialState{ 25, 49, N };

        for (MutationType mutation : {WORD, RULE}) {
            for (CheckpointPattern pattern : {LINEAR, CLUSTERED, SQUARE, CIRCULAR, RANDOM, NONE}) {
                for (int i = 0; i < runsPerCombination; ++i) {
                    auto startTime = std::chrono::high_resolution_clock::now();  // Start time

                    Evolution evolution(popSize, "F-F+F", iterations, initialState, generations, CHECKPOINT_DISTANCE, mutation, pattern);
                    evolution.Run();
                    float bestFitness = evolution.GetBestIndividual().Fitness;

                    auto endTime = std::chrono::high_resolution_clock::now();  // End time
                    std::chrono::duration<double> elapsed = endTime - startTime;  // Compute duration

                    LogResults(mutation, pattern, bestFitness, elapsed.count());  // Log results
                }
            }
        }
    }

private:
    int runsPerCombination;
    std::ofstream logFile;  // Output file for logging results

    void LogResults(MutationType mutation, CheckpointPattern pattern, float bestFitness, double timeTaken) {
        // Ensure the log file is open before writing
        if (!logFile.is_open()) {
            std::cerr << "Log file is not open!" << std::endl;
            return;
        }

        // Write the results in CSV format
        logFile << (mutation == WORD ? "WORD" : "RULE") << ",";  // Mutation Type
        logFile << (pattern == LINEAR ? "LINEAR" :
            pattern == CLUSTERED ? "CLUSTERED" :
            pattern == SQUARE ? "SQUARE" :
            pattern == CIRCULAR ? "CIRCULAR" :
            pattern == RANDOM ? "RANDOM" : "NONE") << ",";  // Checkpoint Pattern
        logFile << bestFitness << ",";  // Best Fitness
        logFile << timeTaken << "\n";  // Time Taken
        logFile.flush();  // Ensure data is written to the file immediately
    }
};
