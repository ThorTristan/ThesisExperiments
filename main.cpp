#pragma once
#include "mainHeaderFiles.h"
#include "json.hpp"



bool quit = false;
RenderManager RM;
FitnessFunction FF;
GridManager GM;

Testing testing;

int main(int argc, char* argv[])
{






	
	InputManager InputManager;
	LsystemGenerator LG;
	//Experiment1 experiment(30); //..DONE
	Experiment3 experiment3(30);
	
	//ExperimentConstraints experiment2(30, constraintMatrices); //..DONE

	TurtleState initialState{25,49,N};
	
	//Evolution evolution = Evolution(popSize, "F-F+F", iterations, initialState, generations, CHECKPOINT_DISTANCE,RULE,CIRCULAR);

	//evolution.SetMutationParams({ 'F','+','-','[',']' }, { 60,20,20 }, 1);
	//experiment.Run();
	//evolution.Run();

	//experiment2.RunTest({ 'F','+','-','[',']' },  5, { 60,20,20 },WORD );
	//std::cout << "after";
	//experiment2.Run();
		 

	//std::vector<int> grid = testing.CreateTestGrid(50, 50);
	//testing.PrintGrid(grid,50,50);

	//std::string lsystem = R"([]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FFFF-FF-F[F]-++FF][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FFFF-FF-++FF][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][[]FFFF-F[[]]FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FFFF-FF-++FF][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FFFF-FF-++FF][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][-[]FFF[+-]+F-F[[F-]F+]--F-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FFFF-FF-++FF][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][]-[][FFFF-FF-++FF][[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FFFF-FF-++FF][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][-++[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FF]]+[]FF-FF-++FFF][--][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-F[-]F-F-++FF][FFFF-FF-++FF][-FFFF-FF-++FF][F+[+[FFFF-FF-++FF][F]-+F+FFF-FF-++FF][FFFF-FF-++FF][][+[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FF-+F[FFF-FF-++FF][-FFFF-F++[][F-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-F[F]-FF-++FF][FFFF-FF-++FF][-FFFF-FF-++FF][FF+-]F[FF-FF-++FF][FFFF-FF+--+[-++FF][FFFF-FF-++FF][[]FFFF-FF-++FFF]+[+][FFFF-FF-++FF][++-FF+-F-FFF-FF-++FF][FFFF-FF-++FF][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][[]FFFF]-]F][F]F[-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FFF++FF-F-FF-++FF][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][-[]FFFF-FF-++FF][F[]FF-FFF-FF-++FF][++-FFFF-FF-++FF][FFFF-FF-++FF][-FFFF-FF-++F[][]+F][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF[FF]F][[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FFFF-FF[-[]]-++FF][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][-++[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][F][-F+FFF-FF-++FF][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFF++[]]F-FF-++FF][[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FFFF]-FFF-FF-++FF][-FFFF-FF-++FF][FFFF-FF-++F-[]+]F-FF+]][FFFF-FF-++FF][FFFF-FF-++FF][][-[]FFF-+-FFF-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FFFF-FF-++FF][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FFF-+---++FF][[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FFFF-FF-++FF][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-F-+F-++FF][[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FFFF-F+++-FF-++FF][-FF+][[]FF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FFFF-FF-++FF][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][-[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FFFF-FF-++FF][-FFF-FFFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][[]FFFF-FF-++FF][FFFF-FF-++F-]++[F][++-FFFF-FF-++FF][FFFF-FF-++F+]-[+F][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][-++[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFF-FF+FF-FF-++FF][FFFF-FF-++FF][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][[]FFFF-FF-++FF][FFFF-FF-++FF][++-FFFF-FF-++FF][FF+-]]-FF-FF-++FF][-FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][FFFF-FF-++FF][][)"; 
	//std::stack<char> charStack = testing.ConvertToStack(lsystem);

	//std::vector<int> grid = testing.CreateTestGrid(50, 50, charStack);


	RM.prepareScene();
	//testing.TestFitnessFunction();
	
	//testing.PrintGrid(grid, 50, 50);

	//FF.CheckpointDistanceFitness(FF.CreateCheckpoints(50,50,CIRCULAR,4),constraintMatrix,initialState,charStack,50,50);


	experiment3.Run();

	//testing.TestCSP(charStack);
	RM.presentScene();

	while (!quit)
	{
		

		
		//evolution.Run();
		//RM.prepareScene();
		//GM.RenderGrid(turtleTest.returnGridVector(), 150, 150, 250, 250, 70, 70);

		//RM.presentScene();

		SDL_ShowCursor(1);
		
		
		InputManager.Listen();


	}

	return 0;

}