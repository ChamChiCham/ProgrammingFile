// 보드의 크기 4/4
// 8개의 다른 대문자를 배치
// color 1 2 3 4 5 6 7 8
// 2개의 좌표 입력받음. char[3]
// 횟수 15회, 점수 배점 점당 1
// 

#include <iostream>
#include <random>
#include <algorithm>
#include <vector>
#include <Windows.h>
#include <cassert>

using namespace std;

random_device seed;
mt19937 gen(seed());

class CSolve
{
private:
	struct SBoard
	{
		char card;
		bool reversed;

		SBoard() :
			card(NULL),
			reversed(false)
		{}
	};

	const int MOVE_LIMIT = 20;


	SBoard			board[4][4];
	pair<int, int>	choose[2];
	int				remain;
	int				score;

public:

	CSolve() :
		board{} ,
		choose{pair<int, int>(- 1, -1), pair<int, int>(-1, -1)} ,
		remain(MOVE_LIMIT) ,
		score(0)
	{
		initBoard();
	}

	void initBoard()
	{
		vector<char> rand_char = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
								'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H' };

		shuffle(rand_char.begin(), rand_char.end(), gen);

		for (auto& row : board) {
			for (auto& data : row) {
				data.card = rand_char.back();
				rand_char.pop_back();
				data.reversed = false;
			}
		}
	}

	void printBoard()
	{
		cout << "\ta\tb\tc\tb" << endl;
		int count = 1;

		for (int i = 0; i < 4; ++i) {
			cout << count++;
			for (int j = 0; j < 4; ++j) {
				if ((choose[0].first == j && choose[0].second == i) ||
					(choose[1].first == j && choose[1].second == i))
				{
					cout << "\tO";
					continue;
				}

				if (board[i][j].reversed) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), board[i][j].card - 'A' + 1);
					cout << "\t" << board[i][j].card;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				}
				else {
					cout << "\t*";
				}
			}
			cout << endl;
		}
		cout << "REMAIN : " << remain << " SCORE : " << score << endl << endl;
	}

	void processPoint()
	{
		Sleep(1500);

		if (board[choose[0].second][choose[0].first].card
			== board[choose[1].second][choose[1].first].card) {

			board[choose[0].second][choose[0].first].reversed = true;
			board[choose[1].second][choose[1].first].reversed = true;

			for (auto& cho : choose)
				cho = pair<int, int>(-1, -1);

			score++;
		}

		else {
			board[choose[0].second][choose[0].first].reversed = true;
			board[choose[1].second][choose[1].first].reversed = true;

			pair<int, int> temp[2];
			temp[0] = choose[0];
			temp[1] = choose[1];

			for (auto& cho : choose)
				cho = pair<int, int>(-1, -1);

			printBoard();

			Sleep(1500);

			board[temp[0].second][temp[0].first].reversed = false;
			board[temp[1].second][temp[1].first].reversed = false;
		}
	}
	
	const bool inputPoint(int& _x, int& _y)
	{
		string input;
		int x = 0, y = 0;

		cout << "input card : ";
		cin >> input;

		if (input[2] != NULL) {
			cout << "Wrong Insert" << endl << endl;
			return false;
		}

		if (input[0] == 'r') {
			score = 0;
			remain = MOVE_LIMIT;
			initBoard();
			cout << "Reset" << endl << endl;
			return false;
		}

		if (!remain) {
			cout << "No Remains" << endl << endl;
		}

		if (cin.fail()) {
			cout << "Invaild input" << endl << endl;
			cin.clear();
			cin.ignore(100);
			return false;
		}

		x = input[0] - 'a';
		y = input[1] - '1';

		if (!(0 <= x && x <= 3 && 0 <= y && y <= 3))
		{
			cout << "Out of range" << endl << endl;
			return false;
		}

		if (board[x][y].reversed)
		{
			cout << "Already reversed" << endl << endl;
			return false;
		}

		_x = x;
		_y = y;

		return true;
	}

	void doPoint()
	{
		int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		
		if (!inputPoint(x1, y1))
			return;

		choose[0] = pair<int, int>(x1, y1);
		printBoard();

		if (!inputPoint(x2, y2)) {
			return;
		}

		if (x1 == x2 && y1 == y2) {
			cout << "choosed Same Pos" << endl << endl;
			return;
		}

		choose[1] = pair<int, int>(x2, y2);
		printBoard();


		processPoint();

		remain--;

	}



	void run()
	{
		while (true)
		{
			printBoard();

			doPoint();

			for (auto& cho : choose)
				cho = pair<int, int>(-1, -1);
		}
	}

};

int main()
{
	CSolve program;
	program.run();
}