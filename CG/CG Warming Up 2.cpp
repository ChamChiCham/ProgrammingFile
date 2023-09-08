// 데이터 읽기

// 읽은 문자열 출력, 공백을 기준으로 단어를 센다.

// 대문자가 포함된 단어의 개수를 세어 출력
// 
// 1개 이상의 공백 -> 1개
// 특문 구분 x
// 숫자와 문자열이 모두 있는 경우 문자열로 카운트
//  
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <sstream>
#include <cassert>

using namespace std;

class Solve
{
private:

	const char* FILE_NAME = "data.txt";

	ifstream		file;
	list<string>	lines;
	bool			is_run;

	struct SToggle
	{
		bool d;
		bool e;
		bool f;

		SToggle() :
			d(false),
			e(false),
			f(false)
		{}
	};

	SToggle toggle;

public:

	Solve() :
		file{} ,
		lines{} ,
		is_run(true) ,
		toggle{}
	{}


private:

	enum class WORD { NONE = 0, NUM = 1, CAP = 2 };

	void init()
	{
		file.open(FILE_NAME);
	
		if (!file.is_open()) {
			assert(false && "File isn't opened.");
		}
		
		string temp;
		while (getline(file, temp)) {
			lines.push_back(temp);
		}
	}

	void printAllLines()
	{
		for (list<string>::iterator iter = lines.begin(); iter != lines.end(); ++iter) {
			string output = *iter;

			if (toggle.d) {
				reverse(output.begin(), output.end());
			}

			if (toggle.e) {
				for (int i = 3; i < output.size(); i += 3) {
					output.insert(i++, "@");
					output.insert(i++, "@");
				}
			}

			if (toggle.f) {
				istringstream iss(output);
				string temp_word;
				string::iterator siter = output.begin();
				bool change = false;
				while (iss >> temp_word) {

					while (siter != output.end() && *siter == ' ') { siter++; }

					change = !change;

					if (change) {
						reverse(temp_word.begin(), temp_word.end());
						output.replace(siter, siter + temp_word.size(), temp_word);
					}

					siter += temp_word.size();
					while (siter != output.end() && *siter == ' ') { siter++; }
					temp_word.clear();
				}
			}

			cout << output << endl;
		}
		cout << endl;
	}

	const WORD checkWord(const string& word)
	{
		for (const auto& ch : word) {
			if ('0' < ch && ch > '9') {
				break;
			}

			if (ch == word.back()) {
				return WORD::NUM;
			}
		}

		for (const auto& ch : word) {
			if (isupper(ch)) {
				return WORD::CAP;
			}
		}

		return WORD::NONE;
	}

	void printWordsCount()
	{
		int wordCount = 0, numCount = 0, capCount = 0;


		for (const auto& line : lines) {

			istringstream iss(line);
			string word;
			while (iss >> word) {
				switch (checkWord(word))
				{
				case WORD::NUM:
					numCount++;
					break;
				case WORD::CAP:
					capCount++;

				case WORD::NONE:
					wordCount++;
					break;
				default:
					break;
				}
			}
		}

		cout << "Word Count : " << wordCount << endl;
		cout << "Number Count : " << numCount << endl;
		cout << "Capital Count : " << capCount << endl << endl;

	}

	void changeChar()
	{
		char old_char, new_char;
		cin >> old_char >> new_char;
		for (auto& line : lines) {
			for (auto& ch : line) {
				if (ch == old_char)
					ch = new_char;
			}
		}
	}

	void checkSymmetric()
	{
		for (const auto& line : lines) {
			istringstream iss(line);
			string word;
			string r_word;
			string sym_word;
			list<string> sym_words;
			while (iss >> word) {
				r_word = word;
				reverse(r_word.begin(), r_word.end());
				for (int i = 0; i < word.size() / 2; ++i) {
					if ()
				}
				if (!sym_word.empty()) {

				}
			}
		}
	}

	
	void doCommand()
	{
		char input;
		cin >> input;

		if (cin.fail()) {
			cerr << "Input Error" << endl;
			cin.clear();
			cin.ignore(100);
			return;
		}

		switch (input)
		{
			// 한줄 전체 거꾸로
		case 'd':
			toggle.d = !toggle.d;
			printAllLines();
			break;

			// 3문자 후 @ 삽입
		case 'e':
			toggle.e = !toggle.e;
			printAllLines();
			break;

			// 뒤집 원래 반복
		case 'f':
			toggle.f = !toggle.f;
			printAllLines();
			break;

		case 'g':
			changeChar();
			printAllLines();
			break;

		case 'h':
			checkSymmetric();
			break;

		case '+':

			break;

		case '-':

			break;

		case 'q':
			is_run = false;
			break;

		default:
			break;
		}
	}
	
	 
public:

	void run()
	{
		init();

		cout << "input data file name: " << FILE_NAME << endl;

		printAllLines();

		printWordsCount();

		
		while (is_run)
		{
			doCommand();

		}
		


	}



};


int main()
{
	Solve program;
	program.run();
}
