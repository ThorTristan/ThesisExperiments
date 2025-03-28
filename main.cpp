#include "renderManager.h"
#include "inputManager.h"
#include "lsystemCreator.h"

bool quit = false;

int main(int argc, char* argv[])
{
	
	RenderManager RM;
	InputManager IM;
	LsystemGenerator LG;

	std::unordered_map<char, std::vector<std::pair<std::string, float>>> testRules =
	{
		{'F', {{"]FFF+-][", 1.0}}},
		{'-', {{"-", 1.0}}},
		{'+', {{"+", 1.0}}},
		{'[', {{"[", 1.0}}},
		{']', {{"]", 1.0}}}
	};

	int iterations = 1;
	LG.Generate("FFF", iterations, testRules);
	LG.PrintStack(LG.ReturnFinalStack());

		 
	while (!quit)
	{

		
		
		IM.Listen();


	}

	return 0;

}