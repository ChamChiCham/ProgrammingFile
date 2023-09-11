#include <iostream>
#include <list>
#include <random>
#include <Windows.h>

using namespace std;

class CSolve
{
private:

	enum class EMOVE { N = -1, R, L, D, U };

	struct SMove
	{
		int dist;
		EMOVE dir;

		SMove() :
			dist(0),
			dir(EMOVE::N)
		{}

		SMove(const int _dist, const EMOVE _dir) :
			dist(_dist),
			dir(_dir)
		{}
	};

	enum { B_S = 50, T_C = 4 };
	bool board[B_S][B_S];
	list<SMove> move_list;
	int pos[2];
	bool is_run;
	bool is_start;
	list<SMove>::iterator move_iter;

public:
	CSolve() :
		board{},
		move_list{},
		pos{ -1, -1 },
		is_run(true),
		is_start(false),
		move_iter{}
	{
		for (auto& row : board)
			for (auto& data : row)
				data = true;


		// temp route, 수정 필요
		for (int i = 0; i < 9; ++i) {
			move_list.push_back(SMove(5, EMOVE::D));
			move_list.push_back(SMove(5, EMOVE::R));
		}
		move_list.push_back(SMove(4, EMOVE::D));
		move_list.push_back(SMove(4, EMOVE::R));
	}

private:

	void makeRoute()
	{
		list<SMove> mr_move_list;
		int mr_pos[2] = {};

		bool turn = true;

		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> range_dist(1, 5);
		uniform_int_distribution<int> range_dir(0, 5);

		while (true)
		{
			int dist = range_dist(gen);
			int dir_int = range_dir(gen);
			EMOVE dir_emove = EMOVE::N;

			// dir setting
			if (dir_int != 0) {
				if (turn) {
					dir_emove = EMOVE::R;
				}
				else
					dir_emove = EMOVE::D;
			}

			else {
				if (turn) {
					dir_emove = EMOVE::L;
				}
				else
					dir_emove = EMOVE::U;
			}

			// random created move
			switch (dir_emove)
			{
			case EMOVE::R:
				mr_pos[0] += dist;
				if (mr_pos[0] >= B_S) {
					mr_pos[0] -= dist * 2;
					dir_emove = EMOVE::L;
				}
				break;

			case EMOVE::L:
				mr_pos[0] -= dist;
				if (mr_pos[0] < 0) {
					mr_pos[0] += dist * 2;
					dir_emove = EMOVE::R;
				}
				break;
			case EMOVE::D:
				mr_pos[1] += dist;
				if (mr_pos[1] >= B_S) {
					mr_pos[1] -= dist * 2;
					dir_emove = EMOVE::U;
				}
				break;
			case EMOVE::U:
				mr_pos[1] -= dist;
				if (mr_pos[1] < 0) {
					mr_pos[1] += dist * 2;
					dir_emove = EMOVE::D;
				}
				break;
			default:
				break;
			}

			// push
			mr_move_list.push_back(SMove(dist, dir_emove));

			// finished. 나머지 2개의 move를 추가해 B_S에 맞춤.
			if (mr_pos[0] >= B_S - 5 && mr_pos[1] >= B_S - 5)
			{
				break;
			}

			turn = !turn;
		}

		if (B_S - 1 == mr_pos[0]) {
			mr_move_list.push_back(SMove(1, EMOVE::L));
			mr_pos[0]--;
			turn = !turn;
		}
		else if (B_S - 1 == mr_pos[1]) {
			mr_move_list.push_back(SMove(1, EMOVE::U));
			mr_pos[1]--;
			turn = !turn;
		}

		if (!turn) {
			if (B_S - mr_pos[0] - 1 != 0)
				mr_move_list.push_back(SMove(B_S - mr_pos[0] - 1, EMOVE::R));
			if (B_S - mr_pos[1] - 1 != 0)
				mr_move_list.push_back(SMove(B_S - mr_pos[1] - 1, EMOVE::D));
		}
		else {
			if (B_S - mr_pos[1] - 1 != 0)
				mr_move_list.push_back(SMove(B_S - mr_pos[1] - 1, EMOVE::D));
			if (B_S - mr_pos[0] - 1 != 0)
				mr_move_list.push_back(SMove(B_S - mr_pos[0] - 1, EMOVE::R));
		}

		move_list = mr_move_list;

	}

	void printBoard()
	{
		for (int i = 0; i < B_S; ++i) {
			for (int j = 0; j < B_S; ++j) {
				if (j == pos[0] && i == pos[1]) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), T_C);
					cout << "*";
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
					continue;
				}
				
				if (board[i][j])
					cout << "1";
				else
					cout << "0";
				
			}
			cout << endl;
		}
		cout << endl;
	}

	void makeBoard()
	{
		int now_pos[2] = {};

		board[0][0] = false;
		for (const auto& move : move_list) {
			switch (move.dir)
			{
			case EMOVE::R:
				for (int i = 0; i < move.dist; ++i) {
					board[now_pos[1]][++now_pos[0]] = false;
				}
				break;
			case EMOVE::L:
				for (int i = 0; i < move.dist; ++i) {
					board[now_pos[1]][--now_pos[0]] = false;
				}
				break;
			case EMOVE::D:
				for (int i = 0; i < move.dist; ++i) {
					board[++now_pos[1]][now_pos[0]] = false;
				}
				break;
			case EMOVE::U:
				for (int i = 0; i < move.dist; ++i) {
					board[--now_pos[1]][now_pos[0]] = false;
				}
				break;
			default:
				break;
			}
		}
	}

	void moveForward()
	{
		if (move_iter == move_list.end())
			return;

		switch (move_iter->dir)
		{
		case EMOVE::R:
			pos[0] += move_iter->dist;
			break;
		case EMOVE::L:
			pos[0] -= move_iter->dist;
			break;
		case EMOVE::D:
			pos[1] += move_iter->dist;
			break;
		case EMOVE::U:
			pos[1] -= move_iter->dist;
			break;
		default:
			break;
		}
		move_iter++;
	}

	void moveBack()
	{
		if (move_iter == move_list.begin())
			return;

		move_iter--;
		switch (move_iter->dir)
		{
		case EMOVE::R:
			pos[0] -= move_iter->dist;
			break;
		case EMOVE::L:
			pos[0] += move_iter->dist;
			break;
		case EMOVE::D:
			pos[1] -= move_iter->dist;
			break;
		case EMOVE::U:
			pos[1] += move_iter->dist;
			break;
		default:
			break;
		}
	}

	void startMove()
	{
		is_start = true;
		pos[0] = pos[1] = 0;
		move_iter = move_list.begin();
	}

	void doCommand()
	{
		char input;
		cin >> input;
		switch (input)
		{
		case 'm':
			makeNewRoute();
			break;
		case 'r':
			startMove();
			break;
		case '+':
			if (!is_start)
				break;
			moveForward();
			break;
		case '-':
			if (!is_start)
				break;
			moveBack();
			break;
		case 'q':
			is_run = false;
			break;
		default:
			break;
		}
	}

	void makeNewRoute()
	{
		for (auto& row : board)
			for (auto& data : row)
				data = true;

		makeRoute();

		pos[0] = pos[1] = -1;

		is_start = false;

		makeBoard();
	}

public:
	void run()
	{
		makeBoard();

		while (is_run)
		{
			system("cls");

			printBoard();

			doCommand();
		}
	}
};

int main()
{
	CSolve program;
	program.run();
}

