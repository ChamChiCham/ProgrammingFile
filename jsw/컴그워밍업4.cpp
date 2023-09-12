#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define SIZE 10

struct point {
	int x;
	int y;
	int z;
	bool check;
	point() : x(0), y(0), z(0), check(false) {}
};
void dotprint(point point[SIZE]) {
	for (int j = 9; j >= 0; j--) {
		if (point[j].check == true) {
			printf("%d : %d %d %d\n", j, point[j].x, point[j].y, point[j].z);
		}
	}
}
void upadd(point point[SIZE],int& i) {
	int change_idx = 0;
	for (int i = 9; i >= 0; i--) {
		if (point[i].check) {
			change_idx = i + 1;
			break;
		}
	}

	scanf("%d", &point[change_idx].x);
	scanf("%d", &point[change_idx].y);
	scanf("%d", &point[change_idx].z);
	point[change_idx].check=true;
	i++;
	dotprint(point);
	printf("\n");
}//+하면 위에서 나오는 함수
void updel(point point[SIZE], int& i) {

	for (int j = 9; j >= 0; j--) {
		if (point[j].check == true) {
			point[j].check = false;
			break;
		}
	}
	i--;
	dotprint(point);
	printf("\n");
}//-하면 위에서 지우는 함수
void downadd(point point[SIZE], int& i) {
	int x, y, z;
	scanf("%d", &x);
	scanf("%d", &y);
	scanf("%d", &z);

	if (point[0].check) {
		for (int j = 9; j >= 0; j--) {
			if (point[j].check == true) {
				point[j + 1].x = point[j].x;
				point[j + 1].y = point[j].y;
				point[j + 1].z = point[j].z;
				point[j + 1].check = true;
			}
		}
		point[0].x = x;
		point[0].y = y;
		point[0].z = z;
		point[0].check = true;
	}
	else {
		for (int i = 0; i < SIZE; i++) {
			if (point[i].check == true) {
				point[i - 1].x = x;
				point[i - 1].y = y;
				point[i - 1].z = z;
				point[i - 1].check = true;
				break;
			}
		}
	}
	i++;
	dotprint(point);
	printf("\n");
}//e 누르면 밑에서 나오는 함수
void downdel(point point[SIZE], int& i) {
	for (int j = 0; j < SIZE; j++) {
		if (point[j].check == true) {
			point[j].check = false;
			break;
		}
	}
	i--;
	dotprint(point);
	printf("\n");
}//d 누르면 밑에서 지우는 함수
void resetprint(point point[SIZE], int& i) {
	for (int j = 0; j < SIZE; j++) {
		point[j].check = false;
	}
	dotprint(point);
	i = 0;
}
void far(point point[SIZE], int& i) {
	int distance[SIZE];
	int max;
	int newx, newy, newz;
	for (int j = 0; j < i; j++) {
		distance[j] = (point[j].x * point[j].x) + (point[j].y * point[j].y) + (point[j].z * point[j].z);
	}
	max = distance[0];
	for (int j = 0; j < i; j++) {
		if (distance[j] > max) {
			max = distance[j];
			newx = point[j].x;
			newy = point[j].y;
			newz = point[j].z;
		}
	}
	printf("%d %d %d\n", newx, newy, newz);
}
void near(point point[SIZE], int& i) {
	int distance[SIZE];
	int min;
	int newx, newy, newz;
	for (int j = 0; j < i; j++) {
		distance[j] = (point[j].x * point[j].x) + (point[j].y * point[j].y) + (point[j].z * point[j].z);
	}
	min = distance[0];
	for (int j = 0; j < i; j++) {
		if (distance[j] < min) {
			min = distance[j];
			newx = point[j].x;
			newy = point[j].y;
			newz = point[j].z;
		}
	}
	printf("%d %d %d\n", newx, newy, newz);
}
void upsort(point point[SIZE], int& i) {
	int distance[SIZE];
	int tempx, tempy, tempz;
	for (int j = 0; j < i; j++) {
		distance[j] = (point[j].x * point[j].x) + (point[j].y * point[j].y) + (point[j].z * point[j].z);
		if (distance[j] < distance[j + 1]) {
			tempx = point[j].x;
			point[j].x = point[j + 1].x;
			point[j + 1].x = tempx;
			tempy = point[j].y;
			point[j].y = point[j + 1].y;
			point[j + 1].y = tempy;
			tempz = point[j].x;
			point[j].z = point[j + 1].z;
			point[j + 1].z = tempz;
		}
	}
	dotprint(point);
}
void downsort(point point[SIZE], int& i) {
	int distance[SIZE];
	int tempx, tempy, tempz;
	for (int j = 0; j < i; j++) {
		distance[j] = (point[j].x * point[j].x) + (point[j].y * point[j].y) + (point[j].z * point[j].z);
		if (distance[j] > distance[j + 1]) {
			tempx = point[j].x;
			point[j].x = point[j + 1].x;
			point[j + 1].x = tempx;
			tempy = point[j].y;
			point[j].y = point[j + 1].y;
			point[j + 1].y = tempy;
			tempz = point[j].x;
			point[j].z = point[j + 1].z;
			point[j + 1].z = tempz;
		}
	}
	dotprint(point);
}

int main() {
	bool check = false;
	char command;
	struct point dots[SIZE] = {};
	int i = 0;

	while (1) {
		scanf(" %c", &command);

		switch (command) {
		case '+':
			upadd(dots, i);
			break;
		case'-':
			updel(dots, i);
			break;
		case'e':
			downadd(dots, i);
			break;
		case'd':
			downdel(dots, i);
			break;
		case'l':
			printf("현재 저장된 리스트의 길이 : %d\n", i);
			break;
		case'c':
			resetprint(dots, i);
			break;
		case'm':
			far(dots, i);
			break;
		case'n':
			near(dots, i);
			break;
		case'a':
			upsort(dots, i);
			break;
		case's':
			downsort(dots, i);
			break;
		case'q':
			return 0;
		}
	}
	return 0;
}

//스트럭트 출력 존재 여부 true false로 체킹
//true면 값이 존재하는 것이므로 출력
//false면 출력 X
//값을 넣을 때 그 이프?인지 머일지는 모르겠지만 그 내부에 체킹이 true 변경되었다고 쓴다 check = true;
//값 지우는건 false를 이용해 지운다?
//-는 내부가 false인지 true인지 확인 하다가 true 만나면 거기는 false로 바꾼다 그러고 출력
//d는 밑에서부터 -와 같다
//넣기 위에서부터 false인지 체킹 true가 나오면 그 전 false에 값을 넣는다 다 차있으면 갔다버린다
//뒤에서 넣기 아래서부터 체크하고 true나오면 바로 아래인걸 바꾼다 그러나 바로 true가 나오면 올려서 00에 넣는다
//c는 싹다 false로 바꾼다
