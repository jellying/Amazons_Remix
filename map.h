#ifndef _MAP_H
#define _MAP_H

#include "Define.h"
#include <cstring>


enum entry_type
{
	exact, lower_bound, upper_bound
};

class Node;

class MoveType
{
public:
	int x[3], y[3];
	double val;
	int rating;
	int num;
	Node* child;
	int visit;
	int win;
	int color;
};


class Node
{
public:
	int visit;
	int win;
	int depth;
	int color;
	MoveType *move;
	int len;
	Node *parent;
	MoveType *lastMove;
	bool flag;
};


class point
{
public:
	int x, y;
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

class HashEntry
{
public:
	Un64 hash64;
	unsigned hash32;
	int depth;
	int color;

};

class HashRes
{
public:
	double val;
	entry_type entry;
};

class MapType
{
public:

	MapType();

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
	static HashItem HashTable[2500000][2];

	int mappoint[MAXSIZE][MAXSIZE];

	int MobVal[MAXSIZE][MAXSIZE];

	void MakeMove(MoveType &move,int color);

	void UnMakeMove(MoveType &move, int color);

	void CreatMove(int color,int depth);

	static Un64 rand64();

	static unsigned int rand32();

	static void Hash_init();


private:

	void HashMove(MoveType &move, int color);

	void UnHashMove(MoveType &move, int color);

	void MobMove(MoveType &move, int color);

	void UnMobMove(MoveType &move, int color);
};

#endif