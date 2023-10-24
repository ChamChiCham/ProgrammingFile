#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <cmath>

#include "CShaderManager.h"
#include "Define.h"
#include "Struct.h"

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
	SShapeData* data;


public:

	std::vector<glm::mat4> mats;
	std::vector<glm::vec3> colors;


public:

	CShape() : data{}, mats{} {}

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

	void draw(const unsigned int _program, const SView& _view, const glm::mat4& _proj, const int _mode) const
	{
		glUseProgram(_program);
		glBindVertexArray(buffer.VAO);

		glm::mat4 MM = glm::mat4(1.f);
		for (auto& mat : mats) {
			MM = mat * MM;
		}

		glm::mat4 MV = glm::lookAt(_view.eye, _view.at, _view.up);
		glm::mat4 MP = _proj;

		// uniform matrix 
		unsigned int transformLocation = glGetUniformLocation(_program, "modelTransform");
		unsigned int viewLocation = glGetUniformLocation(_program, "viewTransform");
		unsigned int projectionLocation = glGetUniformLocation(_program, "projectionTransform");

		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &MM[0][0]);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &MV[0][0]);
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &MP[0][0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.EBO);
		glDrawElements(_mode, data->indices.size(), GL_UNSIGNED_INT, 0);
	}

	void scale(const int _idx, const float _fir, const float _sec, const float _thi)
	{
		scale(_idx, glm::vec3(_fir, _sec, _thi));
	}

	void scale(const int _idx, glm::vec3 _vec)
	{
		while (mats.size() <= _idx) {
			mats.push_back(glm::mat4(1.f));
		}

		mats[_idx] = glm::scale(mats[_idx], _vec);
	}

	void rotate(const int _idx, const float _deg, const float _fir, const float _sec, const float _thi)
	{
		rotate(_idx, _deg, glm::vec3(_fir, _sec, _thi));
	}

	void rotate(const int _idx, const float _deg, glm::vec3 _vec)
	{
		while (mats.size() <= _idx) {
			mats.push_back(glm::mat4(1.f));
		}

		mats[_idx] = glm::rotate(mats[_idx], glm::radians(_deg), _vec);
	}


	void translate(const int _idx, const float _fir, const float _sec, const float _thi)
	{
		translate(_idx, glm::vec3(_fir, _sec, _thi));
	}

	void translate(const int _idx, glm::vec3 _vec)
	{
		while (mats.size() <= _idx) {
			mats.push_back(glm::mat4(1.f));
		}

		mats[_idx] = glm::translate(mats[_idx], _vec);
	}
};

class CLine {

private:
	SBuffer buffer;
	glm::vec3 color = glm::vec3{ 0.f, 0.f, 0.f };

public:

	std::vector<glm::mat4> mats;
	std::vector<GLfloat> coords;



public:

	void updateBuffer()
	{

		if (buffer.VAO != 0) {
			glDeleteVertexArrays(1, &buffer.VAO);
			glDeleteBuffers(2, buffer.VBO);
		}


		std::vector<GLfloat> VBO1;

		for (int i = 0; i < coords.size() / 3; ++i) {
			VBO1.push_back(color.x);
			VBO1.push_back(color.y);
			VBO1.push_back(color.z);
		}


		glGenBuffers(2, buffer.VBO);

		glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(GLfloat), coords.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, VBO1.size() * sizeof(GLfloat), VBO1.data(), GL_STATIC_DRAW);


		glGenVertexArrays(1, &buffer.VAO);
		glBindVertexArray(buffer.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(1);

	}

	void draw(const unsigned int _program, const SView& _view, const glm::mat4& _proj, const int _mode) const
	{
		glUseProgram(_program);
		glBindVertexArray(buffer.VAO);

		glm::mat4 MM = glm::mat4(1.f);
		for (auto& mat : mats) {
			MM = mat * MM;
		}

		glm::mat4 MV = glm::lookAt(_view.eye, _view.at, _view.up);
		glm::mat4 MP = _proj;

		// uniform matrix 
		unsigned int transformLocation = glGetUniformLocation(_program, "modelTransform");
		unsigned int viewLocation = glGetUniformLocation(_program, "viewTransform");
		unsigned int projectionLocation = glGetUniformLocation(_program, "projectionTransform");

		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &MM[0][0]);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &MV[0][0]);
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &MP[0][0]);

		glDrawArrays(_mode, 0, coords.size() / 3);
	}

	void setColor(const float _r, const float _g, const float _b) {
		color = glm::vec3(_r, _g, _b);
	}

	void scale(const int _idx, const float _fir, const float _sec, const float _thi)
	{
		scale(_idx, glm::vec3(_fir, _sec, _thi));
	}

	void scale(const int _idx, glm::vec3 _vec)
	{
		while (mats.size() <= _idx) {
			mats.push_back(glm::mat4(1.f));
		}

		mats[_idx] = glm::scale(mats[_idx], _vec);
	}

	void rotate(const int _idx, const float _deg, const float _fir, const float _sec, const float _thi)
	{
		rotate(_idx, _deg, glm::vec3(_fir, _sec, _thi));
	}

	void rotate(const int _idx, const float _deg, glm::vec3 _vec)
	{
		while (mats.size() <= _idx) {
			mats.push_back(glm::mat4(1.f));
		}

		mats[_idx] = glm::rotate(mats[_idx], glm::radians(_deg), _vec);
	}


	void translate(const int _idx, const float _fir, const float _sec, const float _thi)
	{
		translate(_idx, glm::vec3(_fir, _sec, _thi));
	}

	void translate(const int _idx, glm::vec3 _vec)
	{
		while (mats.size() <= _idx) {
			mats.push_back(glm::mat4(1.f));
		}

		mats[_idx] = glm::translate(mats[_idx], _vec);
	}

	void addCoord(const float _x, const float _y, const float _z)
	{
		coords.push_back(_x);
		coords.push_back(_y);
		coords.push_back(_z);
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
	SView view;
	glm::mat4 proj = glm::mat4(1.f);

	// bg color
	glm::vec3 bg_color = glm::vec3(1.f, 1.f, 1.f);

	// shape data list
	std::vector<SShapeData> shape_data;



	// --
	// process member Variable
	// --

	std::vector<CShape> shapes;
	std::vector<CLine> lines;

	bool mode_targ[2] = { false, false };
	bool mode_c[2] = { false, false };

	bool mode_r = false;
	int mode_r_time = 0;
	float mode_r_setta = 0;
	float mode_r_r = 0;
	float mode_r_setta1 = 0;
	float mode_r_r1 = 0;


	int mode_t_time = 0;
	int mode_8_time = 0;
	bool mode_8 = false;
	int mode_9_time = 0;
	int mode_0_time = 0;
	bool mode_0 = false;





public:

	CWindowMgr()
	{}


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


		// --
		// create shape data
		// --
		shape_data.push_back(SShapeData());
		shape_data[SHAPE_DICE].coords =
		{
			1.f,	1.f,	1.f,
			-1.f,	1.f,	1.f,
			1.f,	-1.f,	1.f,
			-1.f,	-1.f,	1.f,
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
			0, 1, 3,
			0, 3, 2,
			0, 2, 4,
			2, 6, 4,
			2, 3, 6,
			3, 7, 6,
			4, 6, 7,
			4, 7, 5,
			7, 3, 5,
			1, 5, 3,
			4, 5, 1,
			0, 4, 1
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
			0, 2, 1,
			0, 3, 2,
			0, 1, 3,
			1, 2, 3
		};

		shape_data.push_back(SShapeData());
		shape_data[SHAPE_SQUARE].coords =
		{
			1.f,	0.f,	1.f,
			-1.f,	0.f,	1.f,
			1.f,	0.f,	-1.f,
			-1.f,	0.f,	-1.f
		};

		shape_data[SHAPE_SQUARE].indices =
		{
			0, 2, 1,
			1, 2, 3
		};

		shape_data.push_back(SShapeData());
		shape_data[SHAPE_TRIANGLE].coords =
		{
			1.f,	0.f,	1.f,
			-1.f,	0.f,	1.f,
			0.f,	0.f,	-1.f,
		};

		shape_data[SHAPE_TRIANGLE].indices =
		{
			0, 2, 1
		};

		createObject();

		// --
		// make shapes
		// --

		shapes.push_back(CShape());
		shapes[0].setData(shape_data[SHAPE_SPHERE]);
		for (int i = 0; i < shape_data[SHAPE_SPHERE].coords.size() / 3; ++i)
			shapes[0].colors.push_back(glm::vec3(1.f, 0.f, 0.f));
		shapes[0].translate(0, 0.f, -0.05f, 0.f);
		shapes[0].scale(1, 0.392837782f, 0.392837782f, 0.392837782f);
		shapes[0].scale(1, 0.1f, 0.1f, 0.1f);
		shapes[0].translate(3, -0.5f, 0.f, 0.f);

		shapes.push_back(CShape());
		shapes[1].setData(shape_data[SHAPE_DICE]);
		for (int i = 0; i < shape_data[SHAPE_DICE].coords.size() / 3; ++i)
			shapes[1].colors.push_back(glm::vec3(0.f, 1.f, 0.f));
		shapes[1].scale(1, 0.1f, 0.1f, 0.1f);
		shapes[1].translate(3, 0.5f, 0.f, 0.f);



		lines.push_back(CLine());
		lines[0].addCoord(-1.f, 0.f, 0.f);
		lines[0].addCoord(1.f, 0.f, 0.f);
		lines[0].setColor(1.f, 0.f, 0.f);

		lines.push_back(CLine());
		lines[1].addCoord(0.f, -1.f, 0.f);
		lines[1].addCoord(0.f, 1.f, 0.f);
		lines[1].setColor(0.f, 1.f, 0.f);


		lines.push_back(CLine());
		lines[2].addCoord(0.f, 0.f, -1.f);
		lines[2].addCoord(0.f, 0.f, 1.f);
		lines[2].setColor(0.f, 0.f, 1.f);

		lines.push_back(CLine());
		lines[3].setColor(0.f, 1.f, 1.f);

		float setta = 0.f;
		for (float i = 0; i < 0.99f; i += 0.01f) {
			lines[3].addCoord(i * glm::cos(glm::radians(setta)), 0.f, i * glm::sin(glm::radians(setta)));
			setta += 20.f;
			if (setta == 360.f)
				setta = 0.f;
		}





		for (auto& shape : shapes)
			shape.updateBuffer();

		for (auto& line : lines)
			line.updateBuffer();


		// --
		// set view
		// --

		view.eye = glm::vec3(2.0f, 2.0f, 2.0f);
		view.at = glm::vec3(0.f, 0.f, 0.f);
		view.up = glm::vec3(0.f, 1.f, 0.f);
		proj = glm::ortho(-1.f, 1.f, -1.f, 1.f, 0.1f, 10.f);


		// --
		// explain
		// --

		std::cout << "- 도형 변환" << std::endl;
		std::cout << "1/2/3: 변환할 도형을 선택 (1: 왼쪽 / 2: 오른쪽 / 3: 둘 다)" << std::endl;
		std::cout << "x/X, y/Y, z/Z: '선택한 도형'을 이동" << std::endl;
		std::cout << "c/C: '선택한 도형'을 확대/축소" << std::endl;
		std::cout << "a/A: '선택한 도형'을 원점에 대해 확대/축소" << std::endl;
		std::cout << "w/s: '전체 도형' 위/아래 이동" << std::endl << std::endl;

		std::cout << "- 애니메이션 구현" << std::endl;
		std::cout << "r:  xz 평면에 스파이럴을 그리고, 그 스파이럴 위치에 따라 객체 이동 애니메이션" << std::endl;
		std::cout << "t: 현재 자리에서 두 도형이 원점으로 이동  제자리로 다시 이동하는 애니메이션" << std::endl;
		std::cout << "8: 두 도형이 중심점을 통과하며 상대방의 자리로 이동하는 애니메이션" << std::endl;
		std::cout << "9: 두 도형이 공전하면서 상대방의 자리로 이동하는 애니메이션" << std::endl;
		std::cout << "0: 두 도형이 공전하면서 상대방의 자리로 이동하는 애니메이션" << std::endl;



	}

	void createObject()
	{
		std::ifstream file("Sphere.obj");
		std::string line;

		if (!file.is_open()) {
			std::cerr << "ERROR: file open failed\n" << std::endl;
			return;
		}


		shape_data.push_back(SShapeData());
		while (std::getline(file, line)) {

			if (line.substr(0, 2) == "v ") {
				float vertices[3] = {};
				sscanf_s(line.c_str(), "v %f %f %f", &vertices[0], &vertices[1], &vertices[2]);

				for (auto& vertex : vertices)
					shape_data[SHAPE_SPHERE].coords.push_back(vertex);
			}
			else if (line.substr(0, 2) == "f ") {
				int indices[3] = {}, dummy0[3] = {}, dummy1[3] = {};
				sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
					&indices[0], &dummy0[0], &dummy1[0],
					&indices[1], &dummy0[1], &dummy1[1],
					&indices[2], &dummy0[2], &dummy1[2]);
				for (auto& index : indices) {
					shape_data[SHAPE_SPHERE].indices.push_back(index - 1);
				}
			}
		}

		file.close();

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


		glEnable(GL_DEPTH_TEST);

		int mode = GL_TRIANGLES;

		for (auto& shape : shapes)
			shape.draw(ShaderMgr.program, view, proj, mode);

		for (int i = 0; i < 3; ++i)
			lines[i].draw(ShaderMgr.program, view, proj, GL_LINE_LOOP);

		if (mode_r)
			lines[3].draw(ShaderMgr.program, view, proj, GL_LINE_LOOP);

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
			mode_targ[0] = true;
			mode_targ[1] = false;
			break;

		case '2':
			mode_targ[0] = false;
			mode_targ[1] = true;
			break;

		case '3':
			mode_targ[0] = true;
			mode_targ[1] = true;
			break;

		case 'x':
			if (mode_targ[0]) {
				shapes[0].translate(3, 0.05f, 0.f, 0.f);
			}

			if (mode_targ[1]) {
				shapes[1].translate(3, 0.05f, 0.f, 0.f);
			}
			break;

		case 'X':
			if (mode_targ[0]) {
				shapes[0].translate(3, -0.05f, 0.f, 0.f);
			}

			if (mode_targ[1]) {
				shapes[1].translate(3, -0.05f, 0.f, 0.f);
			}
			break;

		case 'y':
			if (mode_targ[0]) {
				shapes[0].translate(3, 0.0f, 0.05f, 0.f);
			}

			if (mode_targ[1]) {
				shapes[1].translate(3, 0.0f, 0.05f, 0.f);
			}
			break;

		case 'Y':
			if (mode_targ[0]) {
				shapes[0].translate(3, 0.0f, -0.05f, 0.f);
			}

			if (mode_targ[1]) {
				shapes[1].translate(3, 0.0f, -0.05f, 0.f);
			}
			break;

		case 'z':
			if (mode_targ[0]) {
				shapes[0].translate(3, 0.0f, 0.f, 0.05f);
			}

			if (mode_targ[1]) {
				shapes[1].translate(3, 0.0f, 0.f, 0.05f);
			}
			break;

		case 'Z':
			if (mode_targ[0]) {
				shapes[0].translate(3, 0.0f, 0.f, -0.05f);
			}

			if (mode_targ[1]) {
				shapes[1].translate(3, 0.0f, 0.f, -0.05f);
			}
			break;


		case 'c':
			if (mode_targ[0]) {
				shapes[0].scale(1, 1.25f, 1.25f, 1.25f);
			}

			if (mode_targ[1]) {
				shapes[1].scale(1, 1.25f, 1.25f, 1.25f);
			}
			break;

		case 'C':
			if (mode_targ[0]) {
				shapes[0].scale(1, 0.8f, 0.8f, 0.8f);
			}

			if (mode_targ[1]) {
				shapes[1].scale(1, 0.8f, 0.8f, 0.8f);
			}
			break;

		case 'a':
			if (mode_targ[0]) {
				shapes[0].scale(4, 1.25f, 1.25f, 1.25f);
			}

			if (mode_targ[1]) {
				shapes[1].scale(4, 1.25f, 1.25f, 1.25f);
			}
			break;

		case 'A':
			if (mode_targ[0]) {
				shapes[0].scale(4, 0.8f, 0.8f, 0.8f);
			}

			if (mode_targ[1]) {
				shapes[1].scale(4, 0.8f, 0.8f, 0.8f);
			}
			break;


		case 'w': case 'W':
			shapes[0].translate(5, 0.f, 0.05f, 0.f);
			shapes[1].translate(5, 0.f, 0.05f, 0.f);
			lines[0].translate(0, 0.f, 0.05f, 0.f);
			lines[2].translate(0, 0.f, 0.05f, 0.f);

			break;

		case 's': case 'S':
		
			shapes[0].translate(5, 0.f, -0.05f, 0.f);
			shapes[1].translate(5, 0.f, -0.05f, 0.f);
			lines[0].translate(0, 0.f, -0.05f, 0.f);
			lines[2].translate(0, 0.f, -0.05f, 0.f);

			break;


		case 'r':
			if (mode_r_time != 0)
				break;

			if (mode_r) {
				mode_r = false;
				shapes[0].mats[3] = glm::mat4(1.f);
				shapes[1].mats[3] = glm::mat4(1.f);
				shapes[0].translate(3, -0.5f, 0.f, 0.f);
				shapes[1].translate(3, 0.5f, 0.f, 0.f);
			}
			else {
				mode_r = true;
				mode_r_time = 396;
				mode_r_r = 0;
				mode_r_setta = 0;
				mode_r_r1 = 0;
				mode_r_setta1 = 0;
				shapes[0].mats[3] = glm::mat4(1.f);
				shapes[1].mats[3] = glm::mat4(1.f);
			}

			break;

		case 't':
			if (mode_t_time != 0)
				break;

			mode_t_time = 200;
			break;

		case '8':
			if (mode_8_time != 0)
				break;
			mode_8 = !mode_8;
			mode_8_time = 200;
			break;

		case '9':
			if (mode_9_time != 0)
				break;

			mode_9_time = 180;
			break;

		case '0':
			if (mode_0_time != 0)
				break;

			mode_8 = !mode_8;
			mode_0_time = 200;
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
		if (mode_r && mode_r_time != 0) {

			shapes[0].translate(3, - mode_r_r * glm::cos(glm::radians(mode_r_setta)), 0.f, - mode_r_r * glm::sin(glm::radians(mode_r_setta)));
			shapes[1].translate(3, -mode_r_r1 * glm::cos(glm::radians(mode_r_setta1)), 0.f, -mode_r_r1 * glm::sin(glm::radians(mode_r_setta1)));


			mode_r_r1 += 0.00125f;
			mode_r_setta1 += 2.5f;
			mode_r_r += 0.0025f;
			mode_r_setta += 5.f;

			shapes[0].translate(3, mode_r_r * glm::cos(glm::radians(mode_r_setta)), 0.f, mode_r_r * glm::sin(glm::radians(mode_r_setta)));
			shapes[1].translate(3, mode_r_r1 * glm::cos(glm::radians(mode_r_setta1)), 0.f, mode_r_r1 * glm::sin(glm::radians(mode_r_setta1)));

			mode_r_time--;
		}

		if (mode_t_time != 0) {
			if (mode_t_time > 100) {
				shapes[0].translate(3, 0.005f, 0.f, 0.f);
				shapes[1].translate(3, -0.005f, 0.f, 0.f);
			}
			else {
				shapes[0].translate(3, -0.005f, 0.f, 0.f);
				shapes[1].translate(3, +0.005f, 0.f, 0.f);
			}

			mode_t_time--;
		}

		if (mode_8_time != 0) {
			if (mode_8) {
				shapes[0].translate(3, 0.005f, 0.f, 0.f);
				shapes[1].translate(3, -0.005f, 0.f, 0.f);
			}
			else {
				shapes[0].translate(3, -0.005f, 0.f, 0.f);
				shapes[1].translate(3, 0.005f, 0.f, 0.f);
			}
			mode_8_time--;
		}

		if (mode_9_time != 0) {
			shapes[0].rotate(6, 1.f, 0.f, 1.f, 0.f);
			shapes[1].rotate(6, 1.f, 0.f, 1.f, 0.f);
			mode_9_time--;
		}

		if (mode_0_time != 0) {
			if (mode_0_time > 100) {
				if (mode_8) {
					shapes[0].translate(3, 0.005f, 0.005f, 0.f);
					shapes[1].translate(3, -0.005f, -0.005f, 0.f);
				}
				else {
					shapes[0].translate(3, -0.005f, -0.005f, 0.f);
					shapes[1].translate(3, 0.005f, +0.005f, 0.f);
				}
			}
			else {
				if (mode_8) {
					shapes[0].translate(3, 0.005f, -0.005f, 0.f);
					shapes[1].translate(3, -0.005f, +0.005f, 0.f);
				}
				else {
					shapes[0].translate(3, -0.005f, 0.005f, 0.f);
					shapes[1].translate(3, 0.005f, -0.005f, 0.f);
				}
			}

			mode_0_time--;
		}


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