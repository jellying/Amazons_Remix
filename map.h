#ifndef _MAP_H
#define _MAP_H

#include "Define.h"
#include <cstring>
class point
{
public:
	int x, y;
};

class MoveType
{
public:
	int x[3], y[3];
	double val;
	int rating;
	int num;
};

class StackType
{
public:
	MoveType moves[MAXMOVE];
	int len;
};

class MapType
{
public:
	static int start, end;

	point whiteChess[4], redChess[4];

	StackType MoveStack[MAXDEPTH];

	static int step;

	static int dir[8][2];

	int mappoint[MAXSIZE][MAXSIZE];

	void MakeMove(MoveType move,int color);

	void UnMakeMove(MoveType move, int color);

	void CreatMove(int color,int depth);

	
private:
};

#endif