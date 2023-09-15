#include <iostream>
#include <utility>
#include <memory>
#include <string>
#include <random>

const int MIN_V = 0;
const int MAX_V = 2;
const int MAX_M = 3;

std::random_device rd;  // true 랜덤 시드 생성
std::mt19937 gen(rd()); // Mersenne Twister 19937 엔진 사용 (다른 엔진도 가능)
std::uniform_int_distribution<int> dist(MIN_V, MAX_V);

class MatrixProgram
{
private:
	std::pair<short[MAX_M][MAX_M], short[MAX_M][MAX_M]> matrix;
	char command;
	bool is_run;


public:

	MatrixProgram() :
		matrix(),
		command(NULL),
		is_run(true)
	{}

private:

	void printMatrix(const short p_matrix[MAX_M][MAX_M])
	{
		for (int i = 0; i < MAX_M; ++i) {
			for (int j = 0; j < MAX_M; ++j) {
				std::cout << p_matrix[i][j] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl << std::endl;
	}

	void printMatrix(const short p_matrix[MAX_M + 1][MAX_M + 1])
	{
		for (int i = 0; i < MAX_M + 1; ++i) {
			for (int j = 0; j < MAX_M + 1; ++j) {
				std::cout << p_matrix[i][j] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl << std::endl;
	}

	void readCommand()
	{
		std::cin >> command;

		if (std::cin.fail()) {
			std::cerr << "입력 오류 발생!" << std::endl;
			std::cin.clear();
			std::cin.ignore(100, '\n');
		}
	}

	void setRandomValue()
	{
		for (auto& row : matrix.first) {
			for (auto& element : row) {
				element = dist(gen);
			}
		}

		for (auto& row : matrix.second) {
			for (auto& element : row) {
				element = dist(gen);
			}
		}
	}


	void mulMatrix()
	{
		short result[MAX_M][MAX_M] = {};

		for (int i = 0; i < MAX_M; ++i) {
			for (int j = 0; j < MAX_M; ++j) {
				for (int k = 0; k < MAX_M; ++k) {
					result[i][j] += matrix.first[i][k] * matrix.second[k][j];
				}
			}
		}

		printMatrix(matrix.first);
		std::cout << "*" << std::endl << std::endl;
		printMatrix(matrix.second);
		std::cout << "=" << std::endl << std::endl;
		printMatrix(result);

	}

	void addMatrix()
	{
		short result[MAX_M][MAX_M] = {};

		for (int i = 0; i < MAX_M; ++i) {
			for (int j = 0; j < MAX_M; ++j) {
				result[i][j] = matrix.first[i][j] + matrix.second[i][j];
			}
		}

		printMatrix(matrix.first);
		std::cout << "+" << std::endl << std::endl;
		printMatrix(matrix.second);
		std::cout << "=" << std::endl << std::endl;
		printMatrix(result);
	}

	void difMatrix()
	{
		short result[MAX_M][MAX_M] = {};

		for (int i = 0; i < MAX_M; ++i) {
			for (int j = 0; j < MAX_M; ++j) {
				result[i][j] = matrix.first[i][j] - matrix.second[i][j];
			}
		}

		printMatrix(matrix.first);
		std::cout << "-" << std::endl << std::endl;
		printMatrix(matrix.second);
		std::cout << "=" << std::endl << std::endl;
		printMatrix(result);
	}

	void mulIntMatrix()
	{
		short result1[MAX_M][MAX_M] = {};
		short result2[MAX_M][MAX_M] = {};

		int value = command - '0';
		for (int i = 0; i < MAX_M; ++i) {
			for (int j = 0; j < MAX_M; ++j) {
				result1[i][j] = matrix.first[i][j] * value;
				result2[i][j] = matrix.second[i][j] * value;
			}
		}
		printMatrix(result1);
		printMatrix(result2);
	}

	int detMatrixCalc(short matrix[MAX_M][MAX_M])
	{
		return 
			(matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1])) -
			(matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0])) +
			(matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]));
	}

	// MAX_M이 3일 경우에만 작동
	void detMatrix()
	{
		if (MAX_M != 3)
			return;

		std::cout << detMatrixCalc(matrix.first) << " " << detMatrixCalc(matrix.second);
	}

	void TranMatrix()
	{
		short result1[MAX_M][MAX_M] = {};
		short result2[MAX_M][MAX_M] = {};

		for (int i = 0; i < MAX_M; ++i) {
			for (int j = 0; j < MAX_M; ++j) {
				result1[i][j] = matrix.first[j][i];
				result2[i][j] = matrix.second[j][i];
			}
		}

		printMatrix(result1);
		printMatrix(result2);
		std::cout << detMatrixCalc(result1) << " " << detMatrixCalc(result2) << std::endl;
	}

	void henceMartix()
	{
		short result1[MAX_M + 1][MAX_M + 1] = {};
		short result2[MAX_M + 1][MAX_M + 1] = {};

		for (int i = 0; i < MAX_M; ++i) {
			for (int j = 0; j < MAX_M; ++j) {
				result1[i][j] = matrix.first[i][j];
				result2[i][j] = matrix.second[i][j];
			}
		}
		result1[MAX_M][MAX_M] = result2[MAX_M][MAX_M] = 1;

		printMatrix(result1);
		printMatrix(result2);
		std::cout << detMatrixCalc(matrix.first) << " " << detMatrixCalc(matrix.second) << std::endl;

	}

	void doCommand()
	{
		switch (command)
		{
		case 'm':
			mulMatrix();
			break;
		case 'a':
			addMatrix();
			break;
		case 'd':
			difMatrix();
			break;
		case 'r':
			detMatrix();
			break;
		case 't':
			TranMatrix();
			break;
		case 'h':
			henceMartix();
			break;
		case 's':
			setRandomValue();
			break;

		case 'q':
			is_run = false;
			break;
		default:
			if (command >= '1' && command <= '9')
				mulIntMatrix();

			break;
		}
	}

public:

	void run()
	{
		setRandomValue();

		while (true) {

			readCommand();

			doCommand();

			if (!is_run)
				break;

		}
	}


};


int main()
{
	MatrixProgram program;

	program.run();
}