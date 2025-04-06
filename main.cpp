#pragma once
#include "mainHeaderFiles.h"


bool quit = false;
RenderManager RM;
FitnessFunction FF;
GridManager GM;

int main(int argc, char* argv[])
{
	
	
	InputManager InputManager;
	LsystemGenerator LG;
	Experiment1 experiment(30);
	Experiment2 experiment2(30);
	

	TurtleState initialState{25,49,N};
	
	//Evolution evolution = Evolution(popSize, "F-F+F", iterations, initialState, generations, CHECKPOINT_DISTANCE,RULE,CIRCULAR);

	//evolution.SetMutationParams({ 'F','+','-','[',']' }, { 60,20,20 }, 1);
	experiment.Run();
	//evolution.Run();

	//experiment2.RunTest({ 'F','+','-','[',']' },  5, { 60,20,20 },WORD );
	//experiment2.Run();
		 
	while (!quit)
	{

		
		
		//evolution.Run();
		//RM.prepareScene();
		//GM.RenderGrid(turtleTest.returnGridVector(), 150, 150, 250, 250, 70, 70);

		//RM.presentScene();
		
		
		InputManager.Listen();


	}

	return 0;

}