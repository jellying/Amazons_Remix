#ifndef _DEFINE_H
#define _DEFINE_H
#define _CRT_SECURE_NO_DEPRECATE
#define BLANK  0
#define MAX_DEEP 10
#define RED  2
#define WHITE 1
#define STONE 3

#define		MAXMOVE		3000		//所有招法个数
#define		INF			10000000
#define		MAXSIZE		10			//棋盘大小
#define		MAXTHREAD   5			//线程数
#define		MAXDEPTH	15
#define		MAXSIMULATE	40			//最大模拟量
#define		MINSMULATE	5			//最小模拟量
#define		TIMES		6			//蒙特卡洛估值层数
#define		UCB_K		2.0	//UCB_K参数
#define		PRIME		2499997
#define		FAL			86421357
#define		InBoard(a,b)	(a>=0 && a<10 && b>=0 && b<10)
#define		MIN(a,b)	a>b?b:a
#define		MAX(a,b)	a>b?a:b


#include <thread>
#include <mutex>
#include <condition_variable>
#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <unordered_map>
using namespace std;

typedef unsigned long long int Un64;

#endif