#include <gl/glew.h>              
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <iostream>
#include <utility>
#include <random>

// --
// declare callback func
// --

namespace cb
{
	GLvoid Display();
	GLvoid Reshape(int w, int h);
	GLvoid timer_main_loop(int value);
	GLvoid Mouse(int button, int state, int x, int y);
}

// --
// global vari
// --

// random 
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> randdist_color(0.f, 1.f);


// --
// define class
// --

class CWindowMgr
{
private:

	// Define Int Constant
	enum {

		// Display Mode
		WINDOW_DISPLAYMODE = GLUT_DOUBLE | GLUT_RGBA,

		// Window Position
		WINDOW_POSITION_X = 100,
		WINDOW_POSITION_Y = 100,

		// Window Size
		WINDOW_SIZE_X = 800,
		WINDOW_SIZE_Y = 600,

	};

	// Define String Constant
	const char* WINDOW_TITLE = "Example2";

	// --
	// define Struct
	// --

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

		void set_pos(const float _x, const float _y) { x = _x; y = _y; }
	};

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
	};

	// Define member Variable
	SRect rects[4];

	SFColor bg_color;


public:

	CWindowMgr() :
		rects{},
		bg_color{0.f, 0.f, 1.f}
	{
		rects[0](-1.f, +1.f, +0.f, +0.f);
		rects[1](+0.f, +1.f, +1.f, +0.f);
		rects[2](-1.f, +0.f, +0.f, -1.f);
		rects[3](+0.f, +0.f, +1.f, -1.f);

		for (auto& rect : rects)
			random_color(rect.color);
	}

	~CWindowMgr() {}
	CWindowMgr(const CWindowMgr& other) = delete;
	CWindowMgr& operator=(const CWindowMgr& other) = delete;


private:

	// --
	// 멤버 함수 
	// --
	
	// Window pos to GL pos
	const std::pair<float, float> pos_win_to_gl(const int _x, const int _y)
	{
		float ay = (int)abs(WINDOW_SIZE_Y - _y);
		float rx = float((_x - (WINDOW_SIZE_X / 2))) / float((WINDOW_SIZE_X / 2));
		float ry = float((ay - (WINDOW_SIZE_Y / 2))) / float((WINDOW_SIZE_Y / 2));
		return std::make_pair(rx, ry);
	}

	// GL Pos to Window Pos
	const std::pair<int, int> pos_gl_to_win(const float _x, const float _y)
	{
		int rx = (_x * float(WINDOW_SIZE_X / 2)) + float(WINDOW_SIZE_X / 2);
		int ry = (_y * float(WINDOW_SIZE_Y / 2)) + float(WINDOW_SIZE_Y / 2);
		ry = (int)abs(float(WINDOW_SIZE_Y) - ry);
		return std::make_pair(rx, ry);
	}



	// set random color
	void random_color(float& _r, float& _g, float& _b)
	{
		_r = randdist_color(gen);
		_g = randdist_color(gen);
		_b = randdist_color(gen);
	}
	void random_color(SFColor& _color)
	{
		random_color(_color.r, _color.g, _color.b);
	}



	// resize rect
	void rect_size_up(SRect& _rect)
	{
		float dx = (_rect.pos[1].x - _rect.pos[0].x) / 2;
		float dy = (_rect.pos[1].y - _rect.pos[0].y) / 2;

		_rect.pos[0].x -= dx;
		_rect.pos[1].x += dx;
		_rect.pos[0].y -= dy;
		_rect.pos[1].y += dy;
	}

	void rect_size_down(SRect& _rect)
	{
		float dx = (_rect.pos[1].x - _rect.pos[0].x) / 4;
		float dy = (_rect.pos[1].y - _rect.pos[0].y) / 4;

		_rect.pos[0].x += dx;
		_rect.pos[1].x -= dx;
		_rect.pos[0].y += dy;
		_rect.pos[1].y -= dy;
	}

public:

	// Callback Function
	void Display()
	{
		glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		for (auto& rect : rects) {
			glColor3f(rect.color.r, rect.color.g, rect.color.b);
			glRectf(rect.pos[0].x, rect.pos[0].y, rect.pos[1].x, rect.pos[1].y);
		}

		glutSwapBuffers();
	}

	void Mouse(const int _button, const int _state, const int _x, const int _y)
	{

		
		if (GLUT_LEFT_BUTTON == _button && GLUT_DOWN == _state) {
			for (auto& rect : rects) {
				std::pair<int, int> pos1 = pos_gl_to_win(rect.pos[0].x, rect.pos[0].y);
				std::pair<int, int> pos2 = pos_gl_to_win(rect.pos[1].x, rect.pos[1].y);
				if (pos1.first <= _x && _x <= pos2.first &&
					pos1.second <= _y && _y <= pos2.second) {
					random_color(rect.color);
					return;
				}
			}
			random_color(bg_color);
		}

		else if (GLUT_RIGHT_BUTTON == _button && GLUT_DOWN == _state) {
			for (auto& rect : rects) {
				std::pair<int, int> pos1 = pos_gl_to_win(rect.pos[0].x, rect.pos[0].y);
				std::pair<int, int> pos2 = pos_gl_to_win(rect.pos[1].x, rect.pos[1].y);
				if (pos1.first <= _x && _x <= pos2.first &&
					pos1.second <= _y && _y <= pos2.second) {
					rect_size_down(rect);
					return;
				}
			}

			int select = 0;
			if (_x < WINDOW_SIZE_X / 2) {
				if (_y > WINDOW_SIZE_Y / 2)
					select = 2;
			}
			else {
				if (_y < WINDOW_SIZE_Y / 2)
					select = 1;
				else
					select = 3;
			}

			rect_size_up(rects[select]);
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
		glutTimerFunc(10, cb::timer_main_loop, 1);

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

GLvoid cb::Mouse(int button, int state, int x, int y)
{
	Window.Mouse(button, state, x, y);
}

// --
// (CALLBACK) Main Loop
// FPS : 100
// --
GLvoid cb::timer_main_loop(int value)
{

	// Game State update


	// render (Display 함수 호출)
	glutPostRedisplay();

	glutTimerFunc(10, timer_main_loop, 1);
}

void main(int argc, char** argv)
{
	Window.init(argc, argv);
	Window.run();
}
