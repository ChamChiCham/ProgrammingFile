#include <list>
#include <iostream>
#include <cassert>

using namespace std;

class CSolve
{
private:

	struct SData
	{
		int x;
		int y;
		int z;
		int idx;

		SData() :
			x(0),
			y(0),
			z(0),
			idx(-1)
		{}

		SData(const int _x, const int _y, const int _z, const int _idx) :
			x(_x),
			y(_y),
			z(_z),
			idx(_idx)
		{}
	};

	enum { T_N, T_A, T_S };

	list<SData> list_data;
	list<SData> sorted_list_data;
	int			start;
	int			end;

	bool		is_run;
	int			toggle;

public:

	CSolve() :
		list_data{},
		sorted_list_data{},
		start(0),
		end(0),
		is_run(true),
		toggle(T_N)
	{}

	void printList()
	{
		list<SData> print_data;

		if (toggle == T_N) {
			print_data = list_data;
		}
		else {
			print_data = sorted_list_data;
		}

		for (const auto& data : print_data) {
			cout << data.idx << " : " << data.x << " " << data.y << " "
				<< data.z << " " << endl;
		}
	}

	void pushBackList()
	{
		if (list_data.size() == 10)
			return;

		int x, y, z;
		cin >> x >> y >> z;
		list_data.push_back(SData(x, y, z, end++));
	}

	void popBackList()
	{
		if (list_data.empty())
			return;
		list_data.pop_back();
		end--;

		if (list_data.empty())
			start = end = 0;
	}

	void pushFrontList()
	{
		if (list_data.size() == 10)
			return;

		int x, y, z;
		cin >> x >> y >> z;

		if (start != 0) {
			list_data.push_front(SData(x, y, z, --start));
		}
		else {
			for (auto& data : list_data) {
				data.idx++;
			}
			list_data.push_front(SData(x, y, z, start));
			end++;
		}
	}

	void popFrontList()
	{
		if (list_data.empty())
			return;

		list_data.pop_front();
		start++;

		if (list_data.empty())
			start = end = 0;
	}

	void clearList()
	{
		start = 0;
		end = 0;
		list_data.clear();
	}

	const float calcDist(list<SData>::iterator& _iter)
	{
		return float(sqrt((pow(float(_iter->x), 2.0) + pow(float(_iter->y), 2.0) + pow(float(_iter->z), 2.0))));
	}

	list<SData>::iterator maxDistItor()
	{
		float max_value = 0.f;
		list<SData>::iterator max_iter;
		for (list<SData>::iterator iter = list_data.begin(); iter != list_data.end(); ++iter) {
			if (calcDist(iter) > max_value) {
				max_iter = iter;
				max_value = calcDist(iter);
			}
		}
		return max_iter;
	}

	void maxDist()
	{
		if (list_data.empty())
			return;

		list<SData>::iterator max_iter = maxDistItor();

		cout << max_iter->idx << " : " << max_iter->x << " "
			<< max_iter->y << " " << max_iter->z << endl << endl;
	}

	list<SData>::iterator minDistItor()
	{
		float min_value = 0.f;
		list<SData>::iterator min_iter;
		for (list<SData>::iterator iter = list_data.begin(); iter != list_data.end(); ++iter) {

			if (min_value == 0.f) {
				min_iter = iter;
				min_value = calcDist(iter);
			}
			else if (calcDist(iter) < min_value) {
				min_iter = iter;
				min_value = calcDist(iter);
			}
		}
		return min_iter;
	}

	void minDist()
	{
		if (list_data.empty())
			return;

		list<SData>::iterator min_iter = minDistItor();

		cout << min_iter->idx << " : " << min_iter->x << " "
			<< min_iter->y << " " << min_iter->z << endl << endl;
	}

	void sortMaxDist()
	{
		if (toggle == T_A) {
			toggle = T_N;
			return;
		}
		else {
			toggle = T_A;
		}

		list<SData> new_list;
		list<SData> temp_list = list_data;

		for (int i = 0; i < end - start; ++i) {
			list<SData>::iterator targ_iter = maxDistItor();
			new_list.push_back(SData(targ_iter->x, targ_iter->y, targ_iter->z, i));
			list_data.erase(targ_iter);
		}

		sorted_list_data = new_list;
		list_data = temp_list;
	}

	void sortMinDist()
	{
		if (toggle == T_S) {
			toggle = T_N;
			return;
		}
		else {
			toggle = T_S;
		}

		list<SData> new_list;
		list<SData> temp_list = list_data;

		for (int i = 0; i < end - start; ++i) {
			list<SData>::iterator targ_iter = minDistItor();
			new_list.push_back(SData(targ_iter->x, targ_iter->y, targ_iter->z, i));
			list_data.erase(targ_iter);
		}

		sorted_list_data = new_list;
		list_data = temp_list;
	}

	void doCommand()
	{
		char input;
		cin >> input;
		switch (input)
		{
		case '+':
			pushBackList();
			printList();
			break;
		case '-':
			popBackList();
			printList();
			break;
		case 'e':
			pushFrontList();
			printList();
			break;
		case 'd':
			popFrontList();
			printList();
			break;
		case 'c':
			clearList();
			break;
		case 'm':
			maxDist();
			break;
		case 'n':
			minDist();
			break;
		case 'a':
			sortMaxDist();
			printList();
			break;
		case 's':
			sortMinDist();
			printList();
			break;
		case 'q':
			is_run = false;
			break;
		default:
			break;
		}

		if (input != 'a' && input != 's') {
			if (toggle == T_A) {
				sortMaxDist();
			}
			else if (toggle == T_S) {
				sortMinDist();
			}
		}
	}



public:




	void run()
	{
		while (is_run)
		{
			doCommand();
		}
	}


};

int main()
{
	CSolve program;

	program.run();
}