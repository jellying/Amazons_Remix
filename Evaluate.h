#ifndef _EVALUATE_H
#define _EVALUATE_H
#include "map.h"
class Eval
{
public:
	
	static int QueenMove[MAXTHREAD][10][10][2], KingMove[MAXTHREAD][10][10][2];

	static double Evaluate(int color, int threadNum,const MapType& map);
	
	static void Queen(const MapType &map, int color, int threadNum);

private:


	static void King(const MapType& map, int color, int threadNum);

	static double CalMob(const MapType& map, int threadNum,int color,double timew);


};
#endif