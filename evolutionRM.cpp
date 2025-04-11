#include "evolutionRM.h"
#include <fstream>
#include <ctime>  // for std::time and std::strftime
#include <sstream> // for std::ostringstream
//#include <nlohmann/json.hpp> 


#define _CRT_SECURE_NO_WARNINGS

extern GridManager GM;
extern RenderManager RM;
extern FitnessFunction FF;



// Helper function to generate a timestamp string
std::string GetTimestamp()
{
    std::time_t now = std::time(nullptr);
    std::tm timeinfo;
    localtime_s(&timeinfo, &now);  // Safe, no warning
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", &timeinfo);
    return std::string(buffer);
}

void Evolution::PrintIndividual(std::stack<char> ind)
{
    std::string output;

    // Pop from a copy so we don't modify the original
    while (!ind.empty())
    {
        output += ind.top();
        ind.pop();
    }

    // The characters come out in reverse, since it's a stack
    std::reverse(output.begin(), output.end());

    std::cout << "Individual: " << output << std::endl;
}

Evolution::Evolution(int popSize, std::string startingWord, int ruleIterations, TurtleState initialState, int numGenerations, FitnessType chosenFitness, MutationType chosenMutation, CheckpointPattern chosenPattern, std::vector<std::vector<int>> matrix)
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
    m_ChosenMatrix = matrix;


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
        m_Population[i].Evaluate(m_ChosenFitness, m_ChosenMatrix);
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

    GM.RenderGrid(gridVector, FF.CreateCheckpoints(50, 50, m_Pattern, numCheckpoints),300, 100, 300, 300, gridHeight, gridWidth);
    RenderFitness(individual, 500,100);
        
    RenderText(std::to_string(static_cast<int>(std::round(m_AverageFitness))), 500, 200, "Average Fitness: ");
    RenderText(std::to_string(static_cast<int>(std::round(m_CurrentGeneration))), 100, 50, "Generation: ");


    RM.presentScene();



}

void Evolution::RenderFitness(Individual individual, int xPos, int yPos)
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

    SDL_Rect rect = { xPos,yPos, surface->w, surface->h }; 
    SDL_RenderCopy(RM.renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);


}

void Evolution::RenderText(const std::string& numberText, int xPos, int yPos, const std::string& descText)
{
    SDL_Color white = { 255, 255, 255, 255 };

    // Construct the full text string
    std::string fullText = descText + numberText;

    // Render the text to a surface
    SDL_Surface* surface = TTF_RenderText_Solid(RM.font, fullText.c_str(), white);
    if (!surface)
    {
        std::cerr << "Failed to render text surface: " << TTF_GetError() << std::endl;
        return;
    }

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(RM.renderer, surface);
    if (!texture)
    {
        std::cerr << "Failed to create texture from surface: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect rect = { xPos, yPos, surface->w, surface->h };
    SDL_RenderCopy(RM.renderer, texture, nullptr, &rect);

    // Clean up
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
            m_Population[i].MutateWord(m_SymbolSet, m_ExpansionSize, m_MutationChances);
            break;
        default:
            std::cout << "default mutation chosen" << std::endl;
            m_Population[i].MutateRule(m_SymbolSet, m_MutationChances);
            break;
        }
        


    }


}

void SaveWindowAsImage(SDL_Renderer* renderer, int width, int height, const std::string& fileName)
{
    // Create an SDL surface to hold the pixels
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32,
        0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    if (!surface) {
        std::cerr << "Failed to create surface: " << SDL_GetError() << std::endl;
        return;
    }

    // Read the pixels from the renderer and store them in the surface
    if (SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch) != 0) {
        std::cerr << "Failed to read pixels: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }

    // Save the surface to a PNG file
    if (IMG_SavePNG(surface, fileName.c_str()) != 0) {
        std::cerr << "Failed to save image: " << IMG_GetError() << std::endl;
    }

    // Free the surface
    SDL_FreeSurface(surface);
}

float Evolution::Evaluation()
{
    float totalFitness = 0.0f;

    for (auto& individual : m_Population)
    {
        individual.Evaluate(m_ChosenFitness, m_ChosenMatrix);
        totalFitness += individual.GetIndividual().Fitness;
    }
    float averageFitness = totalFitness / m_Population.size();
    m_AverageFitness = averageFitness;

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

// pass in the best individual of each generation
// need to log starting rule and if its word etc
void Evolution::Run(int runIndex)
{
    int counter = 0;
    float averageFitness = 0.0f;

    if (!m_Complete)
    {
        InitialisePopulation();


        std::ofstream logFile("best_individuals_log.csv", std::ios::app);
        logFile << "Run Index,Generation,Best Fitness,Genetic Code\n";  

        for (int i = 0; i < m_NumberOfGenerations; i++)
        {
            SortPopulation();  
            RenderIndividual(m_BestIndividual);  

            Selection(); 
            Mutation();  
            m_AverageFitness = Evaluation();  



            std::string geneticCode = ConvertStackToString(m_BestIndividual.individual);
            std::hash<std::string> hasher;
            size_t hashValue = hasher(geneticCode);
            std::stringstream ss;
            ss << std::hex << hashValue;
            std::string hashHex = ss.str();

            std::string mutateVersion = "default";

            if (m_MutationChoice == WORD)
            {
                mutateVersion = "WORD";
            }
            else if (m_MutationChoice == RULE)
            {
                mutateVersion = "RULE";
            }
            else
            {
                mutateVersion = "default";
            }

            std::vector<std::string> symbolSetStr;
            for (char c : m_SymbolSet)
            {
                symbolSetStr.push_back(std::string(1, c));
            }


            // log CSV
            logFile << runIndex << "," << i + 1 << "," << m_BestIndividual.Fitness << ","
                << m_AverageFitness << "," << mutateVersion << "," << m_ExpansionSize << "," << hashHex << "\n";

            // save json
            std::string timestamp = GetTimestamp();
            std::string jsonFilename = "genomes_run_" + std::to_string(runIndex) + "_" + timestamp + ".json";

            std::ifstream existing(jsonFilename);
            json genomeData;
            if (existing.is_open()) {
                existing >> genomeData;
                existing.close();
            }




            genomeData[hashHex] = {
                {"genetic_code", geneticCode},
                {"rule_map", ConvertRuleMap(m_BestIndividual.rule)},
                {"starting_rule", ConvertRuleMap(m_BestIndividual.StartingRule)},
                {"mutation_choice", mutateVersion},
                {"symbol_set", symbolSetStr},
                {"mutation_chances", m_MutationChances},
                {"expansion_size", m_ExpansionSize}
            };

            std::ofstream genomeOut(jsonFilename);
            genomeOut << genomeData.dump(2); // pretty print
            genomeOut.close();




            // log the average Fitness
            // the best individual (including the fitness) for each generation 

       
            //logFile << runIndex << "," << i + 1 << "," << m_BestIndividual.Fitness << "\n";

            std::cout << "Generation: " << i + 1 << "/" << m_NumberOfGenerations
                << " | Avg Fitness: " << m_AverageFitness << std::endl;

            m_CurrentGeneration = i+1;
            



        }

        std::string fileName = "window_capture_run_FitnessOptimized" + std::to_string(runIndex) + "_END_" + GetTimestamp() + ".png";
        //FF.ExportDistancesToCSV("distances" + std::to_string(runIndex) + "_END_" + GetTimestamp() + ".csv", runIndex);
        //std::cout << "second log" << std::endl;


        SaveWindowAsImage(RM.renderer, 800, 500, fileName);

        logFile.close();  

        m_Complete = true;
        std::cout << "Evolution complete. Best individuals data saved to best_individuals_log.csv" << std::endl;
    }
}


//void Evolution::Run(std::vector<std::vector<Individual>>& bestEachGeneration, int runIndex)
//{
//    float averageFitness = 0.0f;
//
//    if (!m_Complete)
//    {
//        InitialisePopulation();
//
//        // Open a log file for storing best individual data across generations and runs
//        std::ofstream logFile("best_individuals_log.csv", std::ios::app);
//        logFile << "Run Index,Generation,Best Fitness,Genetic Code,Rule,StartingRule\n";  // Add appropriate headers
//
//        for (int i = 0; i < m_NumberOfGenerations; i++)
//        {
//            SortPopulation();  // Sort the population by fitness
//            RenderIndividual(m_BestIndividual);  // Visualize the best individual
//
//            Selection();  // Perform selection
//            Mutation();  // Perform mutation
//            averageFitness = Evaluation();  // Evaluate fitness of the population
//
//            // Convert stack to string for logging
//            //std::string geneticCode = StackToString(m_BestIndividual.individual);
//
//            // Serialize rule and starting rule
//            //std::string rule = SerializeRules(m_BestIndividual.rule);
//            //std::string startingRule = SerializeRules(m_BestIndividual.StartingRule);
//
//            // Log the best individual’s fitness, genetic code, and rules
//            logFile << runIndex << "," << i + 1 << "," << m_BestIndividual.Fitness << ",", "\n";
//
//            std::cout << "Generation: " << i + 1 << "/" << m_NumberOfGenerations
//                << " | Avg Fitness: " << averageFitness << std::endl;
//
//            SDL_Delay(100);  // Delay to visualize
//
//            // Store the best individual for this generation in the respective runIndex
//            //bestEachGeneration[runIndex].push_back(m_BestIndividual);
//        }
//
//        logFile.close();  // Close the log file after all generations are processed
//
//        m_Complete = true;
//        std::cout << "Evolution complete. Best individuals data saved to best_individuals_log.csv" << std::endl;
//    }
//}
//
//// Convert stack<char> to string
std::string Evolution::ConvertStackToString(std::stack<char> stack) {
    std::vector<char> chars;
    while (!stack.empty()) {
        chars.push_back(stack.top());
        stack.pop();
    }
    std::reverse(chars.begin(), chars.end());
    return std::string(chars.begin(), chars.end());
}

json Evolution::ConvertRuleMap(const std::unordered_map<char, std::vector<std::pair<std::string, float>>>& ruleMap) {
    json j;
    for (const auto& entry : ruleMap) {
        char symbol = entry.first;
        std::vector<std::string> rules;
        for (const auto& pair : entry.second) {
            rules.push_back(pair.first);
        }
        j[std::string(1, symbol)] = rules;
    }
    return j;
}
//
//// Serialize a rule map into a string representation
std::string Evolution::SerializeRules(const std::unordered_map<char, std::vector<std::pair<std::string, float>>>& ruleMap)
{
    std::string result = "";
    for (const auto& pair : ruleMap)
    {
        result += pair.first + ": [";
        for (const auto& rule : pair.second)
        {
            result += "(" + rule.first + ", " + std::to_string(rule.second) + "), ";
        }
        result += "] ";
    }
    return result;
}



void convertIndividual(Individual individual)
{

    individual.Fitness;
    individual.individual;
    individual.rule;
    individual.StartingRule;


}




