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
	static int now, sum;//��¼�ѽ����ڵ���
	static Node *nodes[50000000];//�ڴ��
	static HashNode HashTable[2500000][2];
	static int start, end;
	static Node* root;
	static Node* nowChild;
	static Node* bestNode;
	static MoveType * MaxUCBMove;
	static MoveType *nowMove;
	static MapType* mainMap;//ָ��ABTree��map�����߹���һ��map
	static MapType initmap;//����mainMap�ķ���

	//���߳��������
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

	//�����̲߳�����ȴ���ȫ�ֵ���һ��
	static void thread_init();

	static void MCwork(int num);

	static void Save(Node *root);

private:

	//���ؿ���ǰ�ĳ�ʼ��
	static void MC_init();

	//�߳����ؿ���
	static void ThreadMC(Node *best,int threadNum);

	//�����߳�
	static void MC_Start(int num, Node* best);

	//��ĳ������ĳ�����������з�
	static void CreatStep(int chess, int dir,point *piece ,int threadNum);

	//��������з�
	static MoveType RandMove(int color, int threadNum, int &over);

	static void FindMaxUCB(Node * root);
	
	static void UCT();

	//��ʼ�����͵�ͼ
	static void init(int color);

	//���ڴ���������½ڵ�
	static Node* newNode();

	static void AddChild(Node* root,MoveType &m,MapType &map);

	//�������о���Ľڵ�
	static Node* LookHashTable(unsigned int hash32, Un64 hash64, int depth, int color);

	//���ڵ�д���ϣ��
	static void EnterHashTable(Node *now, unsigned int hash32, Un64 hash64, int depth, int color);


};


#endif