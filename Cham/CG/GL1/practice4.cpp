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
	GLvoid GameLoop(int value);
	GLvoid TimerZigzag(int value);
	GLvoid Mouse(int button, int state, int x, int y);
	GLvoid Keyboard(unsigned char key, int x, int y);
}

// --
// global vari
// --

// random 
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dist_color(0.f, 1.f);
std::uniform_int_distribution<int> dist_dir(1, 4);

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
		WINDOW_SIZE_Y = 600,

		RECT_STATUS_A = 0x1,
		RECT_STATUS_I = 0x2,
		RECT_STATUS_C = 0x4,
		RECT_STATUS_O = 0x8,

		RECT_DIR_N = 0,
		RECT_DIR_DS = 1,
		RECT_DIR_AS = 2,
		RECT_DIR_DW = 3,
		RECT_DIR_AW = 4,

		RECT_SIZE = 100

	};


	// Define String Constant
	const char* WINDOW_TITLE = "Example2";


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
			int rx = 0, ry = 0;
			rx = x + _value;
			ry = y + _value;
			return SIPos(rx, ry);
		}

		SIPos operator-(const int _value)
		{
			int rx = 0, ry = 0;
			rx = x - _value;
			ry = y - _value;
			return SIPos(rx, ry);
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


	// Define member Variable
	SFColor bg_color;

	SRect rects[5];
	SIPos pos_start[5];
	int pos_dif;
	int rect_count;
	int rect_status;
	int rect_dir[5];

public:

	CWindowMgr() :
		bg_color{ 0.2f, 0.2f, 0.2f },
		pos_start{},
		pos_dif{},
		rect_count(0),
		rect_status(0),
		rect_dir{}
	{}

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

	// --
	// process func
	// --

	void createRect(const int _x, const int _y)
	{
		rects[rect_count](posWinToGL(SIPos(_x, _y) - 25), posWinToGL(SIPos(_x, _y) + 25));
		randomColor(rects[rect_count].color);
		pos_start[rect_count] = SIPos(_x, _y);
		rect_dir[rect_count++] = dist_dir(gen);
	}

	void changeRectDir(const int _idx)
	{
		// Left check
		if (rects[_idx].pos[1].x > 1.f) {
			if (RECT_DIR_DS == rect_dir[_idx])
				rect_dir[_idx] = RECT_DIR_AS;
			else if (RECT_DIR_DW == rect_dir[_idx])
				rect_dir[_idx] = RECT_DIR_AW;
		}
		// right
		if (rects[_idx].pos[0].x < -1.f) {
			if (RECT_DIR_AW == rect_dir[_idx])
				rect_dir[_idx] = RECT_DIR_DW;
			else if (RECT_DIR_AS == rect_dir[_idx])
				rect_dir[_idx] = RECT_DIR_DS;
		}
		// down
		if (rects[_idx].pos[0].y > 1.f) {
			if (RECT_DIR_AW == rect_dir[_idx])
				rect_dir[_idx] = RECT_DIR_AS;
			else if (RECT_DIR_DW == rect_dir[_idx])
				rect_dir[_idx] = RECT_DIR_DS;
		}
		// up
		if (rects[_idx].pos[1].y < -1.f) {
			if (RECT_DIR_AS == rect_dir[_idx])
				rect_dir[_idx] = RECT_DIR_AW;
			else if (RECT_DIR_DS == rect_dir[_idx])
				rect_dir[_idx] = RECT_DIR_DW;
		}
	}

	void moveRectDiag()
	{
		for (int i = 0; i < rect_count; ++i) {
			changeRectDir(i);
			SIPos fir = posGLToWin(rects[i].pos[0]);
			int gap = 5;
			switch (rect_dir[i])
			{
			case RECT_DIR_DS:
				fir.x += gap;
				fir.y += gap;
				break;

			case RECT_DIR_AS:
				fir.x -= gap;
				fir.y += gap;
				break;

			case RECT_DIR_DW:
				fir.x += gap;
				fir.y -= gap;
				break;

			case RECT_DIR_AW:
				fir.x -= gap;
				fir.y -= gap;
				break;
			default:
				break;
			}
			rects[i](posWinToGL(fir), posWinToGL(fir + 50));
		}
	}

	void setStatus(const int _value)
	{
		if (rect_status & _value)
			rect_status &= ~_value;
		else
			rect_status |= _value;
	}

public:

	// --
	// Main func
	// --

	void updateGameState()
	{
		if (rect_status & RECT_STATUS_A)
			moveRectDiag();

		if (pos_dif == 25)
			pos_dif = -25;
		else
			pos_dif += 1;

	}

	// -- 
	// Callback Function
	// --

	void Display()
	{
		glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		int dif = 0;
		if (rect_status & RECT_STATUS_C)
			dif = pos_dif;

		for (int i = 0; i < rect_count; ++i) {
			SIPos fir = posGLToWin(rects[i].pos[0]);
			SIPos sec = posGLToWin(rects[i].pos[1]);

			fir.x += dif; sec.x -= dif; fir.y -= dif; sec.y += dif;

			SFPos ffir = posWinToGL(fir);
			SFPos fsec = posWinToGL(sec);

			glColor3f(rects[i].color.r, rects[i].color.g, rects[i].color.b);
			glRectf(ffir.x, ffir.y, fsec.x, fsec.y);
		}

		glutSwapBuffers();
	}

	void Mouse(const int _button, const int _state, const int _x, const int _y)
	{
		if (GLUT_LEFT_BUTTON == _button && GLUT_UP == _state && rect_count != 5) {
			createRect(_x, _y);
		}
	}

	void Keyboard(const unsigned char key)
	{
		switch (key)
		{
		case 'a':
			// activate status A
			setStatus(RECT_STATUS_A);
			break;
		case 'i':
			setStatus(RECT_STATUS_I);
			if ((rect_status & RECT_STATUS_A) != (rect_status & RECT_STATUS_I))
				setStatus(RECT_STATUS_A);
			break;

		case 's':
			rect_status = 0;
			break;

		case 'm':
			for (int i = 0; i < rect_count; ++i) {
				rects[i](posWinToGL(pos_start[i]), posWinToGL(pos_start[i] + 50));
			}
			break;

		case 'c':
			setStatus(RECT_STATUS_C);
			break;

		case 'o':
			setStatus(RECT_STATUS_O);
			break;

		case 'r':
			rect_status = 0;
			rect_count = 0;

		default:
			break;
		}
	}

	void TimerZigzag(const int value)
	{
		if (rect_status & RECT_STATUS_O) {
			for (int i = 0; i < rect_count; ++i) {
				randomColor(rects[i].color);
			}
		}

		if ((RECT_STATUS_I & rect_status)) {
			for (int i = 0; i < rect_count; ++i) {
				switch (rect_dir[i])
				{
				case RECT_DIR_DS:
					rect_dir[i] = RECT_DIR_DW;
					break;

				case RECT_DIR_AS:
					rect_dir[i] = RECT_DIR_AW;
					break;

				case RECT_DIR_DW:
					rect_dir[i] = RECT_DIR_DS;
					break;

				case RECT_DIR_AW:
					rect_dir[i] = RECT_DIR_AS;
					break;
				default:
					break;
				}
			}
		}
		glutTimerFunc(150, cb::TimerZigzag, 2);
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
		glutKeyboardFunc(cb::Keyboard);
		glutTimerFunc(20, cb::GameLoop, 1);
		glutTimerFunc(150, cb::TimerZigzag, 2);

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

GLvoid cb::Keyboard(unsigned char key, int x, int y)
{
	Window.Keyboard(key);
}

GLvoid cb::TimerZigzag(int value)
{
	Window.TimerZigzag(value);
}

// (CALLBACK) Main Loop
// FPS : 100
GLvoid cb::GameLoop(int value)
{

	// Game State update
	Window.updateGameState();

	// render (Display 함수 호출)
	glutPostRedisplay();

	// call timer func
	glutTimerFunc(20, GameLoop, 1);
}


void main(int argc, char** argv)
{
	Window.init(argc, argv);
	Window.run();
}