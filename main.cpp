#pragma once
#include "renderManager.h"
#include "inputManager.h"
#include "lsystemCreator.h"
#include "turtle.h"
#include "gridManager.h"
#include "fitnessFunction.h"
#include "individual.h"
#include "helperFunctions.h"
#include "evolutionRM.h"

bool quit = false;
RenderManager RM;
FitnessFunction FF;
GridManager GM;

int main(int argc, char* argv[])
{
	
	
	InputManager InputManager;
	LsystemGenerator LG;
	
	

	std::unordered_map<char, std::vector<std::pair<std::string, float>>> testRules =
	{
	{'F', {{"F+F[+FF][-FF]", 1.0}}},
	{'-', {{"-", 1.0}}},
	{'+', {{"+", 1.0}}},
	{'[', {{"[", 1.0}}},
	{']', {{"]", 1.0}}}
	};


	int iterations = 3;
	int popSize = 100;
	int generations = 50;
	MutationType mutationChoice = RULE;

	TurtleState initialState{25,49,N};
	
	Evolution evolution = Evolution(popSize, "F-F+F", iterations, initialState, generations, AREA,WORD);


	Single single = Single("F-F+F", iterations, initialState);

	//single.GetIndividual().individual;
	//single.PrintLS();
	//single.MutateWord();
	//single.PrintLS();
		
	
		 
	while (!quit)
	{



		evolution.Run();
		//RM.prepareScene();
		//GM.RenderGrid(turtleTest.returnGridVector(), 150, 150, 250, 250, 70, 70);

		//RM.presentScene();
		
		
		InputManager.Listen();


	}

	return 0;

}