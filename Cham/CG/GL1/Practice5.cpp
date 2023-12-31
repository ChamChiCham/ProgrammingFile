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
	GLvoid Mouse(int button, int state, int x, int y);
	GLvoid Motion(int x, int y);
	GLvoid Keyboard(unsigned char key, int x, int y);
}

// --
// global vari
// --

// random 
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dist_color(0.f, 1.f);
std::uniform_int_distribution<int> dist_rect(20, 40);
std::uniform_real_distribution<float> dist_posx(0, 775);
std::uniform_real_distribution<float> dist_posy(0, 575);

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

		SIPos& operator+(const int _value)
		{
			x += _value;
			y += _value;
			return *this;
		}

		SIPos& operator-(const int _value)
		{
			x -= _value;
			y -= _value;
			return *this;
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

		bool operator<(const SRect& _other) const
		{
			return pos[0].x < _other.pos[0].x;
		}
	};


	// Define member Variable
	SFColor bg_color;

	std::list<SRect> rects;
	SRect rect_erase;
	bool rect_select;
	bool is_rect_erase;
	int rect_diff;

public:

	CWindowMgr() :
		bg_color{ 1.f, 1.f, 1.f },
		rects{},
		rect_erase{},
		rect_select(false),
		is_rect_erase(false),
		rect_diff(0)
	{
		makeRect();
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
		float ay = WINDOW_SIZE_Y - _y;
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
		int rx = static_cast<int>((_x * static_cast<float>(WINDOW_SIZE_X / 2)) + static_cast<float>(WINDOW_SIZE_X / 2));
		int ry = static_cast<int>((_y * static_cast<float>(WINDOW_SIZE_Y / 2)) + static_cast<float>(WINDOW_SIZE_Y / 2));
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



	void makeRect()
	{
		int limit = dist_rect(gen);
		for (int i = 0; i < limit; ++i) {
			SRect rect;
			int x1 = dist_posx(gen);
			int y1 = dist_posy(gen);
			rect(posWinToGL(SIPos(x1, y1)), posWinToGL(SIPos(x1 + 25, y1 + 25)));
			randomColor(rect.color);
			rects.push_back(rect);
		}
	}

	void createRect(const int _x, const int _y)
	{
		rect_erase(posWinToGL(SIPos(_x, _y) - 25), posWinToGL(SIPos(_x, _y) + 25));
		rect_erase.color = SFColor(0.f, 0.f, 0.f);
	}

public:

	void updateGameStatus()
	{
		if (!is_rect_erase)
			return;


		for (std::list<SRect>::iterator iter = rects.begin(); iter != rects.end(); ++iter) {
			if ((rect_erase.pos[0].x <= iter->pos[0].x && iter->pos[0].x <= rect_erase.pos[1].x ||
				rect_erase.pos[0].x <= iter->pos[1].x && iter->pos[1].x <= rect_erase.pos[1].x) &&
				(rect_erase.pos[1].y <= iter->pos[1].y && iter->pos[1].y <= rect_erase.pos[0].y ||
					rect_erase.pos[1].y <= iter->pos[0].y && iter->pos[0].y <= rect_erase.pos[0].y)) {
				rect_diff += 5;
				rect_erase.color = iter->color;
				rects.erase(iter);
				break;
			}
		}
	}

	// -- 
	// Callback Function
	// --

	void Display()
	{
		glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		for (const auto& rect : rects) {
			glColor3f(rect.color.r, rect.color.g, rect.color.b);
			glRectf(rect.pos[0].x, rect.pos[0].y, rect.pos[1].x, rect.pos[1].y);
		}

		if (is_rect_erase) {
			glColor3f(rect_erase.color.r, rect_erase.color.g, rect_erase.color.b);
			glRectf(rect_erase.pos[0].x, rect_erase.pos[0].y, rect_erase.pos[1].x, rect_erase.pos[1].y);
		}

		glutSwapBuffers();
	}

	void Mouse(const int _button, const int _state, const int _x, const int _y)
	{
		
		if (GLUT_LEFT_BUTTON == _button && GLUT_DOWN == _state) {
			createRect(_x, _y);
			is_rect_erase = true;
			rect_select = true;
		}

		if (GLUT_LEFT_BUTTON == _button && GLUT_UP == _state) {
			rect_diff = 0;
			is_rect_erase = false;
			rect_select = false;
		}


	}

	void Motion(int x, int y)
	{
		if (rect_select == false)
			return;

		rect_erase(posWinToGL(x - 25 - rect_diff, y - 25 - rect_diff), posWinToGL(x + 25 + rect_diff, y + 25 + rect_diff));


	}

	void Keyboard(unsigned char key)
	{
		if ('r' == key) {
			rects.clear();
			rect_diff = 0;
			is_rect_erase = false;
			makeRect();
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
		glutMotionFunc(cb::Motion);
		glutKeyboardFunc(cb::Keyboard);
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

GLvoid cb::Motion(int x, int y)
{
	Window.Motion(x, y);
}

GLvoid cb::Keyboard(unsigned char key, int x, int y)
{
	Window.Keyboard(key);
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