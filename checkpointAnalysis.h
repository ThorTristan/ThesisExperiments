#pragma once

#include "common.h"
#include "structs.h"
#include "evolutionRM.h"




class Experiment 
{
public:
    Experiment(int runsPerCombination) : runsPerCombination(runsPerCombination) {}

    void Run()
    {
        int popSize = 50;
        int iterations = 3;
        int generations = 30;
        
        TurtleState initialState{ 25,49,N };

        for (MutationType mutation : {WORD, RULE})
        {
            for (CheckpointPattern pattern : {SQUARE, CIRCULAR})
            {
                for (int i = 0; i < runsPerCombination; ++i)
                {

                    Evolution evolution(popSize, "F-F+F", iterations, initialState, generations, CHECKPOINT_DISTANCE, mutation, pattern);
                    evolution.Run();
                    bestIndividuals.push_back(evolution.GetBestIndividual());
                }
            }
        }
    }

private:
    int runsPerCombination;
    std::vector<Individual> bestIndividuals;
};