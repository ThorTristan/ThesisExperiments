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

	Checkpoint() = default;

	Checkpoint(int x_, int y_) : x(x_), y(y_) {}
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

enum MutationType
{

	WORD,
	RULE
};

enum CheckpointPattern 
{
	LINEAR,
	CLUSTERED,
	SQUARE,
	CIRCULAR,
	RANDOM,
	NONE
};
