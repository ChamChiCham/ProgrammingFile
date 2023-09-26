#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <iostream>
#include <list>
#include <random>

// --
// declare callback func
// --

namespace cb
{
	GLvoid Display();
	GLvoid Reshape(int w, int h);
	GLvoid GameLoop(int value);
	GLvoid Timer(int value);
	GLvoid Mouse(int button, int state, int x, int y);
}

// --
// global vari
// --

// random 
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dist_color(0.f, 1.f);
std::uniform_int_distribution<int> dist_posx(0, 700);
std::uniform_int_distribution<int> dist_posy(0, 500);
std::uniform_int_distribution<int> dist_type(1, 3);

// --
// define class
// --

class CWindowMgr
{
private:

	// --
	// Define Constant
	// --
	enum {

		// Display Mode
		WINDOW_DISPLAYMODE = GLUT_DOUBLE | GLUT_RGBA,

		// Window Position
		WINDOW_POSITION_X = 100,
		WINDOW_POSITION_Y = 100,

		// Window Size
		WINDOW_SIZE_X = 800,
		WINDOW_SIZE_Y = 600

	};


	// Define String Constant
	const char* WINDOW_TITLE = "Example";


	// --
	// Define struct
	// --

	// SFPos: float형 좌표계
	struct SFPos
	{
		float x;
		float y;

		SFPos() : x(0.f), y(0.f) {}

		SFPos(const float _x, const float _y) : x(_x), y(_y) {}

		SFPos& operator()(const float _x, const float _y)
		{
			x = _x;
			y = _y;
			return *this;
		}
	};

	// SIPos: int형 좌표계
	struct SIPos
	{
		int x;
		int y;

		SIPos() : x(0), y(0) {}

		SIPos(const int _x, const int _y) : x(_x), y(_y) {}

		SIPos& operator()(const int _x, const int _y)
		{
			x = _x;
			y = _y;
			return *this;
		}

		SIPos operator+(const int _value)
		{
			return SIPos(x + _value, y + _value);
		}

		SIPos operator-(const int _value)
		{
			return SIPos(x - _value, y - _value);
		}
	};

	// SFColor: float형 color
	struct SFColor
	{
		float r;
		float g;
		float b;

		SFColor() : r(0.f), g(0.f), b(0.f) {}

		SFColor(const float _r, const float _g, const float _b) :
			r(_r), g(_g), b(_b)
		{}
	};

	// SRect: SFPos 2, SFColor 1로 구성된 Rect
	struct SRect
	{
		SFPos	pos[2];
		SFColor	color;

		SRect() :
			pos{},
			color{}
		{}

		SRect& operator()(const float _x1, const float _y1, const float _x2, const float _y2)
		{
			pos[0](_x1, _y1);
			pos[1](_x2, _y2);
			return *this;
		}

		SRect& operator()(const SFPos _fir, const SFPos _sec)
		{
			return (*this)(_fir.x, _fir.y, _sec.x, _sec.y);
		}
	};

	struct STimer
	{
		// 타이머 남은시간
		// 타이머의 id
		// 타이머의 종류
		// 타이머의 시작 좌표

		int time;
		int type;
		int id;
		SIPos pos;
		std::list<SRect>::iterator iter;
		
		STimer() :
			time(0),
			type(0),
			id(0),
			pos{},
			iter{}
		{}

	};


	// Define member Variable
	SFColor bg_color;

	std::list<SRect> rects;
	STimer timers[5];
	int now_id;

	std::list<SRect> rects_divide;

public:

	CWindowMgr() :
		bg_color{ 1.f, 1.f, 1.f },
		rects{},
		now_id(100),
		rects_divide{}
	{
		for (int i = 0; i < 5; ++i) {
			SIPos pos(dist_posx(gen), dist_posy(gen));
			SRect rect;
			randomColor(rect.color);
			rect(posWinToGL(pos), posWinToGL(pos.x + 100, pos.y + 100));
			rects.push_back(rect);
		}
	}

		~CWindowMgr() {}
	CWindowMgr(const CWindowMgr& other) = delete;
	CWindowMgr& operator=(const CWindowMgr& other) = delete;


private:

	// --
	// 멤버 함수 
	// --

	// --
	// Utility func
	// -- 

	// Window pos to GL pos
	const SFPos posWinToGL(const int _x, const int _y)
	{
		float ay = static_cast<float>(WINDOW_SIZE_Y - _y);
		float rx = static_cast<float>((_x - (WINDOW_SIZE_X / 2))) / static_cast<float>((WINDOW_SIZE_X / 2));
		float ry = static_cast<float>((ay - (WINDOW_SIZE_Y / 2))) / static_cast<float>((WINDOW_SIZE_Y / 2));
		return SFPos(rx, ry);
	}
	const SFPos posWinToGL(SIPos _ipos)
	{
		return posWinToGL(_ipos.x, _ipos.y);
	}

	// GL Pos to Window Pos
	const SIPos posGLToWin(const float _x, const float _y)
	{
		int rx = (_x * static_cast<float>(WINDOW_SIZE_X / 2)) + static_cast<float>(WINDOW_SIZE_X / 2);
		int ry = (_y * static_cast<float>(WINDOW_SIZE_Y / 2)) + static_cast<float>(WINDOW_SIZE_Y / 2);
		ry = WINDOW_SIZE_Y - ry;
		return SIPos(rx, ry);
	}
	const SIPos posGLToWin(SFPos _fpos)
	{
		return posGLToWin(_fpos.x, _fpos.y);
	}


	// set random color
	void randomColor(float& _r, float& _g, float& _b)
	{
		_r = dist_color(gen);
		_g = dist_color(gen);
		_b = dist_color(gen);
	}
	void randomColor(SFColor& _color)
	{
		randomColor(_color.r, _color.g, _color.b);
	}

public:

	// -- 
	// Callback Function
	// --

	void Display()
	{
		glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		for (std::list<SRect>::reverse_iterator iter = rects.rbegin(); iter != rects.rend(); ++iter) {
			glColor3f(iter->color.r, iter->color.g, iter->color.b);
			glRectf(iter->pos[0].x, iter->pos[0].y, iter->pos[1].x, iter->pos[1].y);
		}

		for (std::list<SRect>::reverse_iterator iter = rects_divide.rbegin(); iter != rects_divide.rend(); ++iter) {
			glColor3f(iter->color.r, iter->color.g, iter->color.b);
			glRectf(iter->pos[0].x, iter->pos[0].y, iter->pos[1].x, iter->pos[1].y);
		}

		glutSwapBuffers();
	}

	void Mouse(const int _button, const int _state, const int _x, const int _y)
	{
		if (GLUT_UP == _state)
		for (std::list<SRect>::iterator iter = rects.begin(); iter != rects.end(); ++iter) {
			SFPos m = posWinToGL(_x, _y);
			if (iter->pos[0].x <= m.x && m.x <= iter->pos[1].x &&
				iter->pos[1].y <= m.y && m.y <= iter->pos[0].y) {

				STimer temp;
				temp.id = now_id;
				temp.pos = posGLToWin(iter->pos[0]);
				temp.pos = temp.pos + 50;
				temp.time = 10;
				temp.type = dist_type(gen);


				SRect divide;
				divide.pos[0] = posWinToGL(temp.pos - 25);
				divide.pos[1] = posWinToGL(temp.pos + 25);
				divide.color = SFColor(iter->color);

				if (temp.type == 3) {
					for (int i = 0; i < 8; ++i) {
						rects_divide.push_back(divide);
						if (i == 0) {
							temp.iter = --rects_divide.end();
						}
					}
				}
				else {
					for (int i = 0; i < 4; ++i) {
						rects_divide.push_back(divide);
						if (i == 0) {
							temp.iter = --rects_divide.end();
						}
					}
				}

				timers[now_id++ - 100] = temp;
				glutTimerFunc(70, cb::Timer, temp.id);
				rects.erase(iter);
				return;
			}
		}
	}

	void moveRectDir(const int dir, std::list<SRect>::iterator& start, int time)
	{
		
		SIPos pos = posGLToWin(start->pos[0]);

		switch (dir)
		{
		case 1:
			pos.x += 20;
			break;
		case 2:
			pos.x += 20;
			pos.y += 20;
			break;
		case 3:
			pos.y += 20;
			break;
		case 4:
			pos.y += 20;
			pos.x -= 20;
			break;
		case 5:
			pos.x -= 20;
			break;
		case 6:
			pos.x -= 20;
			pos.y -= 20;
			break;
		case 7:
			pos.y -= 20;
			break;
		case 8:
			pos.y -= 20;
			pos.x += 20;
			break;
		default:
			break;
		}


		start->pos[0] = posWinToGL(pos);
		start->pos[1] = posWinToGL(pos + 50 - (10 - time) * 5);
	}

	void Timer(const int _value)
	{
		int idx = _value - 100;

		std::list<SRect>::iterator iter = timers[idx].iter;

		switch (timers[idx].type)
		{
		case 1:
			moveRectDir(1, iter, timers[idx].time);
			iter++;
			moveRectDir(3, iter, timers[idx].time);
			iter++;
			moveRectDir(5, iter, timers[idx].time);
			iter++;
			moveRectDir(7, iter, timers[idx].time);
			break;
		case 2:
			moveRectDir(2, iter, timers[idx].time);
			iter++;
			moveRectDir(4, iter, timers[idx].time);
			iter++;
			moveRectDir(6, iter, timers[idx].time);
			iter++;
			moveRectDir(8, iter, timers[idx].time);
			break;
		case 3:
			moveRectDir(1, iter, timers[idx].time);
			iter++;
			moveRectDir(3, iter, timers[idx].time);
			iter++;
			moveRectDir(5, iter, timers[idx].time);
			iter++;
			moveRectDir(7, iter, timers[idx].time);
			iter++;
			moveRectDir(2, iter, timers[idx].time);
			iter++;
			moveRectDir(4, iter, timers[idx].time);
			iter++;
			moveRectDir(6, iter, timers[idx].time);
			iter++;
			moveRectDir(8, iter, timers[idx].time);

			break;
		default:
			break;
		}



		timers[idx].time--;

		if (timers[idx].time != -1) {
			glutTimerFunc(70, cb::Timer, _value);
		}
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

		// set cb func
		glutDisplayFunc(cb::Display);
		glutReshapeFunc(cb::Reshape);
		glutMouseFunc(cb::Mouse);
		glutTimerFunc(10, cb::GameLoop, 1);

		// set position
	}

	// Run Program
	void run()
	{
		glutMainLoop();
	}


};



CWindowMgr Window;


// --
// define callback func
// --


// (CALLBACK) Draw Scene
GLvoid cb::Display()
{
	Window.Display();
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

GLvoid cb::Timer(int value)
{
	Window.Timer(value);
}

// (CALLBACK) Main Loop
// FPS : 100
GLvoid cb::GameLoop(int value)
{

	// Game State update


	// render (Display 함수 호출)
	glutPostRedisplay();

	glutTimerFunc(10, GameLoop, 1);
}


void main(int argc, char** argv)
{
	Window.init(argc, argv);
	Window.run();
}