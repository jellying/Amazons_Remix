#include "map.h"
int MapType::dir[8][2] = { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 }, { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 } };

int MapType::step = 0;

int MapType::start;
int MapType::end;
Un64 MapType::HashRand64[MAXSIZE][MAXSIZE][4];
unsigned int MapType::HashRand32[MAXSIZE][MAXSIZE][4];
HashItem MapType::HashTable[2500000][2];
int MapType::HashOK[4];

//执行、撤销招法
void MapType::MakeMove(const MoveType &move ,int color)
{
	mappoint[move.x[0]][move.y[0]] = BLANK;
	mappoint[move.x[1]][move.y[1]] = color;
	mappoint[move.x[2]][move.y[2]] = STONE;

	for (int i = 0; i < 4; i++)
	{
		if (color == WHITE)
		{
			if (whiteChess[i].x == move.x[0] && whiteChess[i].y == move.y[0])
			{
				whiteChess[i].x = move.x[1];
				whiteChess[i].y = move.y[1];
				break;
			}
		}
		else
		{
			if (redChess[i].x == move.x[0] && redChess[i].y == move.y[0])
			{
				redChess[i].x = move.x[1];
				redChess[i].y = move.y[1];
				break;
			}
		}
	}

	HashMove(move, color);
	MobMove(move, color);
}

void MapType::UnMakeMove(const MoveType &move, int color)
{
	mappoint[move.x[2]][move.y[2]] = BLANK;
	mappoint[move.x[1]][move.y[1]] = BLANK;
	mappoint[move.x[0]][move.y[0]] = color;

	for (int i = 0; i < 4; i++)
	{
		if (color == WHITE)
		{
			if (whiteChess[i].x == move.x[1] && whiteChess[i].y == move.y[1])
			{
				whiteChess[i].x = move.x[0];
				whiteChess[i].y = move.y[0];
				break;
			}
		}
		else
		{
			if (redChess[i].x == move.x[1] && redChess[i].y == move.y[1])
			{
				redChess[i].x = move.x[0];
				redChess[i].y = move.y[0];
				break;
			}
		}
	}
	UnHashMove(move, color);
	UnMobMove(move, color);
}

//生成招法
void MapType::CreatMove(int color,int depth)
{
	point next, stone, now[4];
	int l = 0;
	if (color == WHITE)
	{
		memcpy(now, whiteChess, sizeof(whiteChess));
	}
	else
	{
		memcpy(now, redChess, sizeof(redChess));
	}
	for (int i = 0; i < 4; i++)									//第一层循环找一个棋子
	{
		mappoint[now[i].x][now[i].y] = BLANK;							//首先将起点的棋子拿起，更新为空点
		for (int j = 0; j < 8; j++)								//第二个循环找落子的方向
		{
			next.x = now[i].x + dir[j][0];
			next.y = now[i].y + dir[j][1];
			while (InBoard(next.x,next.y) && mappoint[next.x][next.y] == BLANK)								//朝这个方向搜索所有落子点
			{
				for (int k = 0; k < 8; k++)						//第三个循环选择放箭的方向
				{
					stone.x = next.x + dir[k][0];
					stone.y = next.y + dir[k][1];
					while (InBoard(stone.x,stone.y) && mappoint[stone.x][stone.y] == BLANK)					//朝这个方向搜索所有放箭点
					{
						MoveStack[depth].moves[l].x[0] = now[i].x;
						MoveStack[depth].moves[l].y[0] = now[i].y;
						MoveStack[depth].moves[l].x[1] = next.x;
						MoveStack[depth].moves[l].y[1] = next.y;
						MoveStack[depth].moves[l].x[2] = stone.x;
						MoveStack[depth].moves[l++].y[2] = stone.y;							//用招法栈保存招法					

						stone.x += dir[k][0];
						stone.y += dir[k][1];
					}
				}
				next.x += dir[j][0];
				next.y += dir[j][1];
			}
		}
		mappoint[now[i].x][now[i].y] = color;													//恢复拿起棋子点内容
	}
	MoveStack[depth].len = l;
}

//生成随机数
Un64 MapType::rand64()
{
	Un64 randnum = rand();
	for (int i = 0; i < 4; i++)
	{
		randnum <<= 15;
		randnum ^= rand();
	}
	return randnum;
}

unsigned int MapType::rand32()
{
	unsigned int randnum = rand();
	for (int i = 0; i < 2; i++)
	{
		randnum <<= 15;
		randnum ^= rand();
	}
	return randnum;
}

//初始化随机哈希值
void MapType::Hash_init()
{
	for (int i = 0; i < MAXSIZE; i++)
	{
		for (int j = 0; j < MAXSIZE; j++)
		{
			for (int k = 1; k < 4; k++)
			{
				HashRand32[i][j][k] = rand32();
				HashRand64[i][j][k] = rand64();
			}
		}
	}
}

//在执行招法撤销招法中修改哈希值
void MapType::HashMove(const MoveType &move, int color)
{
	HashKey32 ^= HashRand32[move.x[0]][move.y[0]][color];
	HashKey64 ^= HashRand64[move.x[0]][move.y[0]][color];

	HashKey32 ^= HashRand32[move.x[1]][move.y[1]][color];
	HashKey64 ^= HashRand64[move.x[1]][move.y[1]][color];

	HashKey32 ^= HashRand32[move.x[2]][move.y[2]][STONE];
	HashKey64 ^= HashRand64[move.x[2]][move.y[2]][STONE];
}

void MapType::UnHashMove(const MoveType &move, int color)
{

	HashKey32 ^= HashRand32[move.x[2]][move.y[2]][STONE];
	HashKey64 ^= HashRand64[move.x[2]][move.y[2]][STONE];

	HashKey32 ^= HashRand32[move.x[1]][move.y[1]][color];
	HashKey64 ^= HashRand64[move.x[1]][move.y[1]][color];

	HashKey32 ^= HashRand32[move.x[0]][move.y[0]][color];
	HashKey64 ^= HashRand64[move.x[0]][move.y[0]][color];

}

//执行招法
void MapType::MobMove(const MoveType &move, int color)
{
	int nx[3], ny[3];
	for (int i = 0; i < 8; i++)//更新三个点的自由度和周边格子的自由度
	{
		nx[0] = move.x[0] + dir[i][0];
		ny[0] = move.y[0] + dir[i][1];
		nx[1] = move.x[1] + dir[i][0];
		ny[1] = move.y[1] + dir[i][1];
		nx[2] = move.x[2] + dir[i][0];
		ny[2] = move.y[2] + dir[i][1];
		if (InBoard(nx[0], ny[0]) && mappoint[nx[0]][ny[0]] == BLANK)
		{
			MobVal[nx[0]][ny[0]]++;
			MobVal[move.x[0]][move.y[0]]++;
		}
		if (InBoard(nx[1], ny[1]) && mappoint[nx[1]][ny[1]] == BLANK && (nx[1] != move.x[0] || ny[1] != move.y[0]))
		{
			MobVal[nx[1]][ny[1]]--;
		}
		if (InBoard(nx[2], ny[2]) && mappoint[nx[2]][ny[2]] == BLANK && (nx[2] != move.x[0] || ny[2] != move.y[0]))
		{
			MobVal[nx[2]][ny[2]]--;
		}
	}
	MobVal[move.x[2]][move.y[2]] = 0;//放置障碍点自由度为0，周围点减1
	MobVal[move.x[1]][move.y[1]] = 0;//落子点自由度为0
}

void MapType::UnMobMove(const MoveType &move, int color)
{
	int nx[3], ny[3];
	for (int i = 0; i < 8; i++)//更新三个点的自由度和周边格子的自由度
	{
		nx[0] = move.x[0] + dir[i][0];
		ny[0] = move.y[0] + dir[i][1];
		nx[1] = move.x[1] + dir[i][0];
		ny[1] = move.y[1] + dir[i][1];
		nx[2] = move.x[2] + dir[i][0];
		ny[2] = move.y[2] + dir[i][1];
		if (InBoard(nx[0], ny[0]) && mappoint[nx[0]][ny[0]] == BLANK && (nx[0] != move.x[1] || ny[0] != move.y[1]) && (nx[0] != move.x[2] || ny[0] != move.y[2]))
		{
			MobVal[nx[0]][ny[0]]--;
		}
		if (InBoard(nx[1], ny[1]) && mappoint[nx[1]][ny[1]] == BLANK)
		{
			if (nx[1] != move.x[2] || ny[1] != move.y[2])
			{
				MobVal[nx[1]][ny[1]]++;
			}
			MobVal[move.x[1]][move.y[1]]++;
		}
		if (InBoard(nx[2], ny[2]) && mappoint[nx[2]][ny[2]] == BLANK)
		{
			if (nx[2] != move.x[1] || ny[2] != move.y[1])
			{
				MobVal[nx[2]][ny[2]]++;
			}
			MobVal[move.x[2]][move.y[2]]++;
		}
	}
	MobVal[move.x[0]][move.y[0]] = 0;
}