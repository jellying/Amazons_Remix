#include "Evaluate.h"


int Eval::QueenMove[MAXTHREAD][10][10][2];
int Eval::KingMove[MAXTHREAD][10][10][2];

//计算双方QueenMove
void Eval::Queen(const MapType &map, int color,int threadNum)
{
	int vis[MAXSIZE][MAXSIZE];
	point queue[105];				//手写队列
	int head, tail;					//队列头尾指针
	head = tail = 0;
	memset(vis, 0, sizeof(vis));
	for (int i = 0; i < 4; i++)
	{
		if (color == 0)
		{
			vis[map.whiteChess[i].x][map.whiteChess[i].y] = 1;
			queue[tail++] = map.whiteChess[i];
		}
		else
		{
			vis[map.redChess[i].x][map.redChess[i].y] = 1;
			queue[tail++] = map.redChess[i];
		}
		QueenMove[threadNum][map.whiteChess[i].x][map.whiteChess[i].y][0] = 0;
		QueenMove[threadNum][map.redChess[i].x][map.redChess[i].y][1] = 0;

		QueenMove[threadNum][map.whiteChess[i].x][map.whiteChess[i].y][1] = 0;
		QueenMove[threadNum][map.redChess[i].x][map.redChess[i].y][0] = 0;
	}
	
	int i;
	point now, next;

	while (tail > head)
	{
		now = queue[head];
		head++;
		for (i = 0; i < 8; i++)
		{
			next.x = now.x + map.dir[i][0];
			next.y = now.y + map.dir[i][1];
			while (InBoard(next.x, next.y) && map.mappoint[next.x][next.y] == BLANK)
			{
				if (!vis[next.x][next.y])
				{
					vis[next.x][next.y] = 1;
					queue[tail++] = next;
					QueenMove[threadNum][next.x][next.y][color] = QueenMove[threadNum][now.x][now.y][color] + 1;
				}
				next.x = next.x + map.dir[i][0];
				next.y = next.y + map.dir[i][1];
			}
		}
	}
}

//计算双方KingMove
void Eval::King(const MapType& map, int color, int threadNum)
{
	int vis[MAXSIZE][MAXSIZE];
	point queue[105];				//手写队列
	int head, tail;					//队列头尾指针
	head = tail = 0;
	memset(vis, 0, sizeof(vis));
	for (int i = 0; i < 4; i++)
	{
		if (color == 0)
		{
			vis[map.whiteChess[i].x][map.whiteChess[i].y] = 1;
			queue[tail++] = map.whiteChess[i];
		}
		else
		{
			vis[map.redChess[i].x][map.redChess[i].y] = 1;
			queue[tail++] = map.redChess[i];
		}
		KingMove[threadNum][map.whiteChess[i].x][map.whiteChess[i].y][0] = 0;
		KingMove[threadNum][map.redChess[i].x][map.redChess[i].y][1] = 0;

		KingMove[threadNum][map.whiteChess[i].x][map.whiteChess[i].y][1] = 0;
		KingMove[threadNum][map.redChess[i].x][map.redChess[i].y][0] = 0;
	}

	int i;
	point now, next;
	while (tail > head)
	{
		now = queue[head];
		head++;
		for (i = 0; i < 8; i++)
		{
			next.x = now.x + map.dir[i][0];
			next.y = now.y + map.dir[i][1];
			if (!vis[next.x][next.y] && InBoard(next.x, next.y) && map.mappoint[next.x][next.y] == BLANK)
			{
				vis[next.x][next.y] = 1;
				queue[tail++] = next;
				KingMove[threadNum][next.x][next.y][color] = KingMove[threadNum][now.x][now.y][color] + 1;
			}
		}
	}
}

//计算双方灵活度差
double Eval::CalMob(const MapType& map, int threadNum,int color,double timew)
{
	point nowred, nowwhite, nextred, nextwhite;
	double m1 = 0, m2 = 0;
	double detaM = 0;
	int kingstep;
	double Score=0, mobility1=0, mobility2=0;
	double MobPara = 0.3*timew;
	if (map.step > 30)
		return 0;
	double whiteM = 0, redM = 0;
	for (int i = 0; i < 4; i++)		//计算双方灵活度
	{
		nowred.x = map.redChess[i].x;
		nowred.y = map.redChess[i].y;
		nowwhite.x = map.whiteChess[i].x;
		nowwhite.y = map.whiteChess[i].y;
		m1 = 0;
		m2 = 0;
		for (int j = 0; j < 8; j++)
		{
			nextred.x = nowred.x + map.dir[j][0];
			nextred.y = nowred.y + map.dir[j][1];
			nextwhite.x = nowwhite.x + map.dir[j][0];
			nextwhite.y = nowwhite.y + map.dir[j][1];
			kingstep = 0;
			while (InBoard(nextwhite.x, nextwhite.y) && map.mappoint[nextwhite.x][nextwhite.y] == BLANK)
			{
				if (QueenMove[threadNum][nextwhite.x][nextwhite.y][0] + QueenMove[threadNum][nextwhite.x][nextwhite.y][1] < 1000)
				{
					m1 += (1.0*map.MobVal[nextwhite.x][nextwhite.y] / pow(2, kingstep));
				}
				kingstep++;
				nextwhite.x = nextwhite.x + map.dir[j][0];
				nextwhite.y = nextwhite.y + map.dir[j][1];
			}
			kingstep = 0;
			while (InBoard(nextred.x, nextred.y) && map.mappoint[nextred.x][nextred.y] == BLANK)
			{
				if (QueenMove[threadNum][nextred.x][nextred.y][0] + QueenMove[threadNum][nextred.x][nextred.y][1] < 1000)
				{
					m2 += (1.0*map.MobVal[nextred.x][nextred.y] / pow(2, kingstep));
				}
				kingstep++;
				nextred.x = nextred.x + map.dir[j][0];
				nextred.y = nextred.y + map.dir[j][1];
			}
		}
		if (m1 < 1)
		{
			whiteM -= (20 * timew);
		}
		else if (m1 < 5)
		{
			whiteM -= (10 * timew);
		}
		else if (m1 < 20)
		{
			whiteM -= (5 * timew);
		}
		if (m2 < 1)
		{
			redM -= (20 * timew);
		}
		else if (m2 < 5)
		{
			redM -= (10 * timew);
		}
		else if (m2 < 20)
		{
			redM -= (5 * timew);
		}
		mobility1 += m1;
		mobility2 += m2;
	}
	double deathScore;
	if (color == WHITE)
	{
		detaM = mobility1 - mobility2;
		deathScore = (whiteM - redM);
	}
	else
	{
		detaM = mobility2 - mobility1;
		deathScore = (redM - whiteM);
	}
	Score += (deathScore + detaM*MobPara);
	return Score;
}

double Eval::Evaluate(int color, int threadNum,const MapType& map)
{
	
	for (int i = 0; i < MAXSIZE; i++)
	{
		for (int j = 0; j < MAXSIZE; j++)
		{
			QueenMove[threadNum][i][j][1] = QueenMove[threadNum][i][j][0] = 999;
			KingMove[threadNum][i][j][1] = KingMove[threadNum][i][j][0] = 999;
		}
	}
	Queen(map, 0,threadNum);		//0存储白色的QueenMove和KingMove
	Queen(map, 1, threadNum);				//1存储红色的QueenMove和KingMove

	King(map, 0, threadNum);
	King(map, 1, threadNum);
	double Score = 0;
	double Queen = 0, King = 0;
	double c1 = 0, c2 = 0;
	double w = 0, s;
	double mobility;
	double KPara = 1, QPara = 1, C1Para = 1, C2Para = 1, WPara = 1, MobPara = 1;
	double timew;
	

	for (int i = 0; i < MAXSIZE; i++)
	{
		for (int j = 0; j < MAXSIZE; j++)
		{
			if (map.mappoint[i][j] == BLANK)
			{
				if (QueenMove[threadNum][i][j][0] < QueenMove[threadNum][i][j][1])
					Queen++;
				if (QueenMove[threadNum][i][j][0] > QueenMove[threadNum][i][j][1])
					Queen--;
				/*if (QueenMove[thread][i][j][0] == QueenMove[thread][i][j][1])
				{
				if (color == WHITE)
				Queen += 0.1;
				else
				Queen -= 0.1;
				}*/
				if (KingMove[threadNum][i][j][0] < KingMove[threadNum][i][j][1])
					King++;
				if (KingMove[threadNum][i][j][0] > KingMove[threadNum][i][j][1])
					King--;
				/*if (KingMove[thread][i][j][0] == KingMove[thread][i][j][1])
				{
				if (color == WHITE)
				King += 0.1;
				else
				King -= 0.1;
				}*/
				c1 += (2 * (pow(2, -QueenMove[threadNum][i][j][color - 1]) - pow(2, -QueenMove[threadNum][i][j][2 - color])));
				s = MAX(-1, (KingMove[threadNum][i][j][2 - color] - KingMove[threadNum][i][j][color - 1])*1.0 / 6);
				c2 += (MIN(1, s));
				w += (pow(2, -fabs(1.0*QueenMove[threadNum][i][j][color - 1] - QueenMove[threadNum][i][j][2 - color])));
			}
		}
	}
	if (color == RED)
	{
		Queen = -Queen;
		King = -King;
	}
	timew = sqrt(w / 100);
	QPara = 1 - 1.2 * timew;
	KPara = 0.7*timew;
	C1Para = 0.25*timew;
	C2Para = 0.25*timew;
	mobility = CalMob(map, threadNum, color, timew);

	Score = Queen * QPara + King*KPara + c1*C1Para + c2*C2Para + mobility;
	return Score;
}

