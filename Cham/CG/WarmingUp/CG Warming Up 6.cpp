#include <set>
#include <iostream>

using namespace std;

class CSovle
{
private:

	struct SPoint
	{
		int x;
		int y;

		SPoint() :
			x(0),
			y(0)
		{}

		SPoint(const int _x, const int _y) :
			x(_x),
			y(_y)
		{}
	};

	const int BOARD_SIZE = 30;

	pair<SPoint, SPoint> square;
	bool is_square;

public:
	CSovle() :
		square{},
		is_square(false)
	{}

private:

	void printBoard()
	{
		set<int> able_x;
		set<int> able_y;

		for (int i = square.first.x; i <= square.second.x; ++i) {
			int push_value = i;
			if (push_value < 0)
				push_value += BOARD_SIZE;
			else if (push_value >= BOARD_SIZE)
				push_value -= BOARD_SIZE;
			able_x.insert(push_value);
		}

		for (int i = square.first.y; i <= square.second.y; ++i) {
			int push_value = i;
			if (push_value < 0)
				push_value += BOARD_SIZE;
			else if (push_value >= BOARD_SIZE)
				push_value -= BOARD_SIZE;
			able_y.insert(push_value);
		}


		for (int i = 0; i < BOARD_SIZE; ++i) {
			for (int j = 0; j < BOARD_SIZE; ++j) {
				if (able_x.find(j) != able_x.end() &&
					able_y.find(i) != able_y.end())
					cout << "O ";
				else 
					cout << ". ";
			}
			cout << endl;
		}
		cout << endl;
	}

	void doCommand()
	{
		char input;
		cin >> input;
		switch (input)
		{
			// w a s d (이동) + - i j k l(변환) R
		case 'd':
			square.first.x++;
			square.second.x++;
			break;

		case 'a':
			square.first.x--;
			square.second.x--;
			break;
		case 's':
			square.first.y++;
			square.second.y++;
			break;
		case 'w':
			square.first.y--;
			square.second.y--;
			break;

		case '+':
			if (square.second.x != BOARD_SIZE - 1)
				square.second.x++;
			if (square.second.y != BOARD_SIZE - 1)
				square.second.y++;
			break;
		case '-':
			if (square.second.x != square.first.x)
				square.second.x--;
			if (square.second.y != square.first.y)
				square.second.y--;
			break;
		case 'l':
			if (square.second.x != BOARD_SIZE - 1)
				square.second.x++;
			break;
		case 'j':
			if (square.second.x != square.first.x)
				square.second.x--;
			break;
		case 'k':
			if (square.second.y != BOARD_SIZE - 1)
				square.second.y++;
			break;
		case 'i':
			if (square.second.y != square.first.y)
				square.second.y--;
			break;
		case 'R':
			is_square = false;
			break;
		default:
			break;
		}

		resize();

	}

	void resize()
	{
		if (square.second.x == -1) {
			square.second.x = BOARD_SIZE - 1;
			square.first.x += BOARD_SIZE;
		}
		if (square.second.y == -1) {
			square.second.y = BOARD_SIZE - 1;
			square.first.y += BOARD_SIZE;
		}

		if (square.first.x == BOARD_SIZE) {
			square.first.x = 0;
			square.second.x -= BOARD_SIZE;
		}

		if (square.first.y == BOARD_SIZE) {
			square.first.y = 0;
			square.second.y -= BOARD_SIZE;
		}
	}

	void makeSquare()
	{
		int data[4] = {};
		cout << "input Point : ";
		for (auto& input : data) {
			cin >> input;
		}

		for (auto& input : data) {
			if (!(0 <= input && input < BOARD_SIZE))
				return;
		}

		if (data[0] > data[2])
			swap(data[0], data[2]);

		if (data[1] > data[3])
			swap(data[1], data[3]);

		square = make_pair(SPoint(data[0], data[1]), SPoint(data[2], data[3]));
		is_square = true;

	}
	

public:
	void run()
	{
		while (true)
		{
			system("cls");

			if (!is_square) {
				makeSquare();
			}

			else {
				printBoard();

				doCommand();
			}
		}
	}


};

int main()
{
	CSovle program;
	program.run();
}