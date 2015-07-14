#include "map.h"
int MapType::dir[8][2] = { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 }, { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 } };

int MapType::step = 0;

int MapType::start;
int MapType::end;

void MapType::MakeMove(MoveType move ,int color)
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
}

void MapType::UnMakeMove(MoveType move, int color)
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
}

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

