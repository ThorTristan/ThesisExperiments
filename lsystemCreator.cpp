#include "lsystemCreator.h"
#include "lsystemCreator.h"
#include <random>
#include <iostream>

void LsystemGenerator::Generate(std::string& initialWord, int iterations,
     std::unordered_map<char, std::vector<std::pair<std::string, float>>>& rules)
{
    InitialWord(initialWord);
    m_Iterations = iterations;
    Iterate(rules);
}

std::stack<char> LsystemGenerator::ReturnFinalStack() const
{
    if (m_Stack.empty())
        std::cerr << "Error: stack is empty." << std::endl;

    return m_Stack;
}

void LsystemGenerator::PrintStack(std::stack<char> stack) {
    while (!stack.empty()) {
        std::cout << stack.top();
        stack.pop();
    }
    std::cout << '\n';
}

void LsystemGenerator::InitialWord(std::string& word)
{
    for (char c : word) {
        m_Stack.push(c);
    }
}

void LsystemGenerator::Iterate(std::unordered_map<char, std::vector<std::pair<std::string, float>>>& rules)
{
    for (int i = 0; i < m_Iterations; ++i)
    {
        while (!m_Stack.empty()) {
            ApplyRule(rules);
        }

        std::swap(m_Stack, m_NewStack);
        // Clear the new stack by swapping with an empty one
        std::stack<char>().swap(m_NewStack);
    }
}

void LsystemGenerator::ApplyRule(std::unordered_map<char, std::vector<std::pair<std::string, float>>>& rules)
{
    if (m_Stack.empty() || m_Stack.size() >= 50000000)
        return;

    char symbol = m_Stack.top();
    m_Stack.pop();

    auto it = rules.find(symbol);
    if (it != rules.end())
    {
        static thread_local std::mt19937 gen(std::random_device{}());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float randomValue = dist(gen);

        float cumulativeProb = 0.0f;
        for (const auto& [rule, probability] : it->second)
        {
            cumulativeProb += probability;
            if (randomValue <= cumulativeProb)
            {
                for (char c : rule)
                    m_NewStack.push(c);
                return;
            }
        }
    }

    // No rule matched or symbol not found, push original
    m_NewStack.push(symbol);
}

