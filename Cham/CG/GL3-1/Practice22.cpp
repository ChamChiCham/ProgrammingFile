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

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> rand_obs(-0.9f, 0.9f);

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
	SShapeData* data = nullptr;


public:

	std::vector<glm::mat4> mats;
	glm::vec3 color = glm::vec3(1.f);


public:

	void updateBuffer()
	{
		if (data == nullptr) {
			std::cerr << "data isn't set" << std::endl;
			return;
		}

		if (buffer.VAO != 0) {
			glDeleteVertexArrays(1, &buffer.VAO);
			glDeleteBuffers(2, buffer.VBO);
			glDeleteBuffers(1, &buffer.EBO);
		}


		std::vector<GLfloat> VBO1;

		for (int i = 0; i < data->coords.size() / 3; ++i) {
			VBO1.push_back(color.x);
			VBO1.push_back(color.y);
			VBO1.push_back(color.z);
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

	void setColor(const glm::vec3& _color)
	{
		color = _color;
	}

	void setColor(const float _r, const float _g, const float _b)
	{
		setColor(glm::vec3(_r, _g, _b));
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


	bool mode_o = false;
	int mode_o_time = 0;
	bool walk = false;
	int walk_time = 120;
	glm::vec3 pos = glm::vec3(0.f);
	int dir = 2;
	float speed = 0.01f;

	int mode_j = 0;
	int mode_a = 0;

	glm::vec3 obs_pos[3] = { glm::vec3(rand_obs(gen), 0.1f, rand_obs(gen)),
							glm::vec3(rand_obs(gen), 0.1f, rand_obs(gen)),
							glm::vec3(rand_obs(gen), 0.1f, rand_obs(gen)) };

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

		shapes.push_back(CShape());
		shapes[0].setData(shape_data[SHAPE_SQUARE]);
		shapes[0].setColor(1.f, 0.f, 0.f);

		shapes.push_back(CShape());
		shapes[1].setData(shape_data[SHAPE_SQUARE]);
		shapes[1].setColor(0.f, 1.f, 0.f);
		shapes[1].rotate(0, 180.f, 1.f, 0.f, 0.f);
		shapes[1].translate(1, 0.f, 2.f, 0.f);


		shapes.push_back(CShape());
		shapes[2].setData(shape_data[SHAPE_SQUARE]);
		shapes[2].setColor(0.f, 0.f, 1.f);
		shapes[2].rotate(0, 90.f, 0.f, 0.f, 1.f);
		shapes[2].translate(1, 1.f, 1.0f, 0.f);

		shapes.push_back(CShape());
		shapes[3].setData(shape_data[SHAPE_SQUARE]);
		shapes[3].setColor(1.f, 1.f, 0.f);
		shapes[3].rotate(0, 270.f, 0.f, 0.f, 1.f);
		shapes[3].translate(1, -1.0f, 1.0f, 0.f);

		shapes.push_back(CShape());
		shapes[4].setData(shape_data[SHAPE_SQUARE]);
		shapes[4].setColor(1.f, 0.f, 1.f);
		shapes[4].rotate(0, 270.f, 1.f, 0.f, 0.f);
		shapes[4].translate(1, 0.0f, 1.f, -1.f);

		shapes.push_back(CShape());
		shapes[5].setData(shape_data[SHAPE_SQUARE]);
		shapes[5].setColor(0.f, 1.f, 1.f);
		shapes[5].scale(0, 0.5f, 1.f, 1.f);
		shapes[5].rotate(1, 90.f, 1.f, 0.f, 0.f);
		shapes[5].translate(2, 0.5f, 0.f, 0.f);

		shapes[5].translate(4, -1.f, 1.f, 1.f);

		shapes.push_back(CShape());
		shapes[6].setData(shape_data[SHAPE_SQUARE]);
		shapes[6].setColor(0.f, 1.f, 1.f);
		shapes[6].scale(0, 0.5f, 1.f, 1.f);
		shapes[6].rotate(1, 90.f, 1.f, 0.f, 0.f);
		shapes[6].translate(2, 0.5f, 0.f, 0.f);
		shapes[6].rotate(3, -180.f, 0.f, 1.f, 0.f);

		shapes[6].translate(4, 1.f, 1.f, 1.f);

		shapes.push_back(CShape());
		shapes[7].setData(shape_data[SHAPE_DICE]);
		shapes[7].setColor(0.5f, 0.5f, 0.f);
		shapes[7].scale(0, 0.1f, 0.1f, 0.1f);
		shapes[7].translate(1, 0.f, 0.3f, 0.f);


		shapes.push_back(CShape());
		shapes[8].setData(shape_data[SHAPE_DICE]);
		shapes[8].setColor(0.5f, 0.f, 0.5f);
		shapes[8].scale(0, 0.03f, 0.1f, 0.03f);
		shapes[8].translate(1, 0.f, -0.1f, 0.f);
		shapes[8].rotate(2, 60.f, 1.f, 0.f, 0.f);
		shapes[8].translate(3, -0.05f, 0.2f, 0.f);

		shapes.push_back(CShape());
		shapes[9].setData(shape_data[SHAPE_DICE]);
		shapes[9].setColor(0.5f, 0.f, 0.5f);
		shapes[9].scale(0, 0.03f, 0.1f, 0.03f);
		shapes[9].translate(1, 0.f, -0.1f, 0.f);
		shapes[9].rotate(2, -60.f, 1.f, 0.f, 0.f);
		shapes[9].translate(3, 0.05f, 0.2f, 0.f);


		shapes.push_back(CShape());
		shapes[10].setData(shape_data[SHAPE_DICE]);
		shapes[10].setColor(0.f, 0.5f, 0.5f);
		shapes[10].scale(0, 0.03f, 0.1f, 0.03f);
		shapes[10].translate(1, 0.f, -0.1f, 0.f);
		shapes[10].rotate(2, -60.f, 1.f, 0.f, 0.f);
		shapes[10].translate(3, -0.13f, 0.4f, 0.f);

		shapes.push_back(CShape());
		shapes[11].setData(shape_data[SHAPE_DICE]);
		shapes[11].setColor(0.f, 0.5f, 0.5f);
		shapes[11].scale(0, 0.03f, 0.1f, 0.03f);
		shapes[11].translate(1, 0.f, -0.1f, 0.f);
		shapes[11].rotate(2, 60.f, 1.f, 0.f, 0.f);
		shapes[11].translate(3, 0.13f, 0.4f, 0.f);


		shapes.push_back(CShape());
		shapes[12].setData(shape_data[SHAPE_DICE]);
		shapes[12].setColor(0.f, 0.5f, 0.f);
		shapes[12].scale(0, 0.08f, 0.08f, 0.08f);
		shapes[12].translate(1, 0.f, 0.48f, 0.f);

		shapes.push_back(CShape());
		shapes[13].setData(shape_data[SHAPE_DICE]);
		shapes[13].setColor(0.5f, 0.f, 0.f);
		shapes[13].scale(0, 0.02f, 0.02f, 0.01f);
		shapes[13].translate(1, -0.05f, 0.48f, 0.09f);

		shapes.push_back(CShape());
		shapes[14].setData(shape_data[SHAPE_DICE]);
		shapes[14].setColor(0.5f, 0.f, 0.f);
		shapes[14].scale(0, 0.02f, 0.02f, 0.01f);
		shapes[14].translate(1, 0.05f, 0.48f, 0.09f);

		shapes.push_back(CShape());
		shapes[15].setData(shape_data[SHAPE_DICE]);
		shapes[15].setColor(0.7f, 0.2f, 0.7f);
		shapes[15].scale(0, 0.1f, 0.1f, 0.1f);
		shapes[15].translate(1, obs_pos[0]);

		shapes.push_back(CShape());
		shapes[16].setData(shape_data[SHAPE_DICE]);
		shapes[16].setColor(0.7f, 0.2f, 0.7f);
		shapes[16].scale(0, 0.1f, 0.1f, 0.1f);
		shapes[16].translate(1, obs_pos[1]);

		shapes.push_back(CShape());
		shapes[17].setData(shape_data[SHAPE_DICE]);
		shapes[17].setColor(0.7f, 0.2f, 0.7f);
		shapes[17].scale(0, 0.1f, 0.1f, 0.1f);
		shapes[17].translate(1, obs_pos[2]);


		

		for (auto& shape : shapes)
			shape.updateBuffer();

		for (auto& line : lines)
			line.updateBuffer();


		// --
		// set view
		// --

		view.eye = glm::vec3(0.0f, 1.5f, 4.0f);
		view.at = glm::vec3(0.f, 1.f, 0.f);
		view.up = glm::vec3(0.f, 1.f, 0.f);
		proj = glm::perspective(glm::radians(60.f), 1.3f, 0.1f, 20.f);


		// --
		// explain
		// --

		std::cout << "- 무대 컨트롤" << std::endl;
		std::cout << "o/O: 무대의 앞면이 좌우로 열린다." << std::endl << std::endl;

		std::cout << "- 로봇 컨트롤" << std::endl;
		std::cout << "w/a/s/d: 로봇이 앞/뒤/좌/우 방향으로 이동 방향을 바꿔서 걷는다.\n 가장자리에 도달하면 로봇은 뒤로 돌아 다시 걷는다." << std::endl;
		std::cout << "+/-: 걷는 속도가 빨라지거나/느려진다.\n 속도가 바뀔 때 걷는 다리의 각도가 늘어나거나/줄어든다." << std::endl;
		std::cout << "j: 로봇이 제자리에서 점프한다.\n 장애물 위로 점프하여 올라가고 내려갈 수 있다." << std::endl;
		std::cout << "i: 모든 변환을 리셋" << std::endl << std::endl;

		std::cout << "- 시점 컨트롤" << std::endl;
		std::cout << "z/Z: 앞뒤로 이동" << std::endl;
		std::cout << "x/X: 좌우로 이동." << std::endl;
		std::cout << "y/Y: 카메라가 공전" << std::endl;


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

		for (const auto& shape : shapes)
			shape.draw(ShaderMgr.program, view, proj, mode);

		glutSwapBuffers();
	}

	void Mouse(const int _button, const int _state, const int _x, const int _y)
	{

	}

	void Keyboard(const unsigned char _key, const int _x, const int _y)
	{
		float d, s;

		switch (_key)
		{
		case 'o': case 'O':
			if (mode_o_time != 0)
				break;
			mode_o = !mode_o;
			mode_o_time = 240;
			break;
			
		case 'd':
			for (int i = 7; i <= 14; ++i) {
				shapes[i].mats[4] = glm::mat4(1.f);
				shapes[i].rotate(4, 90.f, 0.f, 1.f, 0.f);
			}
			dir = 0;
			break;

		case 'a':
			for (int i = 7; i <= 14; ++i) {
				shapes[i].mats[4] = glm::mat4(1.f);
				shapes[i].rotate(4, 270.f, 0.f, 1.f, 0.f);
			}
			dir = 1;
			break;

		case 's':
			for (int i = 7; i <= 14; ++i) {
				shapes[i].mats[4] = glm::mat4(1.f);
			}
			dir = 2;
			break;

		case 'w':
			for (int i = 7; i <= 14; ++i) {
				shapes[i].mats[4] = glm::mat4(1.f);
				shapes[i].rotate(4, 180.f, 0.f, 1.f, 0.f);
			}
			dir = 3;
			break;

		case '+':
			if (speed >= 0.03f) {
				break;
			}
			speed += 0.005f;
			walk = false;
			walk_time = 120;
			for (int i = 8; i <= 11; ++i) {
				shapes[i].mats[2] = glm::mat4(1.f);
			}
			shapes[8].rotate(2, 60.f + ((speed - 0.01f) * 240), 1.f, 0.f, 0.f);
			shapes[9].rotate(2, -60.f - ((speed - 0.01f) * 240), 1.f, 0.f, 0.f);
			shapes[10].rotate(2, -60.f - ((speed - 0.01f) * 240), 1.f, 0.f, 0.f);
			shapes[11].rotate(2, 60.f + ((speed - 0.01f) * 240), 1.f, 0.f, 0.f);
			break;

		case '-':
			if (speed <= 0.01f)
				break;
			speed -= 0.005f;
			walk_time = 120;
			walk = false;
			for (int i = 8; i <= 11; ++i) {
				shapes[i].mats[2] = glm::mat4(1.f);
			}
			shapes[8].rotate(2, 60.f + ((speed - 0.01f) * 240), 1.f, 0.f, 0.f);
			shapes[9].rotate(2, - 60.f - ((speed - 0.01f) * 240), 1.f, 0.f, 0.f);
			shapes[10].rotate(2, -60.f - ((speed - 0.01f) * 240), 1.f, 0.f, 0.f);
			shapes[11].rotate(2, 60.f + ((speed - 0.01f) * 240), 1.f, 0.f, 0.f);
			break;

		case 'j':
			if (mode_j > 0)
				break;
			mode_j = 100;
			break;



		case 'i':
			mode_j = 0;
			speed = 0.01f;
			for (int i = 7; i <= 14; ++i) {
				shapes[i].mats[4] = glm::mat4(1.f);
				shapes[i].mats[5] = glm::mat4(1.f);
			}
			dir = 2;
			mode_o_time = 0;
			mode_o = false;
			pos = glm::vec3(0.f);
			shapes[5].mats[3] = glm::mat4(1.f);
			shapes[6].mats[3] = glm::mat4(1.f);
			shapes[6].rotate(3, -180.f, 0.f, 1.f, 0.f);
			view.eye = glm::vec3(0.0f, 1.5f, 4.0f);
			view.at = glm::vec3(0.f, 1.f, 0.f);
			view.up = glm::vec3(0.f, 1.f, 0.f);

			break;




		case 'z':
			view.eye.z += 0.1f;
			view.at.z += 0.1f;
			break;

		case 'Z':
			view.eye.z -= 0.1f;
			view.at.z -= 0.1f;
			break;

		case 'x':
			view.eye.x += 0.1f;
			view.at.x += 0.1f;
			break;
		case 'X':
			view.eye.x -= 0.1f;
			view.at.x -= 0.1f;
			break;

		case 'y':
			if (mode_a == 0)
				mode_a = 1;
			else if (mode_a == 1)
				mode_a = 0;
			break;
		case 'Y':
			if (mode_a == 0)
				mode_a = -1;
			else if (mode_a == -1)
				mode_a = 0;
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
		if (mode_o_time > 0) {
			if (mode_o) {
				shapes[5].rotate(3, -0.5f, 0.f, 1.f, 0.f);
				shapes[6].rotate(3, 0.5f, 0.f, 1.f, 0.f);
			}
			else {
				shapes[5].rotate(3, 0.5f, 0.f, 1.f, 0.f);
				shapes[6].rotate(3, -0.5f, 0.f, 1.f, 0.f);
			}
			mode_o_time -= 1;
		}

		
		if (!walk) {
			shapes[8].rotate(2, -1.f - ((speed - 0.01f) * 20), 1.f, 0.f, 0.f);
			shapes[9].rotate(2, 1.f + ((speed - 0.01f) * 20), 1.f, 0.f, 0.f);
			shapes[10].rotate(2, 1.f + ((speed - 0.01f) * 20), 1.f, 0.f, 0.f);
			shapes[11].rotate(2, -1.f - ((speed - 0.01f) * 20), 1.f, 0.f, 0.f);
		}
		else {
			shapes[8].rotate(2, 1.f + ((speed - 0.01f) * 20), 1.f, 0.f, 0.f);
			shapes[9].rotate(2, -1.f - ((speed - 0.01f) * 20), 1.f, 0.f, 0.f);
			shapes[10].rotate(2, -1.f - ((speed - 0.01f) * 20), 1.f, 0.f, 0.f);
			shapes[11].rotate(2, 1.f + ((speed - 0.01f) * 20), 1.f, 0.f, 0.f);
		}

		walk_time--;
		if (walk_time == 0) {
			walk_time = 120;
			walk = !walk;
		}
		
		bool check = false;

		for (int i = 0; i < 3; ++i) {
			glm::vec3 tpos = pos;
			switch (dir)
			{
			case 0:
				tpos.x += speed;
				break;
			case 1:
				tpos.x -= speed;
				break;
			case 2:
				tpos.z += speed;
				break;
			case 3:
				tpos.z -= speed;
				break;
			default:
				break;
			}
			if (obs_pos[i].x - 0.1f <= tpos.x && tpos.x <= obs_pos[i].x + 0.1f &&
				obs_pos[i].y - 0.15f <= tpos.y && tpos.y <= obs_pos[i].y + 0.1f &&
				obs_pos[i].z - 0.1f <= tpos.z && tpos.z <= obs_pos[i].z + 0.1f) {
				check = true;
				break;
			}
		}

		if (!check) {
			switch (dir)
			{
			case 0:
				for (int i = 7; i <= 14; ++i)
					shapes[i].translate(5, speed, 0.f, 0.f);
				pos.x += speed;
				if (pos.x >= 0.9f) {
					for (int i = 7; i <= 14; ++i) {
						shapes[i].mats[4] = glm::mat4(1.f);
						shapes[i].rotate(4, 270.f, 0.f, 1.f, 0.f);
					}
					dir = 1;
				}
				break;
			case 1:
				for (int i = 7; i <= 14; ++i)
					shapes[i].translate(5, -speed, 0.0f, 0.f);
				pos.x -= speed;
				if (pos.x <= -0.9f) {
					for (int i = 7; i <= 14; ++i) {
						shapes[i].mats[4] = glm::mat4(1.f);
						shapes[i].rotate(4, 90.f, 0.f, 1.f, 0.f);
					}
					dir = 0;
				}
				break;
			case 2:
				for (int i = 7; i <= 14; ++i)
					shapes[i].translate(5, 0.f, 0.0f, speed);
				pos.z += speed;
				if (pos.z >= 0.9f) {
					for (int i = 7; i <= 14; ++i) {
						shapes[i].mats[4] = glm::mat4(1.f);
						shapes[i].rotate(4, 180.f, 0.f, 1.f, 0.f);
					}
					dir = 3;
				}
				break;
			case 3:
				for (int i = 7; i <= 14; ++i)
					shapes[i].translate(5, 0.f, 0.0f, -speed);
				pos.z -= speed;
				if (pos.z <= -0.9f) {
					for (int i = 7; i <= 14; ++i) {
						shapes[i].mats[4] = glm::mat4(1.f);
					}
					dir = 2;
				}
				break;
			default:
				break;
			}
		}

		if (mode_j) {
			if (mode_j > 50) {
				for (int i = 7; i <= 14; ++i) {
					shapes[i].translate(5, 0.f, 0.01f, 0.f);
				}
				pos.y += 0.01f;
			}
			else {
				bool check = false;
				for (int i = 0; i < 3; ++i) {
					glm::vec3 tpos = pos;
					tpos.y -= 0.01f;
					if (obs_pos[i].x - 0.1f <= tpos.x && tpos.x <= obs_pos[i].x + 0.1f &&
						obs_pos[i].y - 0.1f <= tpos.y && tpos.y <= obs_pos[i].y + 0.1f &&
						obs_pos[i].z - 0.1f <= tpos.z && tpos.z <= obs_pos[i].z + 0.1f) {
						check = true;
						mode_j++;
						break;
					}
				}
				if (!check) {
					for (int i = 7; i <= 14; ++i) {
						shapes[i].translate(5, 0.f, -0.01f, 0.f);
					}
					pos.y -= 0.01f;
				}
			}
			mode_j--;
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