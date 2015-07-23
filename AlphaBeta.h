#ifndef _ALPHABETA_H
#define _ALPHABETA_H
#include "Evaluate.h"
#include "Define.h"
#include "map.h"

using namespace std;

int comp(const void *b, const void *a);

int comp1(const void *b, const void *a);

//AlphaBeta�߳���������
class threadPara
{
public:
	int depth, num, color;
	volatile bool isrunning, end, unlockthread;
};

class ABTree
{
public:
	static MapType mainMap;
	static int FillBlank;

	static MoveType SearchGoodMove(int depth, int color);

	static MoveType DeepingIter(int depth, int color);

	static void init();

	static void exit();

private:
	static MapType map[MAXTHREAD];
	
	static thread *ABthread[MAXTHREAD];
	static mutex conlock[MAXTHREAD];
	static threadPara Para[MAXTHREAD];
	static condition_variable order[MAXTHREAD];
	
	static int NTmove;
	static double GlobalAlpha;
	
	static mutex movelock, hashlock;
	
	static MoveType bestMove[MAXTHREAD];
	
	static int histor[10][10][10][10][10][10];

	static double AlphaBeta(int depth, double alpha, double beta, int color,int threadNum);

	//�̹߳���������ȴ������������
	static void thread_work(int num);

	//��һ���߳�searchGoodMove
 	static void ThreadAB(int depth, int threadNum, int color);

	//Ϊ���߳�������ʼ������
	static void Thread_Init();

	//�޸��������ݲ������߳�
	static void doThread(int threadNum, int depth, int color);

	static double LookHashTable(double alpha, double beta, int depth, int color, int threadNum);

	static void EnterHashTable(double val, entry_type entry, int depth, int color, int threadNum);

};
#endif