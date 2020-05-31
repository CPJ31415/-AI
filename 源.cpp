#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <queue>
#include <time.h>
using namespace std;

struct point {
	int x;
	int y;
};

struct dir {   //方向
	int dx;
	int dy;
};

const dir d1 = { 0, 1 }; //横
const dir d2 = { 1, 0 }; //竖
const dir d3 = { 1, -1 }; //撇
const dir d4 = { 1, 1 }; //捺

class chesses {
private:
	int board[15][15] = {0};
	int player = 0;
	int computer = 0;
	int N = 0;
	point new_point;

public:
	int model();  //初始界面
	void show();  //每次刷新界面
	void turn_player();
	void turn_computer();
	bool IsInBoard(point p);   //判断棋子落点是否符合要求
	int score(point p, int mycolor);    //评估函数
	point newpoint(point p, dir d, int lenth);
	bool IsEnd();  //判断是否五子
	void play();  //开始

	queue<point> connect(queue<point> a, queue<point> b); //两个队列相连接
	queue<point> gen();   // 已下棋子周围两格范围内为预下点
	int min(int deep, int sco, int alpha, int beta);
	int max(int deep, int sco, int alpha, int beta);
	point maxmin(int deep);   //极大极小搜索及剪枝
};

int chesses::model() {
	system("cls");
	cout << "*************************************************" << endl;
	cout << "******************0、退出************************" << endl;
	cout << "******************1、电脑先手********************" << endl;
	cout << "******************2、玩家先手********************" << endl;
	cout << "**********输入位置即可落子: x(行) y(列)**********" << endl;
	cout << "*************************************************" << endl;
	while (true)
	{
		int a;
		cin >> a;
		if (a == 0)
			exit(1);
		else if (a == 1 || a == 2)
		{
			for (int i = 0; i < 15; i++)
				for (int j = 0; j < 15; j++)
					board[i][j] = 0;
			show();
			return a;
		}
	}
}

bool chesses::IsInBoard(point p) {
	if (p.y >= 0 && p.y < 15 && p.x >= 0 && p.x < 15)
		return true;
	return false;
}

void chesses::show() {
	system("cls");
	cout << "     1   2   3   4   5   6   7   8   9  10  11  12  13  14  15" << endl;
	cout << "   |---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|" << endl;
	for (int j = 0; j < 15; j++)
	{
		if (j < 9) cout << " " << j + 1 << " |";
		else cout << j + 1 << " |";
		for (int i = 0; i < 15; i++)
		{

			if (board[i][j] == 0) cout << "  ";
			if (board[i][j] == 1)
			{
				if (new_point.x == i && new_point.y == j)
					cout << "!o";
				else
					cout << " o";

			}
			if (board[i][j] == 2)
			{
				if (new_point.x == i && new_point.y == j)
					cout << "!x";
				else
					cout << " x";
			}

			cout << " |";
		}
		cout << endl;
		cout << "   |---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|" << endl;
	}
}

void chesses::turn_player() {
	int m, n;
	cout << endl << "输入横纵坐标" << endl;
	while (true)
	{
		cin >> n >> m;
		if (m > 0 && m <= 15 && n > 0 && n <= 15 && board[m - 1][n - 1] == 0)
		{
			board[m - 1][n - 1] = player;
			new_point = { m - 1, n - 1 };
			show();
			break;
		}
		else
		{
			show();
			cout << endl << "请重新输入横纵坐标" << endl;
		}
	}
}

void chesses::turn_computer() {
	new_point = maxmin(3);   //深度
	board[new_point.x][new_point.y] = computer;
	N = N + 1;
	show();
}

point chesses::newpoint(point p, dir d, int lenth) {
	point p1 = { p.x + d.dx * lenth, p.y + d.dy * lenth };
	return p1;
}

int chesses::score(point p, int mycolor) {
	int win5 = 0, alive4 = 0, die4 = 0, lowdie4 = 0, alive3 = 0,
		die3 = 0, tiao3 = 0, alive2 = 0, lowalive2 = 0, die2 = 0, nothing = 0;
	int hiscolor;
	hiscolor = 3 - mycolor;

	for (int i = 1; i <= 4; i++)
	{
		dir d;
		if (i == 1) d = d1;
		if (i == 2) d = d2;
		if (i == 3) d = d3;
		if (i == 4) d = d4;
		int count = 1;
		point le, ri, p1;
		int left[5], right[5]; //从连线两端开始1-4

		le = p;
		p1 = newpoint(p, d, -1);
		while (IsInBoard(p1) && board[p1.x][p1.y] == mycolor)
		{
			le = p1;
			p1 = newpoint(p1, d, -1);
			count++;
		}
		ri = p;
		p1 = newpoint(p, d, 1);
		while (IsInBoard(p1) && board[p1.x][p1.y] == mycolor)
		{
			ri = p1;
			p1 = newpoint(p1, d, 1);
			count++;
		}
		for (int j = 0; j <= 4; j++)
		{
			p1 = newpoint(le, d, -j);
			if (IsInBoard(p1))
				left[j] = board[p1.x][p1.y];
			else left[j] = hiscolor;
			p1 = newpoint(ri, d, j);
			if (IsInBoard(p1))
				right[j] = board[p1.x][p1.y];
			else right[j] = hiscolor;
		}

		if (count >= 5) win5++;
		else if (count == 4)
		{
			if (left[1] == 0 && right[1] == 0)
				alive4++;
			else if (left[1] == 0 || right[1] == 0)
				die4++;
			else
				nothing++;
		}
		else if (count == 3)
		{
			if (left[1] == 0 && right[1] == 0)
			{
				if (left[2] == hiscolor && right[2] == hiscolor)
					die3++;
				else if (left[2] == mycolor || right[2] == mycolor)
					lowdie4++;
				else if (left[2] == 0 || right[2] == 0)
					alive3++;
			}
			else if (left[1] == 0 || right[1] == 0)
			{
				if (left[1] == hiscolor)
				{
					if (right[2] == hiscolor)
						nothing++;
					if (right[2] == 0)
						die3++;
					if (right[2] == mycolor)
						lowdie4++;
				}
				if (right[1] == hiscolor)
				{
					if (left[2] == hiscolor)
						nothing++;
					if (left[2] == 0)
						die3++;
					if (left[2] == mycolor)
						lowdie4++;
				}
			}
			else
				nothing++;
		}
		else if (count == 2)
		{
			if (left[1] == 0 && right[1] == 0)
			{
				if ((right[2] == 0 && right[3] == mycolor) || (left[2] == 0 && left[3] == mycolor))
					die3++;
				else if (left[2] == 0 && right[2] == 0)
					alive2++;
				else if ((right[2] == mycolor && right[3] == hiscolor) || (left[2] == mycolor && left[3] == hiscolor))
					die3++;
				else if ((right[2] == mycolor && right[3] == mycolor) || (left[2] == mycolor && left[3] == mycolor))
					lowdie4++;
				else if ((right[2] == mycolor && right[3] == 0) || (left[2] == mycolor && left[3] == 0))
					tiao3++;
			}
			else if (left[1] == 0 || right[1] == 0)
			{
				if (left[1] == hiscolor)
				{
					if (right[2] == hiscolor || right[3] == hiscolor)
						nothing++;
					else if (right[2] == 0 && right[3] == 0)
						die2++;
					else if (right[2] == mycolor && right[3] == mycolor)
						lowdie4++;
					else if (right[2] == mycolor || right[3] == mycolor)
						die3++;
				}
				if (right[1] == hiscolor)
				{
					if (left[2] == hiscolor || left[3] == hiscolor)
						nothing++;
					else if (left[2] == 0 && left[3] == 0)
						die2++;
					else if (left[2] == mycolor && left[3] == mycolor)
						lowdie4++;
					else if (left[2] == mycolor || left[3] == mycolor)
						die3++;
				}
			}
			else
				nothing++;
		}
		else if (count == 1)
		{
			if (left[1] == 0 && left[2] == mycolor && left[3] == mycolor && left[4] == mycolor)
				lowdie4++;
			else if (right[1] == 0 && right[2] == mycolor && right[3] == mycolor && right[4] == mycolor)
				lowdie4++;
			else if (left[1] == 0 && left[2] == mycolor && left[3] == mycolor && left[4] == 0 && right[1] == 0)
				tiao3++;
			else if (right[1] == 0 && right[2] == mycolor && right[3] == mycolor && right[4] == 0 && left[1] == 0)
				tiao3++;
			else if (left[1] == 0 && left[2] == mycolor && left[3] == mycolor && left[4] == hiscolor && right[1] == 0)
				die3++;
			else if (right[1] == 0 && right[2] == mycolor && right[3] == mycolor && right[4] == hiscolor && left[1] == 0)
				die3++;
			else if (left[1] == 0 && left[2] == 0 && left[3] == mycolor && left[4] == mycolor)
				die3++;
			else if (right[1] == 0 && right[2] == 0 && right[3] == mycolor && right[4] == mycolor)
				die3++;
			else if (left[1] == 0 && left[2] == mycolor && left[3] == 0 && left[4] == mycolor)
				die3++;
			else if (right[1] == 0 && right[2] == mycolor && right[3] == 0 && right[4] == mycolor)
				die3++;
			else if (left[1] == 0 && left[2] == mycolor && left[3] == 0 && left[4] == 0 && right[1] == 0)
				lowalive2++;
			else if (right[1] == 0 && right[2] == mycolor && right[3] == 0 && right[4] == 0 && left[1] == 0)
				lowalive2++;
			else if (left[1] == 0 && left[2] == 0 && left[3] == mycolor && left[4] == 0 && right[1] == 0)
				lowalive2++;
			else if (right[1] == 0 && right[2] == 0 && right[3] == mycolor && right[4] == 0 && left[1] == 0)
				lowalive2++;
			else nothing++;
		}
	}
	if (win5 >= 1)
		return 100000;//赢5
	if (alive4 >= 1)
		return 20000;//活4
	if (alive3 >= 2)
		return 10000; //双活3
	if ((die4 >= 1 && alive3 >= 1) || die4 >= 2) //双死4 死4活3
		return 5000;
	if (die3 >= 1 && alive3 >= 1)
		return 1000;//死3高级活3
	if (die4 >= 1)
		return 500;//高级死4
	if (lowdie4 >= 1)
		return 400;//低级死4
	if (alive3 >= 1)
		return 100;//单活3
	if (tiao3 >= 1)
		return 90;//跳活3
	if (alive2 >= 2)
		return 50;//双活2
	if (alive2 >= 1)
		return 10;//活2
	if (lowalive2 >= 1)
		return 9;//低级活2
	if (die3 >= 1)
		return 5;//死3
	if (die2 >= 1)
		return 2;//死2
	return 1;//没有威胁
}

bool chesses::IsEnd() {
	for (int i = 1; i <= 4; i++)
	{
		dir d;
		int count = 0;
		if (i == 1) d = d1;
		if (i == 2) d = d2;
		if (i == 3) d = d3;
		if (i == 4) d = d4;
		for (int j = -4; j <= 4; j++)
		{
			point p1 = newpoint(new_point, d, j);
			if (IsInBoard(p1) && board[p1.x][p1.y] == board[new_point.x][new_point.y])
				count++;
			else count = 0;
			if (count == 5) return true;
		}
	}
	return false;
}

void chesses::play() {
	int a = model();
	if (a == 1)
	{
		computer = 1;
		player = 2;
		board[7][7] = computer;
		show();
	}
	if (a == 2)
	{
		player = 1;
		computer = 2;
	}
	int cur = player;
	while (true) {
		if (cur == player) turn_player();
		else turn_computer();
		if (IsEnd())
		{
			if (cur == player)
			{
				cout << "Player Win !";
				system("pause");
				break;
			}
			else
			{
				cout << "Computer Win !";
				system("pause");
				break;
			}
		}
		else
			cur = 3 - cur;
	}
}

queue<point> chesses::connect(queue<point>a, queue<point>b) {
	point c;
	while (!b.empty())
	{
		c = b.front();
		b.pop();
		a.push(c);
	}
	return a;
}

queue<point> chesses::gen() {
	queue<point> a, t[5];
	point p;
	int sco = 0;
	int osco = 0;
	for (int j = 0; j < 15; j++)
		for (int i = 0; i < 15; i++)
			if (board[i][j] == 0)
			{
				for (int m = 0; m < 5; m++)
					for (int n = 0; n < 5; n++)
					{
						if (i - 2 + n < 0 || j - 2 + m < 0 || i - 2 + n >= 15 || j - 2 + m >= 15)
							continue;
						if (board[i - 2 + n][j - 2 + m] != 0)
						{
							m = 5;
							p = { i, j };
							sco = score(p, 1);
							osco = score(p, 2);
							if (sco == 100000 || osco == 100000)
								t[0].push(p);
							else if (sco == 20000 || osco == 20000)
								t[1].push(p);
							else if (sco == 10000 || osco == 10000)
								t[2].push(p);
							else if (N >= 10 && (sco >= 400 || osco >= 500))
								t[3].push(p);
							else if (N >= 10 && (sco >= 50 || osco >= 90))
								t[4].push(p);
							else
								a.push(p);
							break;
						}
					}
			}
	if (!t[0].empty()) return t[0];
	if (!t[1].empty()) return t[1];
	if (!t[2].empty()) return t[2];
	if (N >= 10) return connect(t[3], connect(t[4], a));
	return a;
}

int chesses::min(int deep, int sco, int alpha, int beta) {
	if (deep <= 0)
		return sco;
	int best = 1000000;
	queue<point> p = gen();
	point MIN;
	while (!p.empty())
	{
		MIN = p.front();
		p.pop();
		board[MIN.x][MIN.y] = player;
		sco = sco - score(MIN, player);
		int v = max(deep - 1, sco, best < alpha ? best : alpha, beta);
		board[MIN.x][MIN.y] = 0;
		if (v < best)
			best = v;
		if (v < beta)
			break;
	}
	return best;
}

int chesses::max(int deep, int sco, int alpha, int beta) {
	if (deep <= 0)
		return sco;
	int best = -1000000;
	queue<point> p = gen();
	point MAX;
	while (!p.empty())
	{
		MAX = p.front();
		p.pop();
		board[MAX.x][MAX.y] = computer;
		sco = sco + score(MAX, computer);
		int v = min(deep - 1, sco, alpha, best > beta ? best : beta);
		board[MAX.x][MAX.y] = 0;
		if (v > best)
			best = v;
		if (v > alpha)
			break;
	}
	return best;
}

point chesses::maxmin(int deep) {
	int best = -1000000;
	int alpha = 1000000;
	int beta = -1000000;
	queue<point> p = gen();
	queue<point> bestpoints;
	point a, b = p.front();
	while (!p.empty())
	{
		a = p.front();
		p.pop();

		new_point = a;
		board[a.x][a.y] = computer;
		if (IsEnd()) return a;      //如果自己五子，返回
		board[a.x][a.y] = player;
		if (IsEnd()) return a;      //如果对方五子，返回
		board[a.x][a.y] = computer;

		int sco = score(a, computer);
		int v = min(deep - 1, sco, alpha, beta);
		if (v > best)
		{
			best = v;
			bestpoints = queue<point>();
			bestpoints.push(a);
		}
		if (v == best)
		{
			bestpoints.push(a);
		}
		board[a.x][a.y] = 0;
	}
	int len = bestpoints.size();    //随机取最优解中一个
	int ran = rand() % len + 1;
	cout << endl << ran << endl;
	for (int i = 0; i < ran; i++)
	{
		b = bestpoints.front();
		bestpoints.pop();
	}
	return b;
}

int main()
{
	system("mode con cols=70 lines=36");
	system("color 02");
	srand((unsigned int)time(NULL));
	chesses game;
	game.play();
}
