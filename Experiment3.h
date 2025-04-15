#pragma once
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <thread>
#include <mutex>
#include <functional>
#include <algorithm>
#include "individual.h"
#include "fitnessFunction.h"
#include "evolutionRM.h"

class Experiment3 {
private:
    int iterations;
    std::ofstream logFile;
    FitnessFunction FF;

    int totalRuns = 0;
    int currentRun = 0;

    int runIndex = 0;

    std::vector<std::vector<char>> symbolSets = {
        {'F'},
        {'F', '+', '-'},
        {'F', '+', '-', '[', ']'},
        {'F', '[', ']'}
    };

    std::vector<std::vector<int>> mutationChances = {
        {60, 20, 20},
        {25, 50, 25},
        {50, 25, 25},
        {25, 25, 50},
        {20, 20, 60},
        {34, 33, 33}
    };

    std::vector<int> expansionSizes = { 1, 3, 5, 7 };

    std::vector<std::vector<int>> constraintMatrix = {
        {0, 0, 1, 1},
        {0, 0, 1, 0},
        {1, 1, 0, 0},
        {1, 0, 0, 0}
    };

    std::mutex logMutex;

public:
    Experiment3(int iter) : iterations(iter) {
        logFile.open("fitness_log.csv", std::ios::trunc);
        if (!logFile) {
            std::cerr << "Failed to open log file!" << std::endl;
        }

        std::ofstream clearCSV("best_individuals_log.csv", std::ios::trunc);
        clearCSV << "Run Index,Generation,Best Fitness,Genetic Code\n";

        std::ofstream clearJSON("genetic_log_run_" + std::to_string(runIndex) + ".json", std::ios::trunc);
        clearJSON << "{}";

        totalRuns = symbolSets.size() * expansionSizes.size() * mutationChances.size() * 2;

        logFile << "Symbol Set,Expansion Size,Mutation Chance,Mutation Type,Best Fitness,Time Taken (s)\n";
    }

    ~Experiment3() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void Run() {
        std::vector<std::thread> threads;
        std::vector<MutationType> mutationTypes = { WORD, RULE };
        auto experimentStartTime = std::chrono::high_resolution_clock::now();

        int hardwareThreads = std::thread::hardware_concurrency();
        std::cout << "Using up to " << hardwareThreads << " threads...\n";

        for (int iteration = 0; iteration < iterations; ++iteration) {
            std::cout << "Starting repetition " << iteration + 1 << " of " << iterations << "\n";

            std::vector<std::function<void()>> tasks;

            for (MutationType mutationType : mutationTypes) {
                for (const auto& symbolSet : symbolSets) {
                    for (int expansionSize : expansionSizes) {
                        for (const auto& mutationChance : mutationChances) {
                            tasks.emplace_back([this, symbolSet, expansionSize, mutationChance, mutationType, experimentStartTime]() {


                                this->RunTest(symbolSet, expansionSize, mutationChance, mutationType, experimentStartTime);
                                });
                        }
                    }
                }
            }

            size_t index = 0;
            while (index < tasks.size()) {
                size_t batchSize = std::min((size_t)hardwareThreads, tasks.size() - index);
                threads.clear();

                for (size_t i = 0; i < batchSize; ++i) {
                    threads.emplace_back(tasks[index + i]);
                }

                for (auto& t : threads) {
                    if (t.joinable()) t.join();
                }

                index += batchSize;
            }
        }
    }

    void RunTest(const std::vector<char>& symbolSet, int expansionSize, const std::vector<int>& mutationChance,
        MutationType mutationType, std::chrono::high_resolution_clock::time_point start) {
        auto startTime = std::chrono::high_resolution_clock::now();

        std::string startingWord = GenerateRandomSequence(symbolSet, 5);
        TurtleState initialState{ 25, 49, N };
        int popSize = 50;
        int generations = 50;
        int ruleIterations = 3;

        Evolution evolution(popSize, startingWord, ruleIterations, initialState, generations,
            CHECKPOINT_DISTANCE, mutationType, CIRCULAR, constraintMatrix);
        evolution.SetMutationParams(symbolSet, mutationChance, expansionSize);

        int currentIndex;
        {
            std::lock_guard<std::mutex> guard(logMutex);
            currentIndex = runIndex++;
        }
        evolution.Run(currentIndex);
        float fitnessScore = evolution.GetBestIndividual().Fitness;

        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = endTime - startTime;

        LogResults(symbolSet, expansionSize, mutationChance, mutationType, fitnessScore, elapsed.count(), startingWord);

        int runNumber = ++currentRun;
        int remaining = totalRuns * iterations - runNumber;
        double etaSeconds = std::chrono::duration<double>(endTime - start).count() / runNumber * remaining;

        std::cout << "  Completed experiment " << runNumber << " / " << totalRuns * iterations
            << " | ETA: ~" << static_cast<int>(etaSeconds) << "s\n";
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
        MutationType mutationType, float fitness, double timeTaken, const std::string& startingWord) {
        std::lock_guard<std::mutex> guard(logMutex);

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
        logFile << startingWord << "\n";
        logFile.flush();
    }
};
