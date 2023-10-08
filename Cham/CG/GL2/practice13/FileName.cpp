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

namespace cb
{
	GLvoid Display();
	GLvoid Reshape(int w, int h);
	GLvoid GameLoop(int value);
	GLvoid Mouse(int button, int state, int x, int y);
	GLvoid Keyboard(unsigned char key, int x, int y);
	GLvoid Motion(int x, int y);
}


class CWindowMgr
{
private:

	// Define member Variable
	SFColor bg_color;
	CShaderManager ShaderMgr;

	// poligons
	std::vector<SPoligon> poligons;

	// buffer vari
	std::vector<SBuffer> bufs_pol;
	std::list<int> bufs_change;



	int mouse = -1;
	int move = 0;

	SIPos start;


public:

	CWindowMgr() :
		bg_color{ 1.f, 1.f, 1.f },
		ShaderMgr{},
		poligons{},
		bufs_pol{},
		bufs_change{}
	{
		SPoligon poli;
		poli(200, 450, 0x000000);
		poli(200, 150, 0x000000);
		poli(600, 150, 0x000000);
		poli(600, 450, 0x000000);
		poligons.push_back(poli);
		bufs_change.push_back(poligons.size() - 1);

		makeRect(0, 299, 800, 301, 0x000000);
		makeRect(399, 0, 401, 600, 0x000000);
	}

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
		glDrawElements(GL_LINE_LOOP, (bufs_pol[_idx].size - 2) * 3, GL_UNSIGNED_INT, 0);
	}


	// --
	// process func
	// --

	void updateState()
	{
		if (mouse != -1 || move == 1) {
			bufs_change.push_back(0);
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
		if (_button == GLUT_LEFT_BUTTON && _state == GLUT_DOWN) {
			for (int i = 0; i < 4; ++i) {
				SIPos pos = poligons[0].vertices[i].pos;

				if (pos.x - 50 < _x && _x < pos.x + 50 &&
					pos.y - 50 < _y && _y < pos.y + 50) {
					mouse = i;
					return;
				}
			}


			SIPos fpos = poligons[0].vertices[0].pos;
			SIPos spos = poligons[0].vertices[0].pos;

			for (int i = 1; i < 4; ++i) {
				SIPos pos = poligons[0].vertices[i].pos;
				if (fpos.x > pos.x)
					fpos.x = pos.x;
				if (spos.x < pos.x)
					spos.x = pos.x;
				if (fpos.y > pos.y)
					fpos.y = pos.y;
				if (spos.y < pos.y)
					spos.y = pos.y;
			}

			if (fpos.x < _x && _x < spos.x &&
				fpos.y < _y && _y < spos.y) {
				move = 1;
				start(_x, _y);
			}

		}

		if (_button == GLUT_LEFT_BUTTON && _state == GLUT_UP) {
			mouse = -1;
			move = 0;
		}
	}

	void Motion(const int _x, const int _y)
	{
		if (mouse != -1) {
			poligons[0].vertices[mouse].pos(_x, _y);
		}

		if (move) {
			SIPos diff = SIPos(_x - start.x, _y - start.y);
			poligons[0].vertices[0].pos.x = poligons[0].vertices[0].pos.x + diff.x;
			poligons[0].vertices[0].pos.y = poligons[0].vertices[0].pos.y + diff.y;
			poligons[0].vertices[1].pos.x = poligons[0].vertices[1].pos.x + diff.x;
			poligons[0].vertices[1].pos.y = poligons[0].vertices[1].pos.y + diff.y;
			poligons[0].vertices[2].pos.x = poligons[0].vertices[2].pos.x + diff.x;
			poligons[0].vertices[2].pos.y = poligons[0].vertices[2].pos.y + diff.y;
			poligons[0].vertices[3].pos.x = poligons[0].vertices[3].pos.x + diff.x;
			poligons[0].vertices[3].pos.y = poligons[0].vertices[3].pos.y + diff.y;

			start.x = _x;
			start.y = _y;
		}
	}

	void Keyboard(const unsigned char _key, const int _x, const int _y)
	{

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
		bufs_change.push_back(poligons.size() - 1);
	}

	void makeTriangle(const int _x1, const int _y1, const int _x2, const int _y2, const int _x3, const int _y3, const int _color)
	{
		SPoligon poli;
		poli(_x1, _y1, _color);
		poli(_x2, _y2, _color);
		poli(_x3, _y3, _color);
		poligons.push_back(poli);
		bufs_change.push_back(poligons.size() - 1);
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
		glutMotionFunc(cb::Motion);

		// set position
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

GLvoid cb::Motion(int x, int y)
{
	Window.Motion(x, y);
}

// (CALLBACK) Main Loop
// FPS : 100
GLvoid cb::GameLoop(int value)
{

	// Game State update
	Window.updateState();

	// buffer update
	Window.updateBuffer();

	// render (Display 함수 호출)
	glutPostRedisplay();

	glutTimerFunc(10, GameLoop, 1);
}



void main(int argc, char** argv)
{
	Window.init(argc, argv);
	Window.run();
}