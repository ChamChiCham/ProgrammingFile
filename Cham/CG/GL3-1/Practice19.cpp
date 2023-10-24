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


	float setta[3] = {};

	bool mode_p = false;
	bool mode_m = false;
	int mode_y = 0;
	int mode_z = 0;


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

		makeSphere();
		for (int i = 0; i < shape_data[SHAPE_SPHERE].coords.size() / 3; ++i)
			shapes[0].colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		shapes[0].scale(0, 0.3f, 0.3f, 0.3f);


		makeSphere();
		for (int i = 0; i < shape_data[SHAPE_SPHERE].coords.size() / 3; ++i)
			shapes[1].colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		shapes[1].scale(0, 0.2f, 0.2f, 0.2f);
		shapes[1].translate(1, 1.f, 0.f, 0.f);

		makeSphere();
		for (int i = 0; i < shape_data[SHAPE_SPHERE].coords.size() / 3; ++i)
			shapes[2].colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		shapes[2].scale(0, 0.2f, 0.2f, 0.2f);
		shapes[2].translate(1, 1.f, -1.f, 0.f);

		makeSphere();
		for (int i = 0; i < shape_data[SHAPE_SPHERE].coords.size() / 3; ++i)
			shapes[3].colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		shapes[3].scale(0, 0.2f, 0.2f, 0.2f);
		shapes[3].translate(1, -1.f, -1.f, 0.f);

		makeSphere();
		for (int i = 0; i < shape_data[SHAPE_SPHERE].coords.size() / 3; ++i)
			shapes[4].colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		shapes[4].scale(0, 0.1f, 0.1f, 0.1f);
		shapes[4].translate(1, 1.f, 0.f, 0.f);
		shapes[4].translate(3, 0.5f, 0.f, 0.f);

		makeSphere();
		for (int i = 0; i < shape_data[SHAPE_SPHERE].coords.size() / 3; ++i)
			shapes[5].colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		shapes[5].scale(0, 0.1f, 0.1f, 0.1f);
		shapes[5].translate(1, 1.f, -1.f, 0.f);
		shapes[5].translate(3, 0.5f, 0.f, 0.f);

		makeSphere();
		for (int i = 0; i < shape_data[SHAPE_SPHERE].coords.size() / 3; ++i)
			shapes[6].colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		shapes[6].scale(0, 0.1f, 0.1f, 0.1f);
		shapes[6].translate(1, -1.f, -1.f, 0.f);
		shapes[6].translate(3, -0.5f, 0.f, 0.f);



		makeCircle();

		makeCircle();
		lines[1].scale(0, 1.41421356f, 1.41421356f, 1.41421356f);
		lines[1].rotate(1, 45.f, 0.f, 0.f, 1.f);

		makeCircle();
		lines[2].scale(0, 1.41421356f, 1.41421356f, 1.41421356f);
		lines[2].rotate(1, -45.f, 0.f, 0.f, 1.f);

		makeCircle();
		lines[3].scale(0, 0.5f, 0.5f, 0.5f);
		lines[3].translate(2, 1.f, 0.f, 0.f);

		makeCircle();
		lines[4].scale(0, 0.5f, 0.5f, 0.5f);
		lines[4].translate(2, 1.f, -1.f, 0.f);

		makeCircle();
		lines[5].scale(0, 0.5f, 0.5f, 0.5f);
		lines[5].translate(2, -1.f, -1.f, 0.f);





		for (auto& shape : shapes)
			shape.updateBuffer();

		for (auto& line : lines)
			line.updateBuffer();


		// --
		// set view
		// --

		view.eye = glm::vec3(0.0f, 2.0f, 3.0f);
		view.at = glm::vec3(0.f, 0.f, 0.f);
		view.up = glm::vec3(0.f, 1.f, 0.f);
		proj = glm::ortho(-2.5f, 2.5f, -2.5f, 2.5f, 0.1f, 10.0f);



		// --
		// explain
		// --

		std::cout << "p/P: 직각 투영/원근 투영" << std::endl;
		std::cout << "m/M: 솔리드 모델/와이어 모델" << std::endl;
		std::cout << "w/a/s/d/+/-: 도형들을 이동" << std::endl;
		std::cout << "y/Y: y축으로 양/음 방향으로 회전" << std::endl;
		std::cout << "z/Z: 중심의 구를 제외하고 행성, 달, 궤도가 z축에 대하여 양/음 방향으로 일제히 회전" << std::endl;


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

	void makeSphere()
	{
		int idx = shapes.size();
		shapes.push_back(CShape());
		shapes[idx].setData(shape_data[SHAPE_SPHERE]);
		shapes[idx].scale(0, 0.392837782f, 0.392837782f, 0.392837782f);
		shapes[idx].translate(1, 0.f, -0.05f, 0.f);
	}

	void makeCircle()
	{
		int idx = lines.size();
		lines.push_back(CLine());
		for (float i = 0.f; i < 360.f; i += 10.f) {
			lines[idx].addCoord(glm::cos(glm::radians(i)), 0.f, glm::sin(glm::radians(i)));
		}
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

		int mode = mode_m ? GL_LINE_LOOP : GL_TRIANGLES;

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
		switch (_key)
		{
		case 'p': case 'P':
			if (mode_p) {
				mode_p = false;
				proj = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 10.0f);
			}
			else {
				mode_p = true;
				proj = glm::ortho(-2.5f, 2.5f, -2.5f, 2.5f, 0.1f, 10.0f);
			}
			break;

		case 'm': case 'M':
			mode_m = !mode_m;
			break;

		case 'w':
			for (auto& shape : shapes)
				shape.translate(6, 0.f, 0.1f, 0.f);

			for (auto& line : lines)
				line.translate(6, 0.f, 0.1f, 0.f);
			break;

		case 'a':
			for (auto& shape : shapes)
				shape.translate(6, -0.1f, 0.f, 0.f);

			for (auto& line : lines)
				line.translate(6, -0.1f, 0.f, 0.f);
			break;

		case 's':
			for (auto& shape : shapes)
				shape.translate(6, 0.f, -0.1f, 0.f);

			for (auto& line : lines)
				line.translate(6, 0.f, -0.1f, 0.f);
			break;

		case 'd':
			for (auto& shape : shapes)
				shape.translate(6, 0.1f, 0.f, 0.f);

			for (auto& line : lines)
				line.translate(6, 0.1f, 0.f, 0.f);
			break;

		case '+':
			for (auto& shape : shapes)
				shape.translate(6, 0.f, 0.f, 0.1f);

			for (auto& line : lines)
				line.translate(6, 0.f, 0.f, 0.1f);
			break;

		case '-':
			for (auto& shape : shapes)
				shape.translate(6, 0.f, 0.f, -0.1f);

			for (auto& line : lines)
				line.translate(6, 0.f, 0.f, -0.1f);
			break;

		case 'y':

			if (mode_y == 0)
				mode_y = 1;
			else if (mode_y == 1)
				mode_y = 0;
			break;
		
		case 'Y':
			if (mode_y == 0)
				mode_y = -1;
			else if (mode_y == -1)
				mode_y = 0;
			break;

		case 'z':
			if (mode_z == 0)
				mode_z = 1;
			else if (mode_z == 1)
				mode_z = 0;
			break;

		case 'Z':
			if (mode_z == 0)
				mode_z = -1;
			else if (mode_z == -1)
				mode_z = 0;
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
		shapes[1].rotate(2, 0.5f, 0.f, 1.f, 0.f);
		shapes[2].rotate(2, 1.f, 1.f, 1.f, 0.f);
		shapes[3].rotate(2, 2.f, 1.f, -1.f, 0.f);

		shapes[4].rotate(2, 0.5f, 0.f, 1.f, 0.f);
		shapes[5].rotate(2, 1.f, 1.f, 1.f, 0.f);
		shapes[6].rotate(2, 2.f, 1.f, -1.f, 0.f);


		lines[3].rotate(2, 0.5f, 0.f, -1.f, 0.f);
		lines[4].rotate(2, 1.0f, -1.f, -1.f, 0.f);
		lines[5].rotate(2, 2.0f, -1.f, 1.f, 0.f);

		lines[3].rotate(3, 0.5f, 0.f, 1.f, 0.f);
		lines[4].rotate(3, 1.0f, 1.f, 1.f, 0.f);
		lines[5].rotate(3, 2.0f, 1.f, -1.f, 0.f);

		
		shapes[4].mats[3] = glm::mat4(1.f);
		shapes[5].mats[3] = glm::mat4(1.f);
		shapes[6].mats[3] = glm::mat4(1.f);

		setta[0] += 2.f;
		setta[1] += 1.f;
		setta[2] += 0.5f;

		shapes[4].translate(3, 0.5f * glm::sin(glm::radians(setta[0])), 0.f, 0.5f * glm::cos(glm::radians(setta[0])));
		shapes[5].translate(3, 0.5f * glm::sin(glm::radians(setta[1])), 0.f, 0.5f * glm::cos(glm::radians(setta[1])));
		shapes[6].translate(3, 0.5f * glm::sin(glm::radians(setta[2])), 0.f, 0.5f * glm::cos(glm::radians(setta[2])));


		if (mode_y != 0) {
			for (auto& shape : shapes)
				shape.rotate(5, 1.f * mode_y, 0.f, 1.f, 0.f);

			for (auto& line : lines)
				line.rotate(5, 1.f * mode_y, 0.f, 1.f, 0.f);
		}

		if (mode_z != 0) {
			for (int i = 1; i < shapes.size(); ++i)
				shapes[i].rotate(5, 1.f * mode_z, 0.f, 0.f, 1.f);

			for (auto& line : lines)
				line.rotate(5, 1.f * mode_z, 0.f, 0.f, 1.f);
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