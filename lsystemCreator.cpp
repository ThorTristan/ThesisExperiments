#include "lsystemCreator.h"

void LsystemGenerator::Generate(std::string initialWord, int iterations, std::unordered_map<char, std::vector<std::pair<std::string, float>>> rules)
{
    InitialWord(initialWord);
    m_Iterations = iterations;
    Iterate(rules);
}

std::stack<char> LsystemGenerator::ReturnFinalStack() const
{
    if (m_Stack.empty())
    {
        std::cout << "Error, stack empty" << std::endl;
        return m_Stack;
    }
    else
    {
        return m_Stack;
    }
    
}

void LsystemGenerator::PrintStack(std::stack<char> stack) {
    while (!stack.empty()) {
        std::cout << stack.top();
        stack.pop();
    }
    std::cout << std::endl;
}

void LsystemGenerator::InitialWord(std::string word)
{
    for (const auto& symbol : word)
    {
        m_Stack.push(symbol);
    }
}


void LsystemGenerator::Iterate(std::unordered_map<char, std::vector<std::pair<std::string, float>>> rules)
{
    for (int i = 0; i < m_Iterations; ++i)
    {

        while (!m_Stack.empty())
        {
            ApplyRule(rules);
        }

        std::swap(m_Stack, m_NewStack);

        while (!m_NewStack.empty())
        {
            m_NewStack.pop();
        }
    }

}


void LsystemGenerator::ApplyRule(std::unordered_map<char, std::vector<std::pair<std::string, float>>> rules)
{

    if (m_Stack.empty() || m_Stack.size() >= 50000000)
    {
        return;
    }
    char symbol = m_Stack.top();
    m_Stack.pop();


    auto it = rules.find(symbol); 
    if (it != rules.end())
    {
        const auto& rulesSet = it->second;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float randomValue = dist(gen);

        float cumulativeProb = 0.0f;
        for (const auto& [rule, probability] : rulesSet)
        {
            cumulativeProb += probability;
            if (randomValue <= cumulativeProb)
            {
                for (const char& c : rule)
                {
                    m_NewStack.push(c);
                }
                return;
            }
        }
    }
    else
    {
        m_NewStack.push(symbol);
    }
}

