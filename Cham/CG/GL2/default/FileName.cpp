#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <iostream>
#include <vector>
#include <list>

#include "Struct.h"
#include "Define.h"
#include "CShaderManager.h"
#include "Utility.h"



namespace cb
{
	GLvoid Display();
	GLvoid Reshape(int w, int h);
	GLvoid GameLoop(int value);
	GLvoid Mouse(int button, int state, int x, int y);
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

public:

	CWindowMgr() :
		bg_color{ 1.f, 1.f, 1.f },
		ShaderMgr{},
		poligons{},
		bufs_pol{},
		bufs_change{}
	{
		SPoligon poligon;
		poligon.vertices.push_back(SVertex(400, 400, 0xFF00FF));
		poligon.vertices.push_back(SVertex(200, 400, 0xFF00FF));
		poligon.vertices.push_back(SVertex(400, 200, 0xFF00FF));
		poligon.vertices.push_back(SVertex(200, 200, 0xFF00FF));
		poligons.push_back(poligon);
		bufs_change.push_back(0);
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

		
		bufs_pol.push_back(buf);
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
		glDrawElements(GL_TRIANGLES, (bufs_pol[_idx].size - 2) * 3, GL_UNSIGNED_INT, 0);
	}


	// --
	// process func
	// --



	// -- 
	// Callback Function
	// --

	void Display()
	{
		glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(ShaderMgr.program);
		drawPoligon(0);

		glutSwapBuffers();
	}

	void Mouse(const int _button, const int _state, const int _x, const int _y)
	{
		
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

// (CALLBACK) Main Loop
// FPS : 100
GLvoid cb::GameLoop(int value)
{

	// Game State update


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