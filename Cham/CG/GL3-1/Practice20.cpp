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

	int mode_b = 0;
	int mode_m = 0;
	int mode_f = 0;
	int mode_f_status = 0;
	int mode_e = -1;
	int mode_e_time = 0;

	int mode_t = 0;
	int mode_t_status = 0;

	int mode_a = 0;



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
		//shapes.push_back(CShape());
		//shapes[0].setData(shape_data[SHAPE_LINE]);
		//for (int i = 0; i < 8; ++i)
		//	shapes[0].colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

		//shapes.push_back(CShape());
		//shapes[1].setData(shape_data[SHAPE_LINE]);
		//for (int i = 0; i < 8; ++i)
		//	shapes[1].colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		//shapes[1].rotate(0, 90.f, 0.f, 0.f, 1.f);

		//shapes.push_back(CShape());
		//shapes[2].setData(shape_data[SHAPE_LINE]);
		//for (int i = 0; i < 8; ++i)
		//	shapes[2].colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		//shapes[2].rotate(0, 90.f, 0.f, 1.f, 0.f);


		shapes.push_back(CShape());
		shapes[0].setData(shape_data[SHAPE_SQUARE]);
		for (int i = 0; i < 4; ++i) {
			shapes[0].colors.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		}
		shapes[0].scale(0, 10.f, 1.f, 10.f);


		shapes.push_back(CShape());
		shapes[1].setData(shape_data[SHAPE_DICE]);
		for (int i = 0; i < 8; ++i) {
			shapes[1].colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		shapes[1].scale(0, 1.f, 0.6f, 1.f);
		shapes[1].translate(3, 0.f, 0.3f, 0.f);


		shapes.push_back(CShape());
		shapes[2].setData(shape_data[SHAPE_DICE]);
		for (int i = 0; i < 8; ++i) {
			shapes[2].colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		}
		shapes[2].scale(0, 0.6f, 0.4f, 0.6f);
		shapes[2].translate(3, 0.f, 0.8f, 0.f);


		shapes.push_back(CShape());
		shapes[3].setData(shape_data[SHAPE_DICE]);
		for (int i = 0; i < 8; ++i) {
			shapes[3].colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		}
		shapes[3].scale(0, 0.1f, 0.6f, 0.1f);
		shapes[3].translate(1, 0.f, 0.3f, 0.f);
		shapes[3].translate(3, -0.2f, 1.0f, 0.f);


		shapes.push_back(CShape());
		shapes[4].setData(shape_data[SHAPE_DICE]);
		for (int i = 0; i < 8; ++i) {
			shapes[4].colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
		}
		shapes[4].scale(0, 0.1f, 0.6f, 0.1f);
		shapes[4].translate(1, 0.f, 0.3f, 0.f);
		shapes[4].translate(3, 0.2f, 1.0f, 0.f);

		shapes.push_back(CShape());
		shapes[5].setData(shape_data[SHAPE_DICE]);
		for (int i = 0; i < 8; ++i) {
			shapes[5].colors.push_back(glm::vec3(0.0f, 1.0f, 1.0f));
		}
		shapes[5].scale(0, 0.1f, 0.1f, 0.6f);
		shapes[5].translate(1, 0.0f, 0.0f, 0.45f);
		shapes[5].translate(3, 0.7f, 0.1f, 0.8f);

		shapes.push_back(CShape());
		shapes[6].setData(shape_data[SHAPE_DICE]);
		for (int i = 0; i < 8; ++i) {
			shapes[6].colors.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
		}
		shapes[6].scale(0, 0.1f, 0.1f, 0.6f);
		shapes[6].translate(1, 0.0f, 0.0f, 0.45f);
		shapes[6].translate(3, -0.7f, 0.1f, 0.8f);



		for (auto& shape : shapes)
			shape.updateBuffer();

		for (auto& line : lines)
			line.updateBuffer();


		// --
		// set view
		// --

		view.eye = glm::vec3(5.0f, 5.0f, 5.0f);
		view.at = glm::vec3(0.f, 0.f, 0.f);
		view.up = glm::vec3(0.f, 1.f, 0.f);
		proj = glm::perspective(glm::radians(60.f), 1.f, 0.1f, 30.f);


		// --
		// explain
		// --

		std::cout << "- 도형 변환" << std::endl;
		std::cout << "b/B: 크레인의 아래 몸체가 x축 방향으로 양/음 방향으로 이동한다. 다시 누르면 멈춘다." << std::endl;
		std::cout << "m/M: 크레인의 중앙 몸체가 y축에 대하여 양/음 방향으로 회전한다. 다시 누르면 멈춘다" << std::endl;
		std::cout << "f/F: 포신이 y축에 대하여 양/음 방향으로 회전하는데, 두 포신이 서로 반대방향으로 회전한다. 다시 누르면 멈춘다." << std::endl;
		std::cout << "e/E: 2개 포신이 조금씩 이동해서 한 개가 된다/다시 제자리로 이동해서 2개가 된다." << std::endl;
		std::cout << "t/T: 크레인의 맨 위 2개의 팔이 z축에 대하여 양/음 방향으로 서로 반대방향으로 회전한다.  다시 누르면 멈춘다." << std::endl << std::endl;

		std::cout << "- 카메라 변환" << std::endl;
		std::cout << "z/Z: 카메라가 z축 양/음 방향으로 이동" << std::endl;
		std::cout << "x/X: 카메라가 x축 양/음 방향으로 이동" << std::endl;
		std::cout << "y/Y: 카메라 기준 y축에 대하여 회전" << std::endl;
		std::cout << "r/R: 화면의 중심의 y축에 대하여 카메라가 회전" << std::endl;
		std::cout << "a/A: 화면의 중심의 y축에 대하여 카메라가 회전하는 애니메이션을 진행한다/멈춘다." << std::endl << std::endl;

		std::cout << "- 동작 제어" << std::endl;
		std::cout << "s/S: 모든 움직임 멈추기" << std::endl;
		std::cout << "c/C: 모든 움직임이 초기화된다." << std::endl;
		std::cout << "q/Q: 프로그램 종료하기" << std::endl;




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

		for (auto& line : lines)
			line.draw(ShaderMgr.program, view, proj, GL_LINE_LOOP);

		glutSwapBuffers();
	}

	void Mouse(const int _button, const int _state, const int _x, const int _y)
	{

	}

	void Keyboard(const unsigned char _key, const int _x, const int _y)
	{
		float d = 0.f;
		float s = 0.f;

		switch (_key)
		{
		case 'b':
			if (mode_b == 0)
				mode_b = 1;
			else if (mode_b == 1)
				mode_b = 0;
			break;
		case 'B':
			if (mode_b == 0)
				mode_b = -1;
			else if (mode_b == -1)
				mode_b = 0;
			break;

		case 'm':
			if (mode_m == 0)
				mode_m = 1;
			else if (mode_m == 1)
				mode_m = 0;
			break;
		case 'M':
			if (mode_m == 0)
				mode_m = -1;
			else if (mode_m == -1)
				mode_m = 0;
			break;

		case 'f':
			if (mode_f == 0)
				mode_f = 1;
			else if (mode_f == 1)
				mode_f = 0;
			break;
		case 'F':
			if (mode_f == 0)
				mode_f = -1;
			else if (mode_f == -1)
				mode_f = 0;
			break;


		case 'e': 
			if (mode_e_time != 0 && mode_e == -1)
				break;
			mode_e = 1;
			mode_e_time = 100;
			break;

		case 'E':
			if (mode_e_time != 0 && mode_e == 1)
				break;
			mode_e = -1;
			mode_e_time = 100;
			break;

		case 't':
			if (mode_t == 0)
				mode_t = 1;
			else if (mode_t == 1)
				mode_t = 0;
			break;
		case 'T':
			if (mode_t == 0)
				mode_t = -1;
			else if (mode_t == -1)
				mode_t = 0;
			break;

		case 'z':
			view.eye.z += 0.2f;
			view.at.z += 0.2f;
			break;

		case 'Z':
			view.eye.z -= 0.2f;
			view.at.z -= 0.2f;
			break;

		case 'x':
			view.eye.x += 0.2f;
			view.at.x += 0.2f;
			break;

		case 'X':
			view.eye.x -= 0.2f;
			view.at.x -= 0.2f;
			break;

		case 'y':
			d = sqrtf(powf(view.at.x - view.eye.x, 2) + powf(view.at.z - view.eye.z, 2));
			s = atan2f(view.at.z - view.eye.z, view.at.x - view.eye.x);
			view.at.x = view.eye.x + d * cos(s + glm::radians(10.f));
			view.at.z = view.eye.z + d * sin(s + glm::radians(10.f));
			break;

		case 'Y':
			d = sqrtf(powf(view.at.x - view.eye.x, 2) + powf(view.at.z - view.eye.z, 2));
			s = atan2f(view.at.z - view.eye.z, view.at.x - view.eye.x);
			view.at.x = view.eye.x + d * cos(s + glm::radians(-10.f));
			view.at.z = view.eye.z + d * sin(s + glm::radians(-10.f));
			break;

		case 'r':
			d = sqrtf(powf(view.eye.x - view.at.x, 2) + powf(view.eye.z - view.at.z, 2));
			s = atan2f(view.eye.z - view.at.z, view.eye.x - view.at.x);
			view.eye.x = view.at.x + d * cos(s + glm::radians(10.f));
			view.eye.z = view.at.z + d * sin(s + glm::radians(10.f));
			break;

		case 'R':
			d = sqrtf(powf(view.eye.x - view.at.x, 2) + powf(view.eye.z - view.at.z, 2));
			s = atan2f(view.eye.z - view.at.z, view.eye.x - view.at.x);
			view.eye.x = view.at.x + d * cos(s + glm::radians(-10.f));
			view.eye.z = view.at.z + d * sin(s + glm::radians(-10.f));
			break;

		case 'a':
			if (mode_a == 0)
				mode_a = 1;
			else if (mode_a == 1)
				mode_a = 0;
			break;
		case 'A':
			if (mode_a == 0)
				mode_a = -1;
			else if (mode_a == -1)
				mode_a = 0;
			break;

		case 's': case 'S':
			mode_b = mode_m = mode_f = mode_t = mode_a = 0;

			break;

		case 'c': case 'C':
			mode_b = mode_m = mode_f = mode_t = mode_a = 0;
			mode_e_time = 0;
			mode_f_status = 0;


			for (int i = 1; i < shapes.size(); ++i) {
				shapes[i].translate(5, 0.f, 0.f, 0.f);
				shapes[i].mats[5] = glm::mat4(1.f);
			}

			shapes[2].mats[4] = glm::mat4(1.f);
			shapes[3].mats[4] = glm::mat4(1.f);
			shapes[4].mats[4] = glm::mat4(1.f);

			shapes[5].mats[2] = glm::mat4(1.f);
			shapes[6].mats[2] = glm::mat4(1.f);

			shapes[5].mats[3] = glm::mat4(1.f);
			shapes[6].mats[3] = glm::mat4(1.f);

			shapes[5].translate(3, 0.7f, 0.1f, 0.8f);
			shapes[6].translate(3, -0.7f, 0.1f, 0.8f);

			shapes[3].mats[2] = glm::mat4(1.f);
			shapes[4].mats[2] = glm::mat4(1.f);

			view.eye = glm::vec3(5.0f, 5.0f, 5.0f);
			view.at = glm::vec3(0.f, 0.f, 0.f);

			break;

		case 'q': case 'Q':
			PostQuitMessage(0);
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
		if (mode_b != 0) {
			for (int i = 1; i < shapes.size(); ++i) {
				shapes[i].translate(5, 0.02f * mode_b, 0.f, 0.f);
			}
		}

		if (mode_m != 0) {
			shapes[2].rotate(4, 1.f * mode_m, 0.f, 1.f, 0.f);
			shapes[3].rotate(4, 1.f * mode_m, 0.f, 1.f, 0.f);
			shapes[4].rotate(4, 1.f * mode_m, 0.f, 1.f, 0.f);
		}

		if (mode_f != 0) {
			shapes[5].rotate(2, -1.f * mode_f, 0.f, 1.f, 0.f);
			shapes[6].rotate(2, 1.f * mode_f, 0.f, 1.f, 0.f);
			
			
			mode_f_status += mode_f;
			if (mode_f_status == 360)
				mode_f_status = 0;
			else if (mode_f_status == -1)
				mode_f_status = 359;
		}

		if (mode_e_time != 0) {

			if (mode_f_status != 0) {
				shapes[5].rotate(2, -1.f, 0.f, 1.f, 0.f);
				shapes[6].rotate(2, 1.f, 0.f, 1.f, 0.f);
				mode_f_status += 1;
				if (mode_f_status == 360)
					mode_f_status = 0;
			}
			else {
				shapes[5].translate(3, -0.007f * mode_e, 0.f, 0.f);
				shapes[6].translate(3, 0.007f * mode_e, 0.f, 0.f);
				mode_e_time -= 1;
			}
		}

		if (mode_t != 0) {
			if (mode_t == 1 && mode_t_status == 90) {
				mode_t = 0;
				return;
			}

			if (mode_t == -1 && mode_t_status == -90) {
				mode_t = 0;
				return;
			}

			shapes[3].rotate(2, 1.f * mode_t, 1.f, 0.f, 0.f);
			shapes[4].rotate(2, -1.f * mode_t, 1.f, 0.f, 0.f);
			mode_t_status += mode_t;
		}

		if (mode_a != 0) {
			float d = sqrtf(powf(view.eye.x - view.at.x, 2) + powf(view.eye.z - view.at.z, 2));
			float s = atan2f(view.eye.z - view.at.z, view.eye.x - view.at.x);
			view.eye.x = view.at.x + d * cos(s + glm::radians(1.f * mode_a));
			view.eye.z = view.at.z + d * sin(s + glm::radians(1.f * mode_a));
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