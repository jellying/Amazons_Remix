#ifndef _MAP_H
#define _MAP_H

#include "Define.h"
#include <cstring>

typedef unsigned long long int Un64;

enum entry_type
{
	exact, lower_bound, upper_bound
};

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

class HashItem
{
public:
	Un64 checksum;
	entry_type entry;
	int depth;
	double val;
};

class MapType
{
public:
	static int HashOK[4];

	static int start, end;

	point whiteChess[4], redChess[4];

	StackType MoveStack[MAXDEPTH];

	static int step;

	static int dir[8][2];

	unsigned int HashKey32;
	Un64 HashKey64;

	static Un64 HashRand64[MAXSIZE][MAXSIZE][4];
	static unsigned int HashRand32[MAXSIZE][MAXSIZE][4];
	static HashItem HashTable[25000000][2];

	int mappoint[MAXSIZE][MAXSIZE];

	void MakeMove(const MoveType &move,int color);

	void UnMakeMove(const MoveType &move, int color);

	void CreatMove(int color,int depth);

	static Un64 rand64();

	static unsigned int rand32();

	static void Hash_init();

private:

	void HashMove(const MoveType &move, int color);

	void UnHashMove(const MoveType &move, int color);

};

#endif