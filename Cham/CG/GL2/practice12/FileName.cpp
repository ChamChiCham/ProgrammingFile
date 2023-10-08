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

std::uniform_int_distribution<int> rand_x(50, 750);
std::uniform_int_distribution<int> rand_y(50, 550);

std::uniform_int_distribution<int> rand_dir(0, 1);

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


	enum {TRI_SIZE = 25};


	// Define member Variable
	SFColor bg_color;
	CShaderManager ShaderMgr;

	// poligons
	std::vector<SPoligon> poligons;

	// buffer vari
	std::vector<SBuffer> bufs_pol;
	std::list<int> bufs_change;

	std::vector<SIPos> middles;
	std::vector<int> shapes;
	std::vector<int> colors;

	std::vector<int> dir_x;
	std::vector<int> dir_y;

	int mouse = -1;



public:

	CWindowMgr() :
		bg_color{ 1.f, 1.f, 1.f },
		ShaderMgr{},
		poligons{},
		bufs_pol{},
		bufs_change{},
		middles{},
		shapes{},
		colors{},
		dir_x{},
		dir_y{}
	{
		for (int i = 0; i < 15; ++i) {
			dir_x.push_back(0);
			dir_y.push_back(0);
		}
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
			for (const auto& idx : bufs_change) {
				if (idx >= poligons.size())
					continue;
				updatePoligon(idx);
			}
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

	void makeShape(const int i)
	{
		SIPos pos = middles[i];
		SPoligon& poli = poligons[i];
		switch (shapes[i])
		{
			// point to line 그대로.
		case 1:
			poli.vertices.clear();
			poli(pos.x - 5, pos.y - 5, colors[i]);
			poli(pos.x - 5, pos.y + 5, colors[i]);
			poli(pos.x + 5, pos.y - 5, colors[i]);
			poli(pos.x + 5, pos.y + 5, colors[i]);
			bufs_change.push_back(i);
			break;
			// line to triange
		case 2:
			poli.vertices.clear();
			poli(pos.x - 25, pos.y - 5, colors[i]);
			poli(pos.x - 25, pos.y + 5, colors[i]);
			poli(pos.x + 25, pos.y - 5, colors[i]);
			poli(pos.x + 25, pos.y + 5, colors[i]);
			bufs_change.push_back(i);
			break;

			// triangle to square
		case 3:
			poli.vertices.clear();

			poli(pos.x - 25, pos.y + 25, colors[i]);
			poli(pos.x, pos.y - 25, colors[i]);
			poli(pos.x + 25, pos.y + 25, colors[i]);
			bufs_change.push_back(i);
			break;
			// square to pentagon
		case 4:
			poli.vertices.clear();
			poli(pos.x - 25, pos.y - 25, colors[i]);
			poli(pos.x - 25, pos.y + 25, colors[i]);
			poli(pos.x + 25, pos.y - 25, colors[i]);
			poli(pos.x + 25, pos.y + 25, colors[i]);
			bufs_change.push_back(i);
			break;
			// pentagon to point 그대로.
		case 5:
			poli.vertices.clear();
			poli(pos.x - 10, pos.y + 25, colors[i]);
			poli(pos.x - 25, pos.y, colors[i]);
			poli(pos.x + 10, pos.y + 25, colors[i]);
			poli(pos.x, pos.y - 25, colors[i]);
			poli(pos.x + 25, pos.y, colors[i]);
			bufs_change.push_back(i);
			break;
		case 6:
			poli.vertices.clear();
			poli(pos.x - 10, pos.y + 25, colors[i]);
			poli(pos.x - 25, pos.y, colors[i]);
			poli(pos.x + 10, pos.y + 25, colors[i]);
			poli(pos.x - 10, pos.y - 25, colors[i]);
			poli(pos.x + 25, pos.y, colors[i]);
			poli(pos.x + 10, pos.y - 25, colors[i]);

			bufs_change.push_back(i);
		default:
			break;
		}
	}

	const std::pair<SIPos, SIPos> rangeShape(const int _idx)
	{
		SIPos pos = middles[_idx];
		switch (shapes[_idx])
		{
		case 1:
			return std::make_pair(SIPos(pos.x - 5, pos.y - 5), SIPos(pos.x + 5, pos.y + 5));
		case 2:
			return std::make_pair(SIPos(pos.x - 25, pos.y - 5), SIPos(pos.x + 25, pos.y + 5));
		case 3: case 4: case 5: case 6:
			return std::make_pair(SIPos(pos.x - 25, pos.y - 25), SIPos(pos.x + 25, pos.y + 25));
		default:
			return std::make_pair(SIPos(), SIPos());
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
			for (int i = poligons.size() - 1; i >= 0; --i) {
				std::pair<SIPos, SIPos> pos = rangeShape(i);
				if (pos.first.x <= _x && _x <= pos.second.x &&
					pos.first.y <= _y && _y <= pos.second.y) {
					mouse = i;
					break;
				}
			}
		}

		if (_button == GLUT_LEFT_BUTTON && _state == GLUT_UP) {
			if (mouse != -1) {

				std::pair<SIPos, SIPos> ori = rangeShape(mouse);

				for (int i = poligons.size() - 1; i >= 0; --i) {
					
					if (mouse == i)
						continue;

					std::pair<SIPos, SIPos> targ = rangeShape(i);

					if (ori.first.x - (targ.second.x - targ.first.x) < targ.first.x && targ.first.x < ori.second.x &&
						ori.first.y - (targ.second.y - targ.first.y) < targ.first.y && targ.first.y < ori.second.y) {
						
						int new_shape = shapes[mouse] + shapes[i];

						if (new_shape > 6) {
							new_shape -= 6;
						}


						shapes[mouse] = new_shape;
						makeShape(mouse);

						if (dir_x[mouse] == 0) {
							if (rand_dir(gen)) {
								dir_x[mouse] = 1;
								dir_y[mouse] = 1;
							}
							else {
								dir_x[mouse] = -1;
								dir_y[mouse] = -1;
							}
						}

						poligons.erase(poligons.begin() + i);

						middles.erase(middles.begin() + i);

						shapes.erase(shapes.begin() + i);

						colors.erase(colors.begin() + i);

						dir_x.erase(dir_x.begin() + i);
						dir_y.erase(dir_y.begin() + i);


						bufs_pol.erase(bufs_pol.begin() + i);

						for (int j = 0; j < poligons.size(); ++j) {
							bufs_change.push_back(j);
						}

						mouse = -1;
						return;
					}
				}
			}

			mouse = -1;
		}


	}

	void Motion(const int _x, const int _y)
	{
		if (mouse != -1) {
			middles[mouse](_x, _y);
			makeShape(mouse);
		}
	}

	void Keyboard(const unsigned char _key, const int _x, const int _y)
	{

	}

	void updateState()
	{
		for (int i = 0; i < poligons.size(); ++i) {
			if (dir_x[i] == 0)
				continue;
			SIPos& pos = middles[i];
			if (pos.x <= 0 + TRI_SIZE) {
				dir_x[i] = 1;
			}
			else if (pos.x >= 800 - TRI_SIZE) {
				dir_x[i] = -1;
			}

			if (pos.y <= 0 + TRI_SIZE) {
				dir_y[i] = 1;
			}
			else if (pos.y >= 600 - TRI_SIZE) {
				dir_y[i] = -1;
			}


			if (dir_x[i] == -1) {
				pos.x -= 5;
			}
			else if (dir_x[i] == 1) {
				pos.x += 5;
			}

			if (dir_y[i] == -1) {
				pos.y -= 5;
			}
			else if (dir_y[i] == 1) {
				pos.y += 5;
			}
			
			makeShape(i);
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
		glutMotionFunc(cb::Motion);
		glutTimerFunc(10, cb::GameLoop, 1);

		// set position
		for (int i = 0; i < 15; ++i) {
			colors.push_back(rand_color(gen));
			middles.push_back(SIPos(rand_x(gen), rand_y(gen)));
			shapes.push_back((i / 3) + 1);
			poligons.push_back(SPoligon());
			makeShape(i);
		}

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