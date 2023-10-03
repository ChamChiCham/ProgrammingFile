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


	SIPos middles[5];
	int colors[5];
	int	shapes[5];
	int level;
	int mode;

public:

	CWindowMgr() :
		bg_color{ 1.f, 1.f, 1.f },
		ShaderMgr{},
		poligons{},
		bufs_pol{},
		bufs_change{},
		middles{},
		colors{},
		shapes{},
		level{1},
		mode{}
	{
		middles[0](400, 300);
		middles[1](200, 150);
		middles[2](600, 150);
		middles[3](200, 450);
		middles[4](600, 450);
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
		glDrawElements(GL_TRIANGLES, (bufs_pol[_idx].size - 2) * 3, GL_UNSIGNED_INT, 0);
	}


	// --
	// process func
	// --

	void initShape()
	{
		level = 0;
		poligons.clear();
		bufs_pol.clear();
		for (auto& color : colors) {
			color = rand_color(gen);
		}

		shapes[0] = 0;
		for (int i = 1; i < 5; ++i) {
			shapes[i] = i + 1;
		}

		makeTriangle(0, 0, 0, 0, 0, 0, 0x00000);
		makeTriangle(500, 250, 700, 250, 600, 50, colors[1]);
		makeRect(100, 150 - 1, 300, 150 + 1, colors[2]);
		makeRect(100, 350, 300, 550, colors[3]);


		SPoligon poli;
		poli(560, 550, colors[4]);
		poli(500, 450, colors[4]);
		poli(640, 550, colors[4]);
		poli(600, 350, colors[4]);
		poli(700, 450, colors[4]);
		poligons.push_back(poli);
		bufs_change.push_back(poligons.size() - 1);


	}

	void initShapeAni(int i)
	{
		
			SIPos pos = middles[i];
			SPoligon& poli = poligons[i];
			switch (shapes[i])
			{
				// point to line 그대로.
			case 1:
				poli.vertices.clear();
				poli(pos.x, pos.y + 1, colors[i]);
				poli(pos.x, pos.y - 1, colors[i]);
				poli(pos.x, pos.y + 1, colors[i]);
				poli(pos.x, pos.y - 1, colors[i]);
				bufs_change.push_back(i);
				break;
				// line to triange
			case 2:
				poli.vertices.clear();
				poli(pos.x - 100, pos.y, colors[i]);
				poli(pos.x, pos.y, colors[i]);
				poli(pos.x + 100, pos.y, colors[i]);
				bufs_change.push_back(i);
				break;

				// triangle to square
			case 3:
				poli.vertices.clear();
				poli(pos.x - 100, pos.y + 100, colors[i]);
				poli(pos.x, pos.y - 100, colors[i]);
				poli(pos.x + 100, pos.y + 100, colors[i]);
				poli(pos.x, pos.y - 100, colors[i]);
				bufs_change.push_back(i);
				break;
				// square to pentagon
			case 4:
				poli.vertices.clear();
				poli(pos.x - 100, pos.y + 100, colors[i]);
				poli(pos.x - 100, pos.y - 100, colors[i]);
				poli(pos.x + 100, pos.y + 100, colors[i]);
				poli(pos.x, pos.y - 100, colors[i]);
				poli(pos.x + 100, pos.y - 100, colors[i]);
				bufs_change.push_back(i);
				break;
				// pentagon to point 그대로.
			case 5:
				poli.vertices.clear();
				poli(pos.x - 40, pos.y + 100, colors[i]);
				poli(pos.x - 100, pos.y, colors[i]);
				poli(pos.x + 40, pos.y + 100, colors[i]);
				poli(pos.x, pos.y - 100, colors[i]);
				poli(pos.x + 100, pos.y, colors[i]);
				bufs_change.push_back(i);
				break;
			default:
				break;
			}

			updateBuffer();
		
	}

	void animateShape(int _idx)
	{
		switch (shapes[_idx])
		{
		case 1:
			poligons[_idx].vertices[0].pos.x -= 5;
			poligons[_idx].vertices[1].pos.x -= 5;
			poligons[_idx].vertices[2].pos.x += 5;
			poligons[_idx].vertices[3].pos.x += 5;
			break;
		case 2:
			poligons[_idx].vertices[0].pos.y += 5;
			poligons[_idx].vertices[1].pos.y -= 5;
			poligons[_idx].vertices[2].pos.y += 5;
			break;
		case 3:
			poligons[_idx].vertices[1].pos.x -= 5;
			poligons[_idx].vertices[3].pos.x += 5;
			break;
		case 4:
			poligons[_idx].vertices[0].pos.x += 3;
			poligons[_idx].vertices[1].pos.y += 5;
			poligons[_idx].vertices[2].pos.x -= 3;
			poligons[_idx].vertices[4].pos.y += 5;
			break;
		case 5:
			poligons[_idx].vertices[0].pos.x += 2;
			poligons[_idx].vertices[0].pos.y -= 5;

			poligons[_idx].vertices[1].pos.x += 5;

			poligons[_idx].vertices[2].pos.x -= 2;
			poligons[_idx].vertices[2].pos.y -= 5;

			poligons[_idx].vertices[3].pos.y += 5;

			poligons[_idx].vertices[4].pos.x -= 5;
			break;
		default:
			break;
		}
		
		bufs_change.push_back(_idx);

	}

	void updateState()
	{
		if (level == 0) {
			if (mode == 0) {
				level = 1;
			}
			return;
		}
		
		if (level == 1) {
			if (mode == 0) {
				for (int i = 1; i < 5; ++i)
					initShapeAni(i);
			}
		}

		if (mode == 0) {
			for (int i = 1; i < 5; ++i)
				animateShape(i);
		}
		else
			animateShape(0);

		level++;

		if (level == 21) {

			if (mode == 0) {
				for (int i = 1; i < 5; ++i) {
					shapes[i]++;
					if (shapes[i] == 6) {
						shapes[i] = 1;
					}
				}
				level = 1;
			}
			else {
				if (shapes[0] == 5) {
					shapes[0] = 1;
					poligons[0].vertices[0].pos = SIPos(398, 302);
					poligons[0].vertices[1].pos = SIPos(398, 298);
					poligons[0].vertices[2].pos = SIPos(402, 302);
					poligons[0].vertices[3].pos = SIPos(402, 298);
					bufs_change.push_back(0);
				}
				level = 0;
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
		case 'a':
			mode = 0;
			initShape();
			break;
		case 'l':
			level = 1;
			mode = 1;
			poligons.clear();
			bufs_pol.clear();
			shapes[0] = 2;
			poligons.push_back(SPoligon());
			initShapeAni(0);
			break;
		case 't':
			level = 1;
			mode = 1;
			poligons.clear();
			bufs_pol.clear();
			shapes[0] = 3;
			poligons.push_back(SPoligon());
			initShapeAni(0);
			break;
		case 'r':
			level = 1;
			mode = 1;
			poligons.clear();
			bufs_pol.clear();
			shapes[0] =4;
			poligons.push_back(SPoligon());
			initShapeAni(0);
			break;
		case 'p':
			level = 1;
			mode = 1;
			poligons.clear();
			bufs_pol.clear();
			shapes[0] = 5;
			poligons.push_back(SPoligon());
			initShapeAni(0);
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

	
		initShape();
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

	glutTimerFunc(30, GameLoop, 1);
}



void main(int argc, char** argv)
{
	Window.init(argc, argv);
	Window.run();
}