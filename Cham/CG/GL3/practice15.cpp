#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <random>

#include "CShaderManager.h"
#include "Define.h"
#include "Struct.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> rand_dice(0, 5);
std::uniform_int_distribution<int> rand_tetra(0, 3);

// --
// declare callback function 
// --
namespace cb
{
	GLvoid Display();
	GLvoid Reshape(int w, int h);
	GLvoid GameLoop(int value);
	GLvoid Mouse(int button, int state, int x, int y);
	GLvoid Keyboard(unsigned char key, int x, int y);
	GLvoid Motion(int x, int y);
}

class CShape
{
private:

	SBuffer buffer;

	glm::mat4 mat[3];

public:

	SShapeData* data;
	std::vector<glm::vec3> colors;
	

public:

	CShape() : data{}, mat{ glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f) } {}

	void updateBuffer()
	{
		if (data->coords.size() != colors.size() * 3) {
			std::cerr << "not sized between colors and coords" << std::endl;
			return;
		}


		if (buffer.VAO != 0) {
			glDeleteVertexArrays(1, &buffer.VAO);
			glDeleteBuffers(2, buffer.VBO);
			glDeleteBuffers(1, &buffer.EBO);
		}


		std::vector<GLfloat> VBO1;

		for (int i = 0; i < colors.size(); ++i) {
			VBO1.push_back(colors[i].x);
			VBO1.push_back(colors[i].y);
			VBO1.push_back(colors[i].z);
		}

		glGenBuffers(2, buffer.VBO);

		glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, data->coords.size() * sizeof(GLfloat), data->coords.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, VBO1.size() * sizeof(GLfloat), VBO1.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &buffer.EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->indices.size() * sizeof(unsigned int), data->indices.data(), GL_STATIC_DRAW);

		glGenVertexArrays(1, &buffer.VAO);
		glBindVertexArray(buffer.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.EBO);
	}

	void setData(SShapeData& _data)
	{
		data = &_data;
	}

	void draw(const unsigned int _program, const unsigned int _mode) const
	{
		glm::mat4 MR = mat[MAT_T] * mat[MAT_R] * mat[MAT_S];

		glUseProgram(_program);

		// uniform matrix 
		unsigned int transformLocation = glGetUniformLocation(_program, "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(MR));
		glBindVertexArray(buffer.VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.EBO);
		glDrawElements(_mode, data->indices.size(), GL_UNSIGNED_INT, 0);
	}

	void scale(const glm::vec3& _vec)
	{
		mat[MAT_S] = glm::scale(mat[MAT_S], _vec);
	}

	void rotate(const float _deg, const glm::vec3& _vec)
	{
		mat[MAT_R] = glm::rotate(mat[MAT_R], glm::radians(_deg), _vec);
	}

	void translate(const glm::vec3& _vec)
	{
		mat[MAT_T] = glm::translate(mat[MAT_T], _vec);
	}

	void clearMatrix()
	{
		for (auto& matrix : mat)
			matrix = glm::mat4(1.0f);
	}
};

class CWindowMgr
{
private:

	// --
	// declare member Variable
	// --

	// Manager
	CShaderMgr ShaderMgr;

	// basic data

	// bg color
	glm::vec3 bg_color = glm::vec3(1.f, 1.f, 1.f);

	// shape data list
	std::vector<SShapeData> shape_data;



	// --
	// process member Variable
	// --

	std::vector<CShape> shapes;

	int rotate_x = 0;
	int rotate_y = 0;
	int solid = 1;
	int depth = 1;


public:

	// --
	// Basic func
	// --

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


		// set cb func
		glutDisplayFunc(cb::Display);
		glutReshapeFunc(cb::Reshape);
		glutMouseFunc(cb::Mouse);
		glutKeyboardFunc(cb::Keyboard);
		glutTimerFunc(10, cb::GameLoop, 1);
		glutMotionFunc(cb::Motion);



		// create shape data
		shape_data.push_back(SShapeData());
		shape_data[SHAPE_DICE].coords =
		{
			1.f,	1.f,	1.f,
			-1.f,	1.f,	1.f,
			1.f,	-1.f,	1.f,
			- 1.f,	-1.f,	1.f,
			1.f,	1.f,	-1.f,
			-1.f,	1.f,	-1.f,
			1.f,	-1.f,	-1.f,
			-1.f,	-1.f,	-1.f
		};

		shape_data[SHAPE_DICE].indices =
		{
			0, 3, 1,
			0, 2, 3,
			0, 4, 2,
			2, 4, 6,
			2, 6, 3,
			3, 6, 7,
			4, 7, 6,
			4, 5, 7,
			7, 5, 3,
			1, 3, 5,
			4, 1, 5,
			0, 1, 4
		};


		shape_data.push_back(SShapeData());
		shape_data[SHAPE_LINE].coords =
		{
			1.f,	0.002f,		0.002f,
			-1.f,	0.002f,		0.002f,
			1.f,	-0.002f,	0.002f,
			-1.f,	-0.002f,	0.002f,
			1.f,	0.002f,		-0.002f,
			-1.f,	0.002f,		-0.002f,
			1.f,	-0.002f,	-0.002f,
			-1.f,	-0.002f,	-0.002f
		};

		shape_data[SHAPE_LINE].indices =
		{
			0, 3, 1,
			0, 2, 3,
			0, 4, 2,
			2, 4, 6,
			2, 6, 3,
			3, 6, 7,
			4, 7, 6,
			4, 5, 7,
			7, 5, 3,
			1, 3, 5,
			4, 1, 5,
			0, 1, 4
		};

		shape_data.push_back(SShapeData());
		shape_data[SHAPE_SQUARE_PRAMID].coords =
		{
			0.f,	1.f,	0.f,
			1.f,	-1.f,	1.f,
			-1.f,	-1.f,	1.f,
			1.f,	-1.f,	-1.f,
			-1.f,	-1.f,	-1.f,
		};

		shape_data[SHAPE_SQUARE_PRAMID].indices =
		{
			0, 2, 1,
			0, 4, 2,
			0, 3, 4,
			0, 1, 3,
			1, 2, 4,
			1, 4, 3
		};

		shape_data.push_back(SShapeData());
		shape_data[SHAPE_TETRA].coords =
		{
			0.f,	1.f,	0.f,
			1.f,	-1.f,	1.f,
			-1.f,	-1.f,	1.f,
			0.f,	-1.f,	-1.f
		};

		shape_data[SHAPE_TETRA].indices =
		{
			0, 1, 2,
			0, 2, 3,
			0, 3, 1,
			1, 3, 2
		};




		shapes.push_back(CShape());
		shapes[0].setData(shape_data[SHAPE_LINE]);
		for (int i = 0; i < 8; ++i)
			shapes[0].colors.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		shapes[0].updateBuffer();


		shapes.push_back(CShape());
		shapes[1].setData(shape_data[SHAPE_LINE]);
		for (int i = 0; i < 8; ++i)
			shapes[1].colors.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		shapes[1].rotate(90.f, glm::vec3(0.f, 0.f, 1.f));
		shapes[1].updateBuffer();


		shapes.push_back(CShape());
		shapes[2].setData(shape_data[SHAPE_SQUARE_PRAMID]);

		shapes[2].colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		for (int i = 0; i < 2; ++i)
			shapes[2].colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		for (int i = 0; i < 2; ++i)
			shapes[2].colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));


		shapes[2].scale(glm::vec3(0.5f, 0.5f, 0.5f));

		shapes[2].rotate(-30.f, glm::vec3(1.0, 0.0, 0.0));

		shapes[2].rotate(30.f, glm::vec3(0.0, 1.0, 0.0));

		shapes[2].updateBuffer();


	}

	void updateBuffer()
	{

	}



	// --
	// define cb func
	// --

	void Display() const
	{
		glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (depth)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		for (auto& shape : shapes) {
			if (solid)
				shape.draw(ShaderMgr.program, GL_TRIANGLES);
			else
				shape.draw(ShaderMgr.program, GL_LINE_LOOP);
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
		case 'c':
			shapes[2].setData(shape_data[SHAPE_DICE]);

			shapes[2].colors.clear();

			for (int i = 0; i < 2; ++i)
				shapes[2].colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
			for (int i = 0; i < 2; ++i)
				shapes[2].colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
			for (int i = 0; i < 2; ++i)
				shapes[2].colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
			for (int i = 0; i < 2; ++i)
				shapes[2].colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f));

			shapes[2].updateBuffer();
			break;
		case 'p':
			shapes[2].setData(shape_data[SHAPE_SQUARE_PRAMID]);

			shapes[2].colors.clear();

			shapes[2].colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
			for (int i = 0; i < 2; ++i)
				shapes[2].colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
			for (int i = 0; i < 2; ++i)
				shapes[2].colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

			shapes[2].updateBuffer();
			break;
		case 'h':
			if (depth)
				depth = false;
			else
				depth = true;
			break;

		case 'i': case 'I':
			if (solid)
				solid = false;
			else
				solid = true;
			break;

		case 'x':
			if (rotate_x == 1)
				rotate_x = 0;
			else 
				rotate_x = 1;
			break;

		case 'X':
			if (rotate_x == -1)
				rotate_x = 0;
			else
				rotate_x = -1;
			break;

		case 'y':
			if (rotate_y == 1)
				rotate_y = 0;
			else
				rotate_y = 1;
			break;

		case 'Y':
			if (rotate_y == -1)
				rotate_y = 0;
			else
				rotate_y = -1;
			break;

		case 'd':
			shapes[2].translate(glm::vec3(0.03f, 0.f, 0.f));
			break;
		case 'a':
			shapes[2].translate(glm::vec3(-0.03f, 0.f, 0.f));
			break;
		case 'w':
			shapes[2].translate(glm::vec3(0.0f, 0.03f, 0.f));
			break;
		case 's':
			shapes[2].translate(glm::vec3(0.f, -0.03f, 0.f));
			break;

		case 'r':
			shapes[2].clearMatrix();
			shapes[2].scale(glm::vec3(0.5f, 0.5f, 0.5f));
			shapes[2].rotate(-30.f, glm::vec3(1.0, 0.0, 0.0));
			shapes[2].rotate(30.f, glm::vec3(0.0, 1.0, 0.0));
			rotate_x = false;
			rotate_y = false;
			break;


		default:
			break;
		}
	}

	void Motion(const int _x, const int _y)
	{

	}

	// --
	// process func
	// --

	void updateState()
	{
		if (rotate_x != 0)
			shapes[2].rotate(1. * rotate_x, glm::vec3(1.f, 0.f, 0.f));

		if (rotate_y != 0)
			shapes[2].rotate(1. * rotate_y, glm::vec3(0.f, 1.f, 0.f));
	}


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

int main(int argc, char** argv)
{
	Window.init(argc, argv);
	Window.run();
}