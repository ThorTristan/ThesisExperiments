#pragma once


struct TurtleState
{

	int x;
	int y;
	int a;


};

std::pair<int, int> directionVectors[] =
{
    {0, -1},  // UP
    {1, 0},   // RIGHT
    {0, 1},   // DOWN
    {-1, 0},  // LEFT

};


enum Direction
{

	N,
	S,
	W,
	E,
	NW,
	NE,
	SW,
	SE,



};

enum GridState
{
	EMPTY,
	PATH,

};