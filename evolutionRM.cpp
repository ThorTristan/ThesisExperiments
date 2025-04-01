#include "evolutionRM.h"
#include <fstream>

extern GridManager GM;
extern RenderManager RM;
extern FitnessFunction FF;
//extern SceneManager SM;



/*
    Next: Work out why the small grids have such high fitness? is it not rendering the whole thing?
    print out the grid of one of them
    throw exception when pressing somthing
*/





Evolution::Evolution(int popSize, std::string startingWord, int ruleIterations, TurtleState initialState, int numGenerations, FitnessType chosenFitness, MutationType chosenMutation, CheckpointPattern chosenPattern)
{

    m_PopSize = popSize;
    m_StartingWord = startingWord;
    m_RuleIterations = ruleIterations;
    m_InitialState = initialState;
    m_NumberOfGenerations = numGenerations;
    m_Complete = false;
    m_ChosenFitness = chosenFitness;
    m_MutationChoice = chosenMutation;
    m_Pattern = chosenPattern;


}


std::vector<Single> Evolution::GetPopulation() { return m_Population; }
Individual Evolution::GetBestIndividual() { return m_BestIndividual; }

void Evolution::InitialisePopulation()
{


    m_Population.clear();
    m_Population.reserve(m_PopSize);

    for (int i = 0; i < m_PopSize; ++i)
    {
        m_Population.emplace_back(m_StartingWord, m_RuleIterations, m_InitialState,m_Pattern);
        m_Population[i].Evaluate(m_ChosenFitness);
    }
}

void Evolution::SortPopulation()
{

    std::sort(m_Population.begin(), m_Population.end(),
        [](Single& a, Single& b)
        {
            return a.GetIndividual().Fitness > b.GetIndividual().Fitness;
        });

    m_BestIndividual = m_Population.front().GetIndividual();




}


void Evolution::RenderIndividual(Individual individual)
{

  

    int gridHeight = 50;
    int gridWidth = 50;
    RM.prepareScene();

    Turtle turtle(m_InitialState,individual.individual, gridHeight, gridWidth);

    std::vector<int> gridVector = turtle.returnGridVector();



    int numCheckpoints = 4;

    GM.RenderGrid(gridVector, FF.CreateCheckpoints(50, 50, m_Pattern, numCheckpoints),100, 100, 150, 150, gridHeight, gridWidth);
    RenderFitness(individual);


    RM.presentScene();



}

void Evolution::RenderFitness(Individual individual)
{
    int fitness = individual.Fitness;

    SDL_Color white = { 255, 255, 255, 255 };

    std::string text = "Fitness: " + std::to_string(fitness);

    SDL_Surface* surface = TTF_RenderText_Solid(RM.font, text.c_str(), white);
    if (!surface)
    {
        std::cerr << "Failed to render text surface: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(RM.renderer, surface);
    if (!texture)
    {
        std::cerr << "Failed to create texture from surface: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect rect = { 250, 98, surface->w, surface->h }; // Adjust position as needed
    SDL_RenderCopy(RM.renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);


}




void Evolution::Selection()
{

    m_Population.erase(m_Population.begin() + m_Population.size() / 2, m_Population.end());
}

void Evolution::SetMutationParams(std::vector<char> symbolSet, std::vector<int> mutationParams,int expansionSize)
{

    m_SymbolSet = symbolSet;
    m_MutationChances = mutationParams;
    m_ExpansionSize = expansionSize;



}

void Evolution::Mutation()
{
    size_t currentSize = m_Population.size();
    m_Population.insert(m_Population.end(), m_Population.begin(), m_Population.end());

    for (int i = 0; i < currentSize; i++)
    {

        switch (m_MutationChoice)
        {
        case RULE:
            m_Population[i].MutateRule(m_SymbolSet, m_MutationChances);
            break;
        case WORD:
            m_Population[i].MutateWord(m_SymbolSet, m_ExpansionSize);
            break;
        default:
            std::cout << "default mutation chosen" << std::endl;
            m_Population[i].MutateRule(m_SymbolSet, m_MutationChances);
            break;
        }
        


    }


}

float Evolution::Evaluation()
{
    float totalFitness = 0.0f;

    for (auto& individual : m_Population)
    {
        individual.Evaluate(m_ChosenFitness);
        totalFitness += individual.GetIndividual().Fitness;
    }
    float averageFitness = totalFitness / m_Population.size();

    return averageFitness;
}

std::vector<int> Evolution::GrammarToGrid(Individual individual)
{

    int rows = 50;
    int cols = 50;

    Turtle turtle(m_InitialState, individual.individual, rows, cols);



    std::vector<int> grid = turtle.returnGridVector();


    return grid;



}


void Evolution::PrintGrid(Individual individual)
{

    int rows = 50;
    int cols = 50;

    std::vector<int> grid = GrammarToGrid(individual);

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            std::cout << grid[y * cols + x] << " ";
        }
        std::cout << "\n";
    }


}

void Evolution::PrintRuleSet(Individual individual)
{


    for (const auto& rule : individual.rule)
    {
        std::cout << rule.first << " -> ";
        for (const auto& replacement : rule.second)
        {
            std::cout << "{" << replacement.first << ", " << replacement.second << "} ";
        }
    }




}


void Evolution::Run()
{
    float averageFitness = 0.0f;

    if (!m_Complete)
    {
        InitialisePopulation();



        // Open file again for writing
        std::ofstream logFile("fitness_log.csv", std::ios::app);
        logFile << "Generation,AverageFitness\n"; // Write the header

        for (int i = 0; i < m_NumberOfGenerations; i++)
        {
            SortPopulation();
            RenderIndividual(m_BestIndividual);

            Selection();
            Mutation();
            averageFitness = Evaluation();

            // Append new data
            logFile << i + 1 << "," << averageFitness << "\n";

            // Print for debugging
            std::cout << "Generation: " << i + 1 << "/" << m_NumberOfGenerations
                << " | Avg Fitness: " << averageFitness << std::endl;

            SDL_Delay(100);
        }

        logFile.close(); // Close file after logging all generations

        m_Complete = true;
        std::cout << "Evolution complete. Fitness data saved to fitness_log.csv" << std::endl;
    }
}






