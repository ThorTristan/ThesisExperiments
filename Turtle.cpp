#include "turtle.h"


Turtle::Turtle(TurtleState initialState, std::stack<char> lsystemStack, int gridHeight, int gridWidth, int stepLength) :m_CurrentState(initialState), m_StepLength(stepLength), m_Lsystem(lsystemStack), m_GridHeight(gridHeight), m_GridWidth(gridWidth)
{
    m_PreviousState = m_CurrentState;
    m_CurrentDirection = N;

    initializeGrid();

    updateState();
}

std::vector<int> Turtle::returnGridVector() { return m_Grid; }


void Turtle::updateState()
{

    while (!m_Lsystem.empty())
    {
        char c = m_Lsystem.top();
        //std::cout << c;
        m_Lsystem.pop();

        switch (c)
        {

        case 'F':
            moveForward();
            path(PATH);
            break;
        case '+':
            turnLeft();
            break;
        case '-':
            turnRight();
            break;
        case '[':
            pushStack();
            break;
        case ']':
            popStack();
            break;
        default:
            std::cout << "Unknown symbol: " << c << "\n";
            break;
        }


    }


}

bool Turtle::isInBounds(int x, int y)
{
    return x >= 0 && x < m_GridWidth && y >= 0 && y < m_GridHeight;

}



void Turtle::initializeGrid()
{
    m_Grid.resize(m_GridWidth * m_GridHeight, EMPTY);
}



void Turtle::moveForward()
{


    int newX = m_CurrentState.x + directionVectors[static_cast<int>(m_PreviousState.a)].first * m_StepLength;
    int newY = m_CurrentState.y + directionVectors[static_cast<int>(m_PreviousState.a)].second * m_StepLength;

    if (isInBounds(newX, newY))
    {
        m_PreviousState = m_CurrentState;
        m_CurrentState.x = newX;
        m_CurrentState.y = newY;
    }
    else
    {
        //std::cout << "Move out of bounds! Turtle remains in place.\n";
    }
}
void Turtle::turnLeft()
{
    m_CurrentState.a -= 1;
    if (m_CurrentState.a < 0) { m_CurrentState.a = 3; }
}
void Turtle::turnRight()
{
    m_CurrentState.a += 1;
    if (m_CurrentState.a > 3) { m_CurrentState.a = 0; }
}
void Turtle::pushStack()
{
    m_LIFO.push(m_CurrentState);
}
void Turtle::popStack()
{
    if (!m_LIFO.empty())
    {
        m_CurrentState = m_LIFO.top();
        m_LIFO.pop();
    }
    else
    {
        //std::cout << "Stack is empty. Cannot pop state.\n";
    }
}

void Turtle::path(int cellColour)
{
    int x1 = m_PreviousState.x;
    int y1 = m_PreviousState.y;
    int x2 = m_CurrentState.x;
    int y2 = m_CurrentState.y;



    if (x1 == x2)
    {
        int startY = std::min(y1, y2);
        int endY = std::max(y1, y2);
        for (int y = startY; y <= endY; ++y)
        {
            m_Grid[y * m_GridHeight + x1] = cellColour;
        }
    }
    else if (y1 == y2)
    {
        int startX = std::min(x1, x2);
        int endX = std::max(x1, x2);
        for (int x = startX; x <= endX; ++x)
        {
            m_Grid[y1 * m_GridWidth + x] = cellColour;
        }
    }

}