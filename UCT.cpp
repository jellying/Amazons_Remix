#include "UCT.h"

Node* UCTree::nodes[50000000];
HashNode UCTree::HashTable[2500000][2];
int UCTree::start, UCTree::end;
Node* UCTree::root;
Node* UCTree::bestNode;
Node* UCTree::nowChild;
MoveType* UCTree::MaxUCBMove;
MapType* UCTree::mainMap;
MapType UCTree::initmap;
MapType UCTree::map[MAXTHREAD];
int UCTree::now = 0 , UCTree::sum = 0;
int UCTree::srand_time;
int UCTree::MAXTIME;

thread* UCTree::MCTH[MAXTHREAD];
MCThreadPara UCTree::MCPara[MAXTHREAD];
mutex UCTree::MClock[MAXTHREAD];
condition_variable UCTree::MCorder[MAXTHREAD];
int UCTree::MCwin[MAXTHREAD];
StackType UCTree::randStack[MAXTHREAD];

int UCTree::timesum = 0;

//���ڴ��������ڵ�
Node* UCTree::newNode()
{
	Node* N;
	while (now < sum && nodes[now]->flag == true )
		now++;
	if (now < sum)
	{
		N=nodes[now];
		if (N->len>0)
		{
			delete []N->move;
		}
		
		now++;
	}
	else
	{
		nodes[sum++] = new Node();
		N = nodes[now++];
	}
	N->win = 0;
	N->visit = 0;
	N->lastMove = NULL;
	N->len = 0;
	N->flag = false;
	return N;
}

void UCTree::thread_init()
{
	for (int i = 0; i < MAXTHREAD; i++)
	{
		MCTH[i] = new thread(MCwork, i);
	}
}

void UCTree::init(int color)
{
	//memset(HashTable, 0, sizeof(HashTable));
	mainMap = &ABTree::mainMap;
	now = 0;
	//��һ���Լ�������֮��ÿ��������,δ�������ؽ�
	if (root == NULL || root->flag == false)
	{
		root = newNode();
		root->color = 3 - color;
		root->parent = NULL;
		root->len = 0;
		root->depth = 0;
		root->visit = 0;
		root->win = 0;
	}
	else
	{
		root->parent = NULL;
		root->len = 0;
		root->depth = 0;
		FILE *fp;
		fp = fopen("out.txt", "a");
		fprintf(fp, "lastroot: vis %d\n", root->visit);
		fclose(fp);
	}
}

void UCTree::MCwork(int num)
{
	srand((unsigned)srand_time++);
	unique_lock <mutex> MCorderLock(MClock[num]);
	while (1)
	{
		MCorder[num].wait(MCorderLock);
		if (MCPara[num].end)
			break;
		MCPara[num].unlockthread = true;
		ThreadMC(MCPara[num].MCnode, num);
		MCPara[num].isrunning = false;
	}
}

//��鵱ǰ�����Ƿ��Ѿ��������ڵ�
Node* UCTree::LookHashTable(unsigned int hash32, Un64 hash64, int depth, int color)
{
	int x = hash32%PRIME;
	HashNode *HT;
	HT = &UCTree::HashTable[x][color];
	MapType::HashOK[0]++;
	if (depth==HT->depth && HT->checknum == hash64 && HT->board != NULL)
	{
		MapType::HashOK[2]++;
		HT->board->flag = true;
		return HT->board;
	}
	return NULL;
}

void UCTree::EnterHashTable(Node *now, unsigned int hash32, Un64 hash64, int depth, int color)
{
	MapType::HashOK[1]++;
	int x = hash32%PRIME;
	HashNode *HT;
	HT = &UCTree::HashTable[x][color];
	HT->board = now;
	HT->depth = depth;
	HT->checknum = hash64;
}

void UCTree::AddChild(Node* root,MoveType &m,MapType &map)
{
	int color = m.color;
	Node* nownode;
	map.MakeMove(m, color);
	nownode = NULL;
//	nownode = LookHashTable(map.HashKey32, map.HashKey64, root->depth + 1, color % 2);
	//δ������д��
	if (nownode == NULL)
	{
		nownode = newNode();
		nownode->color = color;
		nownode->depth = root->depth + 1;
		nownode->len = 0;
		nownode->visit = 0;
		nownode->win = 0;
		EnterHashTable(nownode, map.HashKey32, map.HashKey64, root->depth + 1, color % 2);
	}
	map.UnMakeMove(m, color);
	nownode->parent = root;
	nownode->lastMove = &m;
	m.child = nownode;
}

void UCTree::MC_init()
{
	memset(MCwin, 0, sizeof(MCwin));
	for (int i = 0; i < MAXTHREAD; i++)
	{
		memcpy(map[i].mappoint, mainMap->mappoint, sizeof(mainMap->mappoint));
		memcpy(map[i].whiteChess, mainMap->whiteChess, sizeof(mainMap->whiteChess));
		memcpy(map[i].redChess, mainMap->redChess, sizeof(mainMap->redChess));
		memcpy(map[i].MobVal, mainMap->MobVal, sizeof(mainMap->MobVal));
	}
}

void UCTree::CreatStep(int chess, int dir,point *piece, int threadNum)
{
	int k = 0;
	point nextpoint, stone, next;
	int color, stepscore;
	color = map[threadNum].mappoint[piece[chess].x][piece[chess].y];
	map[threadNum].mappoint[piece[chess].x][piece[chess].y] = BLANK;
	nextpoint.x = piece[chess].x + MapType::dir[dir][0];
	nextpoint.y = piece[chess].y + MapType::dir[dir][1];
	while (InBoard(nextpoint.x,nextpoint.y) && map[threadNum].mappoint[nextpoint.x][nextpoint.y] == BLANK)
	{
		for (int j = 0; j < 8; j++)
		{
			stone.x = nextpoint.x + MapType::dir[j][0];
			stone.y = nextpoint.y + MapType::dir[j][1];
			while (InBoard(stone.x,stone.y) && map[threadNum].mappoint[stone.x][stone.y] == BLANK)
			{
				map[threadNum].MoveStack[0].moves[k].x[0] = piece[chess].x;
				map[threadNum].MoveStack[0].moves[k].y[0] = piece[chess].y;
				map[threadNum].MoveStack[0].moves[k].x[1] = nextpoint.x;
				map[threadNum].MoveStack[0].moves[k].y[1] = nextpoint.y;
				map[threadNum].MoveStack[0].moves[k].x[2] = stone.x;
				map[threadNum].MoveStack[0].moves[k++].y[2] = stone.y;
				stone.x += MapType::dir[j][0];
				stone.y += MapType::dir[j][1];
			}
		}
		nextpoint.x += MapType::dir[dir][0];
		nextpoint.y += MapType::dir[dir][1];
	}
	map[threadNum].MoveStack[0].len = k;
	map[threadNum].mappoint[piece[chess].x][piece[chess].y] = color;
}


MoveType UCTree::RandMove(int color, int threadNum, int &over)
{
	int chess, direct, num, temp, cl, dl;
	cl = 4;
	int amazon[8], dire[8];
	MoveType next;
	for (int i = 0; i < 8; i++)
	{
		amazon[i] = i;
		dire[i] = i;
	}
	while (cl)															//���4�������ҿ����з�
	{
		dl = 8;
		chess = rand() % cl;											//��������һ������
		while (dl)
		{
			direct = rand() % dl;								//���������һ������
			if (color == WHITE)
				CreatStep(amazon[chess], dire[direct],map[threadNum].whiteChess,threadNum);			//�൱�ڶ����������������һ��CreatMove
			else
				CreatStep(amazon[chess], dire[direct], map[threadNum].redChess, threadNum);
			if (map[threadNum].MoveStack[0].len> 0)									//������з����������һ��
			{
				num = rand() % map[threadNum].MoveStack[0].len;
				next = map[threadNum].MoveStack[0].moves[num];
				return next;
			}
			else														//�������������ߵ�����ĩβ
			{
				temp = dire[dl - 1];
				dire[dl - 1] = dire[direct];
				dire[direct] = temp;
			}
			dl--;														//�´�����������浽�÷���
		}
		temp = amazon[cl - 1];											//�����û���з����ɣ����ߵ���β
		amazon[cl - 1] = amazon[chess];
		amazon[chess] = temp;
		cl--;
	}
	over = 1;														//���û���ҵ��з���return,��˵���������GameOver
	next.x[0] = 0;
	next.y[0] = 1;
	next.x[1] = 2;
	next.y[1] = 3;
	next.x[2] = 4;
	next.y[2] = 5;
	return next;														//GameOverҲҪ���ⷵ��һ���з��������޷���ֵ
}


void UCTree::ThreadMC(Node *best,int threadNum)
{
	int t = TIMES;
	int nextcolor = best->color;
	if (nextcolor == WHITE)
		t--;
	MoveType next;
	int over = 0;
	while (t--)
	{
		nextcolor = 3 - nextcolor;
		next = RandMove(nextcolor, threadNum, over);
		if (over)
		{
			if (nextcolor == best->color)
			{
				MCwin[threadNum] = 0;
			}
			else
			{
				MCwin[threadNum] = 1;
			}
			return;
		}
		map[threadNum].MakeMove(next, nextcolor);
	}
	double val = Eval::Evaluate(nextcolor, threadNum, map[threadNum]);
	
	if (nextcolor != best->color)
	{
		val = -val;
	}
	if (best->color == WHITE)
	{
		if (val > 0)
		{
			MCwin[threadNum] = 1;
		}
		else
		{
			MCwin[threadNum] = 0;
		}
	}
	else
	{
		if (val >= 0)
		{
			MCwin[threadNum] = 1;
		}
		else
		{
			MCwin[threadNum] = 0;
		}
	}
}

void UCTree::MC_Start(int num, Node* best)
{
	MCPara[num].MCnode = best;
	MCPara[num].end = false;
	MCPara[num].isrunning = true;
	MCPara[num].unlockthread = false;
	while (MCPara[num].unlockthread == false)
	{
		MCorder[num].notify_one();
	}
}

void UCTree::FindMaxUCB(Node*root)
{
	double UCB, maxUCB = -INF;
	//����չ����
	int n;
//	n = log(1.0*root->visit / 40  + 1) / log(1.1);
	n = root->len;
	for (int i = 0; i <= n && i<root->len; i++)
	{
		if (root->move[i].child->visit > MINSMULATE)
		{
			UCB = (1.0*root->move[i].child->win) / (root->move[i].child->visit) + sqrt(UCB_K*log(root->visit) / root->move[i].child->visit);
		}
		else
		{
			MaxUCBMove = &root->move[i];
			break;
		}
		if (UCB > maxUCB)
		{
			maxUCB = UCB;
			MaxUCBMove = &root->move[i];
		}
	}
}

void UCTree::UCT()
{
	start = clock();
	end = clock();
	int brk, newwin, newvis;
	//�ȸ���mainMapΪmainMap֮��Ļָ���׼��
	initmap = *mainMap;
//	int times = 40000;
	while (end - start < MAXTIME)
//	while (times--)
	{
		brk = 0;
		//�ָ�mainMap
		memcpy(mainMap->mappoint, initmap.mappoint, sizeof(initmap.mappoint));
		memcpy(mainMap->whiteChess, initmap.whiteChess, sizeof(initmap.whiteChess));
		memcpy(mainMap->redChess, initmap.redChess, sizeof(initmap.redChess));
		memcpy(mainMap->MobVal, initmap.MobVal, sizeof(initmap.MobVal));
		mainMap->HashKey32 = initmap.HashKey32;
		mainMap->HashKey64 = initmap.HashKey64;

		//Ѱ�����UCB�����Ӧ���з���ִ��
		FindMaxUCB(root);
		mainMap->MakeMove(*MaxUCBMove, MaxUCBMove->color);
		bestNode = MaxUCBMove->child;
		bestNode->parent = root;
		bestNode->lastMove = MaxUCBMove;
		while (bestNode->visit>=MAXSIMULATE)	//�������ģ����ʱ
		{
			//û���з�˵��ΪҶ�ӽڵ㣬�����з�������չ
			if (bestNode->len == 0)
			{
				mainMap->CreatMove(3 - bestNode->color, 0);
				if (mainMap->MoveStack[0].len > 0)
				{
					/*ABTree::SearchGoodMove(1, 3 - bestNode->color);
					qsort(mainMap->MoveStack[0].moves, mainMap->MoveStack[0].len, sizeof(MoveType), comp1);
					if (mainMap->MoveStack[0].len > 400)
					mainMap->MoveStack[0].len = 400;*/
					bestNode->len = mainMap->MoveStack[0].len;
					bestNode->move = new MoveType[bestNode->len];
					for (int i = 0; i < mainMap->MoveStack[0].len; i++)
					{
						bestNode->move[i] = mainMap->MoveStack[0].moves[i];
						bestNode->move[i].child = NULL;
						bestNode->move[i].color = 3 - bestNode->color;
						bestNode->move[i].visit = 0;
						bestNode->move[i].win = 0;
						AddChild(bestNode, bestNode->move[i], *mainMap);
					}
				}
				else
				{
					//û�����ɳ��Է����з���ýڵ�Ӯ
					bestNode->visit += 1;
					bestNode->win += 1;
					for (nowChild = bestNode; nowChild->parent != NULL; nowChild = nowChild->parent)
					{
						//���¸��׽ڵ����Ϣ���ɲ�ֵ����Ӧʤ����
						newvis = nowChild->visit - nowChild->lastMove->visit;
						newwin = newvis - (nowChild->win - nowChild->lastMove->win);
						nowChild->parent->win += newwin;
						nowChild->parent->visit += newvis;
						nowChild->lastMove->visit = nowChild->visit;
						nowChild->lastMove->win = nowChild->win;
					}				
					brk = 1;
					break;
				}
			}
			//��������
			FindMaxUCB(bestNode);
			mainMap->MakeMove(*MaxUCBMove, MaxUCBMove->color);
			MaxUCBMove->child->parent = bestNode;
			bestNode = MaxUCBMove->child;
			bestNode->lastMove = MaxUCBMove;
		}
		if (brk)
		{
			end = clock();
			continue;
		}
		//if (bestNode->visit == MAXSIMULATE)	//��չ�ӽڵ�
		//{
		//	//����һ��alphbeta��ֵ��������ǰ400�з�
		//	int s = clock();
		//	mainMap->CreatMove(3 - bestNode->color, 0);
		//	if (mainMap->MoveStack[0].len > 0)
		//	{
		//		/*ABTree::SearchGoodMove(1, 3 - bestNode->color);
		//		qsort(mainMap->MoveStack[0].moves, mainMap->MoveStack[0].len, sizeof(MoveType), comp1);
		//		if (mainMap->MoveStack[0].len > 400)
		//			mainMap->MoveStack[0].len = 400;*/
		//		bestNode->len = mainMap->MoveStack[0].len;
		//		bestNode->move = new MoveType[bestNode->len];
		//		for (int i = 0; i < mainMap->MoveStack[0].len; i++)
		//		{
		//			bestNode->move[i] = mainMap->MoveStack[0].moves[i];
		//			bestNode->move[i].child = NULL;
		//			bestNode->move[i].color = 3 - bestNode->color;
		//			bestNode->move[i].visit = 0;
		//			bestNode->move[i].win = 0;
		//			AddChild(bestNode, bestNode->move[i], *mainMap);
		//		}
		//	}
		//	else
		//	{
		//		bestNode->visit += 1;
		//		bestNode->win += 1;
		//		for (nowChild = bestNode; nowChild->parent != NULL; nowChild = nowChild->parent)
		//		{
		//			//���¸��׽ڵ����Ϣ���ɲ�ֵ����Ӧʤ����
		//			newvis = nowChild->visit - nowChild->lastMove->visit;
		//			newwin = newvis - (nowChild->win - nowChild->lastMove->win);
		//			nowChild->parent->win += newwin;
		//			nowChild->parent->visit += newvis;
		//			nowChild->lastMove->visit = nowChild->visit;
		//			nowChild->lastMove->win = nowChild->win;
		//		}
		//		continue;
		//	}
		//	s = clock();
		//	//��չ�����ģ��һ��
		//	FindMaxUCB(bestNode);
		//	mainMap->MakeMove(*MaxUCBMove, MaxUCBMove->color);
		//	MaxUCBMove->child->parent = bestNode;
		//	bestNode = MaxUCBMove->child;
		//	bestNode->lastMove = MaxUCBMove;
		//	MC_init();
		//	int wintime = 0;
		//	for (int i = 0; i < MAXTHREAD; i++)
		//	{
		//		MC_Start(i, bestNode);
		//	}
		//	for (int i = 0; i < MAXTHREAD; i++)
		//	{
		//		while (MCPara[i].isrunning);
		//	}
		//	for (int i = 0; i < MAXTHREAD; i++)
		//	{
		//		wintime += MCwin[i];
		//	}
		//	bestNode->visit += MAXTHREAD;
		//	bestNode->win += wintime;
		//	for (nowChild = bestNode; nowChild->parent != NULL; nowChild = nowChild->parent)
		//	{
		//		newvis = nowChild->visit - nowChild->lastMove->visit;
		//		newwin = newvis - (nowChild->win - nowChild->lastMove->win);
		//		nowChild->parent->win += newwin;
		//		nowChild->parent->visit += newvis;
		//		nowChild->lastMove->visit = nowChild->visit;
		//		nowChild->lastMove->win = nowChild->win;
		//	}

		//}
		//else 
		if (bestNode->visit < MAXSIMULATE)
		{
			MC_init();
			int wintime = 0;
			for (int i = 0; i < MAXTHREAD; i++)
			{
				MC_Start(i, bestNode);
			}
			for(int i = 0; i < MAXTHREAD; i++)
			{
				while (MCPara[i].isrunning);
			}
			for (int i = 0; i < MAXTHREAD; i++)
			{
				wintime += MCwin[i];
			}
			
			bestNode->visit += MAXTHREAD;
			bestNode->win += wintime;
			for (nowChild = bestNode; nowChild->parent != NULL; nowChild = nowChild->parent)
			{
				newvis = nowChild->visit - nowChild->lastMove->visit;
				newwin = newvis - (nowChild->win - nowChild->lastMove->win);
				nowChild->parent->win += newwin;
				nowChild->parent->visit += newvis;
				nowChild->lastMove->visit = nowChild->visit;
				nowChild->lastMove->win = nowChild->win;
			}
		}
		end = clock();
	}
	//����ٻָ�һ�ε�ͼ����ΪmainMap�����ǵ���Ҫ������ͼ
	*mainMap = initmap;
}

MoveType UCTree::SearchGoodMove(int color)
{
	init(color);
	
	if (root->len == 0)
	{
		mainMap->CreatMove(color, 0);
		ABTree::SearchGoodMove(1, color);
		qsort(mainMap->MoveStack[0].moves, mainMap->MoveStack[0].len, sizeof(MoveType), comp1);
		if (mainMap->MoveStack[0].len > 400)
			mainMap->MoveStack[0].len = 400;

		root->len = mainMap->MoveStack[0].len;
		root->move = new MoveType[root->len];
		for (int i = 0; i < mainMap->MoveStack[0].len; i++)
		{
			root->move[i] = mainMap->MoveStack[0].moves[i];
			root->move[i].child = NULL;
			root->move[i].color = color;
			root->move[i].visit = 0;
			root->move[i].win = 0;
			AddChild(root, root->move[i], *mainMap);
		}
	}
	UCT();
	FILE *fp;
	fp = fopen("rate_out.txt", "a");
	int maxvis = 0;
	double rate, maxRate = -INF;
	MoveType *bestMove=NULL;
	fprintf(fp, "Sum:%d\n", root->len);
	for (int i = 0; i < root->len; i++)
	{
		if (root->move[i].visit == 0)
			continue;
		rate = 1.0* root->move[i].win / root->move[i].visit;
		if (rate>maxRate)
		{
			maxRate = rate;
			bestMove = &root->move[i];
		}
		fprintf(fp, "Num:%d win:%d vis:%d rate:%lf\n", i, root->move[i].win, root->move[i].visit, rate);
	}
	fprintf(fp, "Best: win:%d vis:%d rate:%lf\n", bestMove->win, bestMove->visit, maxRate);
	fprintf(fp, "Root vis:%d nodes: now:%d sum:%d\n", root->visit, now, sum);
	fclose(fp);

	//������������
	//root = bestMove->child;
	return *bestMove;
}

void UCTree::Save(Node *root)
{
	root->flag = true;
	for (int i = 0; i < root->len; i++)
	{
		if (root->move[i].child->len>0)
		{
			Save(root->move[i].child);
		}
		else
		{
			root->move[i].child->flag = true;
		}
	}
}