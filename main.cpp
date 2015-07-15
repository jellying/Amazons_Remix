#ifndef _MAIN_CPP
#define _MAIN_CPP
#include "Define.h"
#include "AlphaBeta.h"

void load_map()		//载入局面调试
{
	int l1 = 0, l2 = 0;
	int loadmap[10][10] =
	{
		0, 0, 0, 0, 2, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
		0, 3, 1, 0, 0, 3, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 3, 0, 0,
		0, 0, 0, 0, 3, 3, 0, 0, 3, 3,
		3, 3, 0, 0, 0, 0, 3, 2, 0, 0,
		0, 0, 0, 0, 3, 0, 0, 0, 0, 0,
		2, 3, 3, 3, 3, 0, 2, 1, 0, 0,
		1, 0, 0, 0, 3, 0, 0, 0, 0, 0,
		0, 3, 3, 0, 3, 0, 0, 3, 0, 0,
	};
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			ABTree::mainMap.mappoint[i][j] = loadmap[i][j];
			if (ABTree::mainMap.mappoint[i][j] == RED)
			{
				ABTree::mainMap.redChess[l1].x = i;
				ABTree::mainMap.redChess[l1].y = j;
				l1++;
			}
			if (ABTree::mainMap.mappoint[i][j] == WHITE)
			{
				ABTree::mainMap.whiteChess[l2].x = i;
				ABTree::mainMap.whiteChess[l2].y = j;
				l2++;
			}
		}
	}
	MapType::step = 0;
}

//初始化地图并计算哈希值
void init_map()
{
	int inimap[10][10] = {
		0, 0, 0, RED, 0, 0, RED, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		RED, 0, 0, 0, 0, 0, 0, 0, 0, RED,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		WHITE, 0, 0, 0, 0, 0, 0, 0, 0, WHITE,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, WHITE, 0, 0, WHITE, 0, 0, 0,
	};

	int l1 = 0, l2 = 0;
	ABTree::mainMap.HashKey32 = 0;
	ABTree::mainMap.HashKey64 = 0;
	int chessType;
	for (int j = 0; j < MAXSIZE; j++)
	{
		for (int k = 0; k < MAXSIZE; k++)
		{
			ABTree::mainMap.mappoint[j][k] = inimap[j][k];
			if (ABTree::mainMap.mappoint[j][k] == RED)
			{
				ABTree::mainMap.redChess[l1].x = j;
				ABTree::mainMap.redChess[l1].y = k;
				l1++;
			}
			if (ABTree::mainMap.mappoint[j][k] == WHITE)
			{
				ABTree::mainMap.whiteChess[l2].x = j;
				ABTree::mainMap.whiteChess[l2].y = k;
				l2++;
			}
			if (inimap[j][k] != BLANK)
			{
				chessType = inimap[j][k];
				ABTree::mainMap.HashKey32 ^= MapType::HashRand32[j][k][chessType];
				ABTree::mainMap.HashKey64 ^= MapType::HashRand64[j][k][chessType];
			}
		}
	}
	int x, y;
	memset(ABTree::mainMap.MobVal, 0, sizeof(ABTree::mainMap.MobVal));
	for (int i = 0; i < MAXSIZE; i++)
	{
		for (int j = 0; j < MAXSIZE; j++)
		{
			if (ABTree::mainMap.mappoint[i][j] == BLANK)
			{
				for (int k = 0; k < 8; k++)
				{
					x = i + MapType::dir[k][0];
					y = j + MapType::dir[k][1];
					if (InBoard(x, y) && ABTree::mainMap.mappoint[x][y] == BLANK)
					{
						ABTree::mainMap.MobVal[x][y]++;
					}
				}
			}
		}
	}
}

//检查是否进入填格阶段
int checkFill()
{
	for (int i = 0; i < MAXSIZE; i++)
	{
		for (int j = 0; j < MAXSIZE; j++)
		{
			Eval::QueenMove[0][i][j][1] = Eval::QueenMove[0][i][j][0] = 999;
		}
	}
	Eval::Queen(ABTree::mainMap, 0, 0);
	Eval::Queen(ABTree::mainMap, 1, 0);
	for (int i = 0; i < MAXSIZE; i++)
	{
		for (int j = 0; j < MAXSIZE; j++)
		{
			if (ABTree::mainMap.mappoint[i][j] == BLANK)
			{
				if (Eval::QueenMove[0][i][j][0] + Eval::QueenMove[0][i][j][1] < 1000)
				{
					return 0;
				}
			}
		}
	}
	return 1;
}

//处理move 命令
void cmd_move(int color)
{
	FILE *fp;
	fp = fopen("out.txt", "a");
	memset(MapType::HashOK, 0, sizeof(MapType::HashOK));
	char move[] = "move AAAAAA\n";	//走法
	if (ABTree::FillBlank == 0 && MapType::step > 40)	//40步之后检查是否到了填格子阶段
	{
		ABTree::FillBlank = checkFill();
	}
	
	MoveType bestMove;
	int s = clock();
	bestMove = ABTree::DeepingIter(3, color);
//	fprintf(fp,"%dms\n", clock() - s);
	ABTree::mainMap.MakeMove(bestMove, color);
//	fprintf(fp, "val:%lf\n", bestMove.val);

	for (int i = 0; i < 4; i++)
	{
//		fprintf(fp,"%d\n", MapType::HashOK[i]);
	}
	
	//将着法转换成要发送的字符形式
	move[5] = bestMove.x[0] + 'A';
	move[6] = bestMove.y[0] + 'A';
	move[7] = bestMove.x[1] + 'A';
	move[8] = bestMove.y[1] + 'A';
	move[9] = bestMove.x[2] + 'A';
	move[10] = bestMove.y[2] + 'A';

	//发送信息
	printf("%s", move);
	fflush(stdout);
//	fprintf(fp, "%s\n", move);
	fclose(fp);
}

int main()
{
	int ourcolor, enemycolor;
	MoveType bestMove;
	char Msg[500] = { 0 };		//保存接收到的消息
	char name[] = "name Remix1.0\n";	//队伍信息
	ABTree::init();
	init_map();
	while (1)
	{
		//循环接收裁判平台发送的消息
		//注意需要发送的字符串应该以'\n'结束，裁判平台才会认为是一次完整的输入
		//发送完需要调用fflush(stdout)清空输出缓冲区，使字符串立刻输出到裁判平台

		memset(Msg, 0, 500);
		scanf("%s", Msg);

		if (strcmp(Msg, "name?") == 0)
		{
			//name?
			printf("%s", name);
			fflush(stdout);
			continue;
		}
		if (strcmp(Msg, "stop") == 0)
		{

			fflush(stdout);
			return 0;
		}


		if (strcmp(Msg, "new") == 0)
		{

			scanf("%s", Msg);

			if (strcmp(Msg, "white") == 0)
			{
				//new white
				ourcolor = WHITE;
				enemycolor = RED;
				cmd_move(ourcolor); //白方先走，所以搜索第一步走法
				continue;
			}
			else
			{
				//new red
				ourcolor = RED;
				enemycolor = WHITE;
				continue;
			}

			continue;
		}

		if (strcmp(Msg, "move") == 0)
		{
			//move
			scanf("%s", Msg);
			if (Msg[6] == '\0')
			{
				//move XX\n
				bestMove.x[0] = Msg[0] - 'A';
				bestMove.y[0] = Msg[1] - 'A';
				bestMove.x[1] = Msg[2] - 'A';
				bestMove.y[1] = Msg[3] - 'A';
				bestMove.x[2] = Msg[4] - 'A';
				bestMove.y[2] = Msg[5] - 'A';
				//执行对方招法
				ABTree::mainMap.MakeMove(bestMove, enemycolor);
			}
			cmd_move(ourcolor);
			continue;
		}

		if (strcmp(Msg, "load") == 0)
			load_map();

		if (strcmp(Msg, "exit") == 0)
		{
			ABTree::exit();
			return 0;
		}

	}

	return 0;
}
#endif