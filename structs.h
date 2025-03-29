#pragma once


struct Point
{

	int x;
	int y;


};

struct TurtleState
{

	int x;
	int y;
	int a;


};

struct Checkpoint
{
	int x;
	int y;
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