#ifndef _UCT_H
#define _UCT_H
#include "Define.h"
#include "AlphaBeta.h"

class MCThreadPara
{
public:
	bool end;
	Node * MCnode;
	volatile bool isrunning, unlockthread;
};

class HashNode
{
public:
	int depth;
	Un64 checknum;
	Node * board;
};

class UCTree
{
public:
	static int now, sum;//记录已建立节点数
	static Node *nodes[50000000];//内存池
	static HashNode HashTable[2500000][2];
	static int start, end;
	static Node* root;
	static Node* nowChild;
	static Node* bestNode;
	static MoveType * MaxUCBMove;
	static MoveType *nowMove;
	static MapType* mainMap;//指向ABTree的map，两者共用一个map
	static MapType initmap;//用于mainMap的返回

	//多线程所需变量
	static MapType map[MAXTHREAD];
	static thread * MCTH[MAXTHREAD];
	static MCThreadPara MCPara[MAXTHREAD];
	static mutex MClock[MAXTHREAD];
	static condition_variable MCorder[MAXTHREAD];
	static int MCwin[MAXTHREAD];
	static StackType randStack[MAXTHREAD];

	static int srand_time;

	static int MAXTIME;

	static int timesum;

	static MoveType SearchGoodMove(int color);

	//开启线程并挂起等待，全局调用一次
	static void thread_init();

	static void MCwork(int num);

	static void Save(Node *root);

private:

	//蒙特卡洛前的初始化
	static void MC_init();

	//线程蒙特卡洛
	static void ThreadMC(Node *best,int threadNum);

	//开启线程
	static void MC_Start(int num, Node* best);

	//对某个棋子某个方向生成招法
	static void CreatStep(int chess, int dir,point *piece ,int threadNum);

	//生成随机招法
	static MoveType RandMove(int color, int threadNum, int &over);

	static void FindMaxUCB(Node * root);
	
	static void UCT();

	//初始化根和地图
	static void init(int color);

	//从内存池中申请新节点
	static Node* newNode();

	static void AddChild(Node* root,MoveType &m,MapType &map);

	//查找已有局面的节点
	static Node* LookHashTable(unsigned int hash32, Un64 hash64, int depth, int color);

	//将节点写入哈希表
	static void EnterHashTable(Node *now, unsigned int hash32, Un64 hash64, int depth, int color);


};


#endif