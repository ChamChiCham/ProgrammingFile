#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <iostream>
#include <vector>
#include <list>
#include <random>

#include "Struct.h"
#include "Define.h"
#include "CShaderManager.h"
#include "Utility.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> rand_color(0, 0xFFFFFF);

const double PI = 3.1415926535;

namespace cb
{
	GLvoid Display();
	GLvoid Reshape(int w, int h);
	GLvoid GameLoop(int value);
	GLvoid Timer1(int value);
	GLvoid Timer2(int value);
	GLvoid Timer3(int value);
	GLvoid Timer4(int value);
	GLvoid Mouse(int button, int state, int x, int y);
	GLvoid Keyboard(unsigned char key, int x, int y);
}


class CWindowMgr
{
private:

	enum {
		TRI_SIZE = 30,
	};

	// Define member Variable
	SFColor bg_color;
	CShaderManager ShaderMgr;

	// poligons
	std::vector<SPoligon> poligons;

	// buffer vari
	std::vector<SBuffer> bufs_pol;
	std::list<int> bufs_change;

	std::vector<SIPos> poligons_pos;

	int status = 0;
	
	int dir_x[4];
	int dir_y[4];

	int dir2[4];

	SIPos dir3_f[4];
	SIPos dir3_s[4];

	// 거리
	double dir4_r[4];
	// 라다안으로 저장
	double dir4_s[4];



public:

	CWindowMgr() :
		bg_color{ 1.f, 1.f, 1.f },
		ShaderMgr{},
		poligons{},
		bufs_pol{},
		bufs_change{},
		dir_x{},
		dir_y{},
		dir2{},
		dir3_f{},
		dir3_s{}
	{}
	
	~CWindowMgr() {}
	CWindowMgr(const CWindowMgr& other) = delete;
	CWindowMgr& operator=(const CWindowMgr& other) = delete;


public:

	// --
	// handle buffer func
	// --

	void updatePoligon(int _idx)
	{
		if (_idx > bufs_pol.size())
			return;

		SBuffer buf;

		buf.size = 0;
		// 정보를 넣는다
		for (const auto& vertex : poligons[_idx].vertices) {
			SFPos fpos = posWinToGL(vertex.pos);
			buf.VBO0_data.push_back(fpos.x);
			buf.VBO0_data.push_back(fpos.y);
			buf.VBO0_data.push_back(0.f);
			SFColor fcolor = colorWinToGL(vertex.color);
			buf.VBO1_data.push_back(fcolor.r);
			buf.VBO1_data.push_back(fcolor.g);
			buf.VBO1_data.push_back(fcolor.b); 
			buf.size++;
		}

		std::vector<unsigned int> idx;
		for (int i = 0; i < buf.size - 2; ++i) {
			idx.push_back(i);
			idx.push_back(i + 1);
			idx.push_back(i + 2);
		}

		glGenBuffers(2, buf.VBO);

		glBindBuffer(GL_ARRAY_BUFFER, buf.VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, buf.VBO0_data.size() * sizeof(GLfloat), buf.VBO0_data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, buf.VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, buf.VBO1_data.size() * sizeof(GLfloat), buf.VBO1_data.data(), GL_STATIC_DRAW);



		glGenBuffers(1, &buf.EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(unsigned int), idx.data(), GL_STATIC_DRAW);



		glGenVertexArrays(1, &buf.VAO);
		glBindVertexArray(buf.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, buf.VBO[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, buf.VBO[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.EBO);

		if (bufs_pol.size() == _idx)
			bufs_pol.push_back(buf);
		else
			bufs_pol[_idx] = buf;
	}

	void updateBuffer()
	{
		if (!bufs_change.empty()) {
			for (const auto& idx : bufs_change)
				updatePoligon(idx);
			bufs_change.clear();
		}
	}

	void drawPoligon(int _idx)
	{
		glBindVertexArray(bufs_pol[_idx].VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs_pol[_idx].EBO);
		glDrawElements(GL_TRIANGLES, (bufs_pol[_idx].size - 2) * 3, GL_UNSIGNED_INT, 0);
	}


	// --
	// process func
	// --

	void moveTriange(const int _idx)
	{
		SIPos& pos = poligons_pos[_idx];
		switch (status)
		{
		case 1:
			if (pos.x <= 0 + TRI_SIZE) {
				dir_x[_idx] = 1;
			}
			else if (pos.x >= 800 - TRI_SIZE) {
				dir_x[_idx] = -1;
			}

			if (pos.y <= 0 + TRI_SIZE) {
				dir_y[_idx] = 1;
			}
			else if (pos.y >= 600 - TRI_SIZE) {
				dir_y[_idx] = -1;
			}
			break;
		case 2:

			switch (dir2[_idx])
			{
				// x 고정, 오른쪽으로 진행
			case 0:
				if (dir_y[_idx] == -1 && pos.y <= 0 + TRI_SIZE) {
					if (pos.x >= 800 - TRI_SIZE) {
						dir2[_idx] = 1;
						pos.x -= TRI_SIZE;
						dir_y[_idx] = 1;
					}
					else {
						pos.x += TRI_SIZE;
						dir_y[_idx] = 1;
					}
				}

				if (dir_y[_idx] == 1 && pos.y >= 600 - TRI_SIZE) {
					if (pos.x >= 800 - TRI_SIZE) {
						dir2[_idx] = 1;
						pos.x -= TRI_SIZE;
						dir_y[_idx] = -1;
					}
					else {
						pos.x += TRI_SIZE;
						dir_y[_idx] = -1;
					}
				}

				break;
				// x 고정, 왼쪽으로 진행
			case 1:

				if (dir_y[_idx] == -1 && pos.y <= 0 + TRI_SIZE) {
					if (pos.x <= 0 + TRI_SIZE) {
						dir2[_idx] = 0;
						pos.x += TRI_SIZE;
						dir_y[_idx] = 1;
					}
					else {
						pos.x -= TRI_SIZE;
						dir_y[_idx] = 1;
					}
				}

				if (dir_y[_idx] == 1 && pos.y >= 600 - TRI_SIZE) {
					if (pos.x <= 0 + TRI_SIZE) {
						dir2[_idx] = 0;
						pos.x += TRI_SIZE;
						dir_y[_idx] = -1;
					}
					else {
						pos.x -= TRI_SIZE;
						dir_y[_idx] = -1;
					}
				}
				break;

				// y 고정, 위쪽으로 진행
			case 2:


				if (dir_x[_idx] == -1 && pos.x <= 0 + TRI_SIZE) {
					if (pos.y >= 600 - TRI_SIZE) {
						dir2[_idx] = 3;
						pos.y -= TRI_SIZE;
						dir_x[_idx] = 1;
					}
					else {
						pos.y += TRI_SIZE;
						dir_x[_idx] = 1;
					}
				}

				if (dir_x[_idx] == 1 && pos.x >= 800 - TRI_SIZE) {
					if (pos.y >= 600 - TRI_SIZE) {
						dir2[_idx] = 3;
						pos.y -= TRI_SIZE;
						dir_x[_idx] = -1;
					}
					else {
						pos.y += TRI_SIZE;
						dir_x[_idx] = -1;
					}
				}


				break;

				// y 고정, 아래로 진행
			case 3:


				if (dir_x[_idx] == -1 && pos.x <= 0 + TRI_SIZE) {
					if (pos.y <= 0 + TRI_SIZE) {
						dir2[_idx] = 2;
						pos.y += TRI_SIZE;
						dir_x[_idx] = 1;
					}
					else {
						pos.y -= TRI_SIZE;
						dir_x[_idx] = 1;
					}
				}

				if (dir_x[_idx] == 1 && pos.x >= 800 - TRI_SIZE) {
					if (pos.y <= 0 + TRI_SIZE) {
						dir2[_idx] = 2;
						pos.y += TRI_SIZE;
						dir_x[_idx] = -1;
					}
					else {
						pos.y -= TRI_SIZE;
						dir_x[_idx] = -1;
					}
				}
				break;
			default:
				break;
			}

			break;
		case 3:
			if (dir_x[_idx] == 1 && pos.x >= dir3_s[_idx].x - TRI_SIZE) {
				changeDir3(_idx, pos);
				if (dir3_s[_idx].x - dir3_f[_idx].x <= TRI_SIZE * 2)
					break;
				dir3_s[_idx].x -= TRI_SIZE;
			}

			else if (dir_x[_idx] == -1 && pos.x <= dir3_f[_idx].x + TRI_SIZE) {
				changeDir3(_idx, pos);
				if (dir3_s[_idx].x - dir3_f[_idx].x <= TRI_SIZE * 2)
					break;
				dir3_f[_idx].x += TRI_SIZE;
			}

			else if (dir_y[_idx] == 1 && pos.y >= dir3_s[_idx].y - TRI_SIZE) {
				changeDir3(_idx, pos);
				if (dir3_s[_idx].y - dir3_f[_idx].y <= TRI_SIZE * 2)
					break;
				dir3_s[_idx].y -= TRI_SIZE;
			}

			else if (dir_y[_idx] == -1 && pos.y <= dir3_f[_idx].y + TRI_SIZE) {
				changeDir3(_idx, pos);
				if (dir3_s[_idx].y - dir3_f[_idx].y <= TRI_SIZE * 2)
					break;
				dir3_f[_idx].y += TRI_SIZE;
			}

			break;
		case 4:

			if (dir4_r[_idx] < 300.)
				dir4_r[_idx] += 1.;

			dir4_s[_idx] += PI / 16.;

			if (dir4_s[_idx] >= 2 * PI)
				dir4_s[_idx] -= 2 * PI;

			pos.x = dir4_r[_idx] * cos(dir4_s[_idx]) + 400;
			pos.y = dir4_r[_idx] * sin(dir4_s[_idx]) + 300;

			updateTrianglePos(_idx);
			return;
		default:
			break;
		}

		if (dir_x[_idx] == -1) {
			pos.x -= 5;
		}
		else if (dir_x[_idx] == 1) {
			pos.x += 5;
		}

		if (dir_y[_idx] == -1) {
			pos.y -= 5;
		}
		else if (dir_y[_idx] == 1) {
			pos.y += 5;
		}

		updateTrianglePos(_idx);
	}

	void changeDir3(const int _idx, SIPos _pos)
	{
		SIPos mid = SIPos(400, 300);

		if (_pos.x >= mid.x && _pos.y >= mid.y) {
			dir_x[_idx] = 0;
			dir_y[_idx] = -1;
		}

		else if (_pos.x >= mid.x && _pos.y < mid.y) {
			dir_x[_idx] = -1;
			dir_y[_idx] = 0;
		}

		else if (_pos.x < mid.x && _pos.y < mid.y) {
			dir_x[_idx] = 0;
			dir_y[_idx] = 1;
		}

		else if (_pos.x < mid.x && _pos.y >= mid.y) {
			dir_x[_idx] = 1;
			dir_y[_idx] = 0;
		}
	}


	// -- 
	// Callback Function
	// --

	void Display()
	{
		glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(ShaderMgr.program);


		for (int i = 0; i < poligons.size(); ++i) {
			drawPoligon(i);
		}

		glutSwapBuffers();
	}

	void Mouse(const int _button, const int _state, const int _x, const int _y)
	{
		
	}

	void Keyboard(const unsigned char _key, const int _x, const int _y)
	{
		switch (_key)
		{
		case '1':

			if (status == 1)
				break;
			dir_x[0] = dir_x[1] = 1;
			dir_x[2] = dir_x[3] = -1;

			dir_y[0] = dir_y[2] = 1;
			dir_y[1] = dir_y[3] = -1;

			status = 1;
			break;

		case '2':

			if (status == 2)
				break;

			for (int i = 0; i < 4; ++i)
				dir2[i] = i;

			dir_x[0] = dir_x[1] = 0;
			dir_y[0] = 1;
			dir_y[1] = -1;


			dir_y[2] = dir_y[3] = 0;
			dir_x[2] = 1;
			dir_x[3] = -1;

			status = 2;
			break;

		case '3':

			for (int i = 0; i < 4; ++i) {
				dir3_f[i] = poligons_pos[i];
				dir3_s[i](800 - dir3_f[i].x, 600 - dir3_f[i].y);

				if (dir3_f[i].x > dir3_s[i].x) {
					int temp = dir3_f[i].x;
					dir3_f[i].x = dir3_s[i].x;
					dir3_s[i].x = temp;
				}

				if (dir3_f[i].y > dir3_s[i].y) {
					int temp = dir3_f[i].y;
					dir3_f[i].y = dir3_s[i].y;
					dir3_s[i].y = temp;
				}
				changeDir3(i, poligons_pos[i]);
			}


			
			status = 3;
			break;
		case '4':

			for (int i = 0; i < 4; ++i) {
				SIPos pos = poligons_pos[i];
				dir4_r[i] = sqrt(pow(400 - pos.x, 2) + pow(300 - pos.y, 2));
				if (pos.x == 400)
					dir4_s[i] = PI / 2;
				else 
					dir4_s[i] = atan2(pos.y - 300, pos.x - 400);

				if (dir4_s[i] < 0) {
					dir4_s[i] += 2 * PI;
				}
			}

			status = 4;
			break;
		default:
			break;
		}
	}

	// --
	// utility func
	// -- 

	void makeRect(const int _x1, const int _y1, const int _x2, const int _y2, const int _color)
	{
		SPoligon poli;
		poli(_x1, _y1, _color);
		poli(_x1, _y2, _color);
		poli(_x2, _y1, _color);
		poli(_x2, _y2, _color);
		poligons.push_back(poli);
		bufs_change.push_back(static_cast<int>(poligons.size()) - 1);
	}

	void makeTriangle(const int _x1, const int _y1, const int _x2, const int _y2, const int _x3, const int _y3, const int _color)
	{
		SPoligon poli;
		poli(_x1, _y1, _color);
		poli(_x2, _y2, _color);
		poli(_x3, _y3, _color);
		poligons.push_back(poli);
		bufs_change.push_back(static_cast<int>(poligons.size()) - 1);
	}

	void updateTrianglePos(const int _idx)
	{
		int x = poligons_pos[_idx].x;
		int y = poligons_pos[_idx].y;

		if (dir_x[_idx] == -1) {
			poligons[_idx].vertices[0].pos = SIPos(x - TRI_SIZE, y);
			poligons[_idx].vertices[1].pos = SIPos(x + TRI_SIZE, y + TRI_SIZE);
			poligons[_idx].vertices[2].pos = SIPos(x + TRI_SIZE, y - TRI_SIZE);
		}

		else if (dir_x[_idx] == 1) {
			poligons[_idx].vertices[0].pos = SIPos(x + TRI_SIZE, y);
			poligons[_idx].vertices[1].pos = SIPos(x - TRI_SIZE, y + TRI_SIZE);
			poligons[_idx].vertices[2].pos = SIPos(x - TRI_SIZE, y - TRI_SIZE);
		}

		else if (dir_y[_idx] == -1) {
			poligons[_idx].vertices[0].pos = SIPos(x, y - TRI_SIZE);
			poligons[_idx].vertices[1].pos = SIPos(x - TRI_SIZE, y + TRI_SIZE);
			poligons[_idx].vertices[2].pos = SIPos(x + TRI_SIZE, y + TRI_SIZE);
		}

		else if (dir_y[_idx] == 1) {
			poligons[_idx].vertices[0].pos = SIPos(x, y + TRI_SIZE);
			poligons[_idx].vertices[1].pos = SIPos(x - TRI_SIZE, y - TRI_SIZE);
			poligons[_idx].vertices[2].pos = SIPos(x + TRI_SIZE, y - TRI_SIZE);
		}
		bufs_change.push_back(_idx);
	}



	// init GL library
	void init(int& argc, char** argv)
	{
		// init GLUT
		glutInit(&argc, argv);
		glutInitDisplayMode(WINDOW_DISPLAYMODE);
		glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);
		glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
		glutCreateWindow(WINDOW_TITLE);

		// init GLEW
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cerr << "Unable to initialize GLEW" << std::endl;
			exit(EXIT_FAILURE);
		}
		else
			std::cout << "GLEW Initialized\n";

		ShaderMgr.makeProgram("Vertex.glsl", "Fragment.glsl");
		updateBuffer();

		// set cb func
		glutDisplayFunc(cb::Display);
		glutReshapeFunc(cb::Reshape);
		glutMouseFunc(cb::Mouse);
		glutKeyboardFunc(cb::Keyboard);
		glutTimerFunc(10, cb::GameLoop, 1);
		glutTimerFunc(20, cb::Timer1, 2);
		glutTimerFunc(30, cb::Timer2, 3);
		glutTimerFunc(40, cb::Timer3, 4);
		glutTimerFunc(50, cb::Timer4, 5);

		// set position
		int size = TRI_SIZE;
		makeTriangle(600, 150 - size, 600 - size, 150 + size, 600 + size, 150 + size, rand_color(gen));
		makeTriangle(200, 150 - size, 200 - size, 150 + size, 200 + size, 150 + size, rand_color(gen));
		makeTriangle(200, 450 - size, 200 - size, 450 + size, 200 + size, 450 + size, rand_color(gen));
		makeTriangle(600, 450 - size, 600 - size, 450 + size, 600 + size, 450 + size, rand_color(gen));
		poligons_pos.push_back(SIPos(600, 150));
		poligons_pos.push_back(SIPos(200, 150));
		poligons_pos.push_back(SIPos(200, 450));
		poligons_pos.push_back(SIPos(600, 450));

	}
	
	void updateGameStatus()
	{
		updateBuffer();

	}

	// Run Program
	void run()
	{
		glutMainLoop();
	}
};


CWindowMgr Window;

// (CALLBACK) Display screen
GLvoid cb::Display()
{
	Window.Display();
}

GLvoid cb::Timer1(int value)
{
	Window.moveTriange(0);
	glutTimerFunc(20, cb::Timer1, 2);
}

GLvoid cb::Timer2(int value)
{
	Window.moveTriange(1);
	glutTimerFunc(30, cb::Timer2, 3);
}

GLvoid cb::Timer3(int value)
{
	Window.moveTriange(2);
	glutTimerFunc(40, cb::Timer3, 4);
}

void cb::Timer4(int value)
{
	Window.moveTriange(3);
	glutTimerFunc(50, cb::Timer4, 5);
}

// (CALLBACK) Reset Viewport
GLvoid cb::Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

// (CALLBACK) Mouse click event
GLvoid cb::Mouse(int button, int state, int x, int y)
{
	Window.Mouse(button, state, x, y);
}

GLvoid cb::Keyboard(unsigned char key, int x, int y)
{
	Window.Keyboard(key, x, y);
}

// (CALLBACK) Main Loop
// FPS : 100
GLvoid cb::GameLoop(int value)
{

	// Game State update
	Window.updateGameStatus();

	// render (Display 함수 호출)
	glutPostRedisplay();

	glutTimerFunc(10, GameLoop, 1);
}



void main(int argc, char** argv)
{
	Window.init(argc, argv);
	Window.run();
}