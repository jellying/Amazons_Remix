#include "AlphaBeta.h"
#define HistMOVE map[threadNum].MoveStack[depth].moves[i]

int comp(const void *b, const void *a)
{
	return (((MoveType *)a)->rating) - (((MoveType *)b)->rating);
}

MapType ABTree::mainMap;
int ABTree::FillBlank = 0;
MapType ABTree::map[MAXTHREAD];
double ABTree::GlobalAlpha;
thread* ABTree::ABthread[MAXTHREAD];
threadPara ABTree::Para[MAXTHREAD];
condition_variable ABTree::order[MAXTHREAD];
int ABTree::NTmove;
mutex ABTree::movelock;
mutex ABTree::conlock[MAXTHREAD];
MoveType ABTree::bestMove[MAXTHREAD];
int ABTree::histor[10][10][10][10][10][10];

//线程循环挂起，等待执行命令
void ABTree::thread_work(int num)
{
	unique_lock < mutex > orderlock(conlock[num]);
	while (1)
	{
		order[num].wait(orderlock);
		if (Para[num].end == true)
			break;
		Para[num].unlockthread = true;
		ThreadAB(Para[num].depth, Para[num].num, Para[num].color);
		Para[num].isrunning = false;
	}
}

//初始化开启线程，只用一次
void ABTree::init()
{
	for (int i = 0; i < MAXTHREAD; i++)
	{
		ABthread[i]=new thread(ABTree::thread_work,i);
	}
}

void ABTree::exit()
{
	for (int i = 0; i < MAXTHREAD; i++)
	{
		Para[i].end = true;
		order[i].notify_one();
	}
}

//多线程第一步SearchGoodMove
void ABTree::ThreadAB(int depth, int threadNum, int color)
{
	int i;
	double val;
	double alpha = -INF;
	movelock.lock();
	i = NTmove++;
	movelock.unlock();
	while (i < mainMap.MoveStack[0].len)
	{
		map[threadNum].MakeMove(mainMap.MoveStack[0].moves[i], color);
		val = -AlphaBeta(depth - 1, -INF, -alpha, 3 - color, threadNum);
		mainMap.MoveStack[0].moves[i].val = val;
		map[threadNum].UnMakeMove(mainMap.MoveStack[0].moves[i], color);
		if (val>alpha)
		{
			alpha = val;
			bestMove[threadNum] = mainMap.MoveStack[0].moves[i];
		}
		movelock.lock();
		if (alpha > GlobalAlpha)
		{
			GlobalAlpha = alpha;
		}
		else
		{
			alpha = GlobalAlpha;
		}
		i = NTmove++;
		movelock.unlock();
	}
}

double ABTree::AlphaBeta(int depth, double alpha, double beta, int color, int threadNum)
{
	double val;
	int best = 0;
	MoveType bestMove;
	if (depth == 0)
	{

		val = Eval::Evaluate(color, threadNum,map[threadNum]);
		return val;
	}
	map[threadNum].CreatMove(color,depth);
	if (map[threadNum].MoveStack[depth].len == 0)
	{
		val = Eval::Evaluate(color, threadNum,map[threadNum]);
		return val;
	}
	//传入历史得分
	for (int i = 0; i < map[threadNum].MoveStack[depth].len; i++)
	{
		map[threadNum].MoveStack[depth].moves[i].rating = histor[HistMOVE.x[0]][HistMOVE.y[0]][HistMOVE.x[1]][HistMOVE.y[1]][HistMOVE.x[2]][HistMOVE.y[2]];
	}
	qsort(map[threadNum].MoveStack[depth].moves, map[threadNum].MoveStack[depth].len, sizeof(MoveType), comp);
	for (int i = 0; i < map[threadNum].MoveStack[depth].len; i++)
	{
		map[threadNum].MakeMove(map[threadNum].MoveStack[depth].moves[i], color);
		val = -AlphaBeta(depth - 1, -beta, -alpha, 3 - color, threadNum);
		map[threadNum].UnMakeMove(map[threadNum].MoveStack[depth].moves[i], color);
		if (val >= beta)
		{
			bestMove = map[threadNum].MoveStack[depth].moves[i];
			best = 1;
			alpha = val;
			histor[bestMove.x[0]][bestMove.y[0]][bestMove.x[1]][bestMove.y[1]][bestMove.x[2]][bestMove.y[2]] += (1 << depth);
			return alpha;
		}
		if (val > alpha)
		{
			best = 1;
			alpha = val;
			bestMove = map[threadNum].MoveStack[depth].moves[i];
		}
	}
	if (best)
	{
		histor[bestMove.x[0]][bestMove.y[0]][bestMove.x[1]][bestMove.y[1]][bestMove.x[2]][bestMove.y[2]] += (1 << depth);
	}
	return alpha;
}

void ABTree::Thread_Init()
{
	for (int i = 0; i < MAXTHREAD; i++)
	{
		map[i] = mainMap;
	}
}

void ABTree::doThread(int threadNum, int depth, int color)
{
	Para[threadNum].color = color;
	Para[threadNum].depth = depth;
	Para[threadNum].num = threadNum;
	Para[threadNum].unlockthread = false;
	Para[threadNum].end = false;
	Para[threadNum].isrunning = true;
	while (Para[threadNum].unlockthread == false)
	{
		order[threadNum].notify_one();
	}
}

MoveType ABTree::SearchGoodMove(int depth, int color)
{
	int maxval = -INF;
	MoveType best;
	mainMap.CreatMove(color, 0);

	FILE *fp;
	fp = fopen("out.txt", "a");

	NTmove = 0;
	GlobalAlpha = -INF;
	Thread_Init();
	for (int i = 0; i < MAXTHREAD; i++)
	{
		doThread(i, depth, color);
	}
	for (int i = 0; i < MAXTHREAD; i++)
	{
		while (Para[i].isrunning)
		{
			
		}
	}
	for (int i = 0; i < MAXTHREAD; i++)
	{
		if (bestMove[i].val>maxval)
		{
			maxval = bestMove[i].val;
			best = bestMove[i];
		}
	}

	/*map[0] = mainMap;
	double val;
	double alpha = -INF;
	for (int i = 0; i < mainMap.MoveStack[0].len; i++)
	{
		map[0].MakeMove(mainMap.MoveStack[0].moves[i], color);

		val = -AlphaBeta(depth - 1, -INF, -alpha, 3 - color, 0);
		mainMap.MoveStack[0].moves[i].val = val;
		map[0].UnMakeMove(mainMap.MoveStack[0].moves[i], color);
		if (val > alpha)
		{
			alpha = val;
			best = mainMap.MoveStack[0].moves[i];
		}
	}*/
	fclose(fp);
	return best;
}
