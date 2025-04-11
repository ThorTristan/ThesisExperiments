#include "GridManager.h"
#include "renderManager.h"
#include "structs.h"
#include "initialiseSDL.h"
//#include "FitnessFunction.h"
extern RenderManager RM;
//extern FitnessFunction FF;




void GridManager::RenderGrid(std::vector<int> grid, std::vector<Checkpoint> checkpointsLocation, int GridPosX, int GridPosY, int GridWidth, int GridHeight, int rows, int cols)
{
    int m_Cols = cols;
    int m_Rows = rows;



    int cellWidth = std::max(1, GridWidth / m_Cols);
    int cellHeight = std::max(1, GridHeight / m_Rows);



    for (int i = 0; i < m_Rows; ++i)
    {
        for (int j = 0; j < m_Cols; ++j)
        {
            SDL_Rect cellRect;
            cellRect.x = j * cellWidth + (GridPosX - GridWidth / 2);
            cellRect.y = i * cellHeight + (GridPosY);
            cellRect.w = cellWidth;
            cellRect.h = cellHeight;


            int state = grid[i * m_Cols + j];
            if (grid.size() != m_Cols * m_Rows) {
                std::cout << "Error: Grid size mismatch! Expected " << (m_Cols * m_Rows)
                    << " but got " << grid.size() << "\n";
            }

            switch (state)
            {
            case EMPTY:
                SDL_SetRenderDrawColor(RM.renderer, 0, 0, 0, 255); // Black for empty
                break;
            case PATH:
                SDL_SetRenderDrawColor(RM.renderer, 255, 255, 255, 200); // White for path
                break;
            default:
                SDL_SetRenderDrawColor(RM.renderer, 255, 255, 255, 255); // Black for unknown
                break;
            }


            SDL_RenderFillRect(RM.renderer, &cellRect);

            SDL_SetRenderDrawColor(RM.renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(RM.renderer, &cellRect);
        }
    }

    RenderCheckpoints(checkpointsLocation, GridPosX, GridPosY, GridWidth, cellWidth, cellHeight);

}


void GridManager::RenderCheckpoints(std::vector<Checkpoint> checkpointsLocation, int GridPosX, int GridPosY, int GridWidth, int cellWidth, int cellHeight)
{


    SDL_SetRenderDrawColor(RM.renderer, 0, 0, 255, 255); // Blue for checkpoints
    for (const auto& checkpoint : checkpointsLocation)
    {
        SDL_Rect checkpointRect;
        checkpointRect.x = checkpoint.x * cellWidth + (GridPosX - GridWidth / 2);
        checkpointRect.y = checkpoint.y * cellHeight + GridPosY;
        checkpointRect.w = cellWidth;
        checkpointRect.h = cellHeight;

        SDL_RenderFillRect(RM.renderer, &checkpointRect);
    }


}
