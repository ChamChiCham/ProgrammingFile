#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <array>
#include <list>

#include "CShaderMgr.h"
#include "Define.h"
#include "Struct.h"
#include "CShapeDataMgr.h"


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
	glm::vec3 color = glm::vec3(1.f);


public:

	std::vector<glm::mat4> mats;


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

		

		glGenBuffers(2, buffer.VBO);

		glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, data->coords.size() * sizeof(GLfloat), data->coords.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, data->normals.size() * sizeof(GLfloat), data->normals.data(), GL_STATIC_DRAW);

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

	void setData(const int _shape)
	{
		data = &CShapeDataMgr::getInst()->getData(_shape);
	}

	void draw(const unsigned int _program, const SView& _view, const glm::mat4& _proj, const int _mode, const SLight& _light) const
	{
		glUseProgram(_program);

		// light
		unsigned int lightPosLocation = glGetUniformLocation(_program, "lightPos");
		glUniform3f(lightPosLocation, _light.x, _light.y, _light.z);

		unsigned int lightColorLocation = glGetUniformLocation(_program, "lightColor");
		glUniform3f(lightColorLocation, _light.r, _light.g, _light.b);

		unsigned int objColorLocation = glGetUniformLocation(_program, "objectColor");
		glUniform3f(objColorLocation, color.r, color.g, color.b);

		unsigned int viewPosLocation = glGetUniformLocation(_program, "viewPos");
		glUniform3f(viewPosLocation, _view.eye.x, _view.eye.y, _view.eye.z);

		glBindVertexArray(buffer.VAO);

		glm::mat4 MM = glm::mat4(1.f);
		for (auto& mat : mats) {
			MM = mat * MM;
		}

		glm::mat4 MV = glm::lookAt(_view.eye, _view.at, _view.up);
		glm::mat4 MP = _proj;

		// uniform matrix 
		unsigned int transformLocation = glGetUniformLocation(_program, "model");
		unsigned int viewLocation = glGetUniformLocation(_program, "view");
		unsigned int projectionLocation = glGetUniformLocation(_program, "projection");

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

	void scale(const int _idx, const glm::vec3 _vec)
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

	void rotate(const int _idx, const float _deg, const glm::vec3 _vec)
	{
		while (mats.size() <= _idx) {
			mats.push_back(glm::mat4(1.f));
		}

		mats[_idx] = glm::rotate(mats[_idx], glm::radians(_deg), _vec);
	}

	void rotate(const int _idx, const float _x, const float _y, const float _z, const float _deg,
		const float _fir, const float _sec, const float _thi)
	{
		rotate(_idx, glm::vec3(_x, _y, _z), _deg, glm::vec3(_fir, _sec, _thi));
	}

	void rotate(const int _idx, const glm::vec3 _axis, const float _deg, const glm::vec3 _vec)
	{
		while (mats.size() <= _idx) {
			mats.push_back(glm::mat4(1.f));
		}

		glm::mat4 m = glm::mat4(1.f);
		m = glm::translate(glm::mat4(1.f), -_axis) * m;
		m = glm::rotate(glm::mat4(1.f), glm::radians(_deg), _vec) * m;
		m = glm::translate(glm::mat4(1.f), _axis) * m;
		mats[_idx] = m * mats[_idx];
	}


	void translate(const int _idx, const float _fir, const float _sec, const float _thi)
	{
		translate(_idx, glm::vec3(_fir, _sec, _thi));
	}

	void translate(const int _idx, const glm::vec3 _vec)
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

	void clearMatrix(const int _idx)
	{
		if (mats.size() <= _idx)
			return;
		mats[_idx] = glm::mat4(1.f);
	}
};

class CLine {

private:
	SBuffer buffer;
	glm::vec3 color = glm::vec3{ 1.f, 1.f, 1.f };

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

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> rand_pos(-2.f, 2.f);
std::uniform_real_distribution<float> rand_speed(1.f, 4.f);

class HW28Snow
{
private:
	CShape shape;
	float speed = 1.f;
	glm::vec3 pos = { 0.f, 5.f, 0.f };

public:

	void init()
	{
		shape.setData(SHAPE_DICE);
		shape.setColor(1.f, 1.f, 1.f);
		shape.scale(0, 0.03f, 0.03f, 0.03f);
		pos.x = rand_pos(gen);
		pos.z = rand_pos(gen);
		speed = rand_speed(gen);
		shape.updateBuffer();
	}

	void draw(const unsigned int _program, const SView& _view, const glm::mat4& _proj, const int _mode, const SLight& _light)
	{
		shape.translate(1, pos);
		shape.draw(_program, _view, _proj, _mode, _light);
		shape.clearMatrix(shape.mats.size() - 1);
	}

	void update()
	{
		pos.y -= 0.01f * speed;
		if (pos.y < 0.f) {
			pos.x = rand_pos(gen);
			pos.z = rand_pos(gen);
			speed = rand_speed(gen);
			pos.y = 5.f;
		}
	}
};


class HW28Pyramid
{
private:
	CShape shape;

public:
	glm::vec3 pos = { 0.f, 0.f, 0.f };
	float size = 1.f;


public:

	void init(const float _size, const glm::vec3& _pos)
	{
		shape.setData(SHAPE_SQUARE_PRAMID);
		shape.setColor(0.f, 1.f, 1.f);
		shape.scale(0, _size, _size, _size);
		shape.translate(1, _pos);
		pos = _pos;
		size = _size;
		shape.updateBuffer();

	}

	void draw(const unsigned int _program, const SView& _view, const glm::mat4& _proj, const int _mode, const SLight& _light)
	{
		shape.draw(_program, _view, _proj, _mode, _light);
	}


};


class HW28Sierpinski
{
private:
	std::list<HW28Pyramid> pyramids;
	int level = 0;

public:

	void init()
	{
		HW28Pyramid pyramid;
		pyramid.init(1.f, glm::vec3(0.f, 1.f, 0.f));
		pyramids.push_back(pyramid);
	}

	void draw(const unsigned int _program, const SView& _view, const glm::mat4& _proj, const int _mode, const SLight& _light)
	{
		for (auto& pyramid : pyramids)
			pyramid.draw(_program, _view, _proj, _mode, _light);
	}

	void next()
	{
		if (level >= 4) {
			level = 0;
			pyramids.clear();
			init();
		}
		else {
			level++;
			int size = pyramids.size();
			for (int i = 0; i < size; ++i) {
				HW28Pyramid pyramid1;
				HW28Pyramid pyramid2;
				HW28Pyramid pyramid3;
				HW28Pyramid pyramid4;
				HW28Pyramid pyramid5;

				HW28Pyramid& pyramid = pyramids.front();
				float p_size = pyramid.size;
				glm::vec3 p_pos = pyramid.pos;

				pyramid1.init(p_size / 2.f, glm::vec3(p_pos.x, p_pos.y + p_size / 2.f, p_pos.z));
				pyramid2.init(p_size / 2.f, glm::vec3(p_pos.x + p_size / 2.f, p_pos.y - p_size / 2.f, p_pos.z + p_size / 2.f));
				pyramid3.init(p_size / 2.f, glm::vec3(p_pos.x - p_size / 2.f, p_pos.y - p_size / 2.f, p_pos.z + p_size / 2.f));
				pyramid4.init(p_size / 2.f, glm::vec3(p_pos.x + p_size / 2.f, p_pos.y - p_size / 2.f, p_pos.z - p_size / 2.f));
				pyramid5.init(p_size / 2.f, glm::vec3(p_pos.x - p_size / 2.f, p_pos.y - p_size / 2.f, p_pos.z - p_size / 2.f));

				pyramids.push_back(pyramid1);
				pyramids.push_back(pyramid2);
				pyramids.push_back(pyramid3);
				pyramids.push_back(pyramid4);
				pyramids.push_back(pyramid5);

				pyramids.pop_front();
			}
		}
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
	glm::vec3 bg_color = glm::vec3(0.f, 0.f, 0.f);

	// shape data list



	// --
	// process member Variable
	// --

	std::vector<CShape> shapes;
	std::vector<CLine> lines;

	SLight light;

	std::array<HW28Snow, 50> snows;

	HW28Sierpinski sierpinski;

	bool mode_r = false;
	bool mode_s = true;
	bool mode_m = true;
	int mode_c = 0;
	int mode_p = 0;

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
		ShaderMgr.makeProgram1("Vertex1.glsl", "Fragment1.glsl");

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

		light.pos = { 0.f, 4.f, 4.f };

		shapes.push_back(CShape());
		shapes[0].setData(SHAPE_SPHERE);
		shapes[0].setColor(1.f, 0.f, 0.f);
		shapes[0].translate(0, 0.f, -0.05f, 0.f);
		shapes[0].scale(1, 0.392837782f, 0.392837782f, 0.392837782f);
		shapes[0].scale(1, 0.5f, 0.5f, 0.5f);
		shapes[0].translate(2, -3.0f, 3.f, 0.f);

		shapes.push_back(CShape());
		shapes[1].setData(SHAPE_SPHERE);
		shapes[1].setColor(0.f, 1.f, 0.f);
		shapes[1].translate(0, 0.f, -0.05f, 0.f);
		shapes[1].scale(1, 0.392837782f, 0.392837782f, 0.392837782f);
		shapes[1].scale(1, 0.5f, 0.5f, 0.5f);
		shapes[1].translate(2, 3.0f * sqrtf(2.f), 0.f, 0.f);

		shapes.push_back(CShape());
		shapes[2].setData(SHAPE_SPHERE);
		shapes[2].setColor(0.f, 0.f, 1.f);
		shapes[2].translate(0, 0.f, -0.05f, 0.f);
		shapes[2].scale(1, 0.392837782f, 0.392837782f, 0.392837782f);
		shapes[2].scale(1, 0.5f, 0.5f, 0.5f);
		shapes[2].translate(2, 3.0f, 3.f, 0.f);
		

		shapes.push_back(CShape());
		shapes[3].setData(SHAPE_DICE);
		shapes[3].setColor(0.3f, 0.3f, 0.3f);
		shapes[3].scale(0, 0.3f, 0.3f, 0.3f);
		shapes[3].translate(1, 0.0f, 4.f, 4.f);

		shapes.push_back(CShape());
		shapes[4].setData(SHAPE_SQUARE);
		shapes[4].setColor(0.5f, 0.5f, 0.5f);
		shapes[4].scale(0, 2.f, 2.f, 2.f);

		lines.push_back(CLine());
		lines.push_back(CLine());
		lines.push_back(CLine());

		float angle = 0;

		for (int i = 0; i < 40; ++i) {
			angle += 9.0f;
			lines[0].addCoord(glm::cos(glm::radians(angle)) * 3.f * sqrtf(2.f), 0.f, glm::sin(glm::radians(angle)) * 3.f * sqrtf(2.f));
			lines[1].addCoord(glm::cos(glm::radians(angle)) * 3.f * sqrtf(2.f), 0.f, glm::sin(glm::radians(angle)) * 3.f * sqrtf(2.f));
			lines[2].addCoord(glm::cos(glm::radians(angle)) * 3.f * sqrtf(2.f), 0.f, glm::sin(glm::radians(angle)) * 3.f * sqrtf(2.f));
		}

		lines[1].rotate(0, 45.f, 0.f, 0.f, 1.f);
		lines[2].rotate(0, -45.f, 0.f, 0.f, 1.f);
		

		for (auto& snow : snows)
			snow.init();

		sierpinski.init();


		for (auto& shape : shapes)
			shape.updateBuffer();

		for (auto& line : lines)
			line.updateBuffer();


		// --
		// set view
		// --

		view.eye = glm::vec3(0.0f, 6.0f, 8.0f);
		view.at = glm::vec3(0.f, 0.f, 0.0f);
		view.up = glm::vec3(0.f, 1.f, 0.f);

		proj = glm::perspective(glm::radians(60.f), 1.0f, 0.1f, 20.f);


		// --
		// explain
		// --

		std::cout << "s: 하늘에서 눈이 내린다" << std::endl;
		std::cout << "0: 시어핀스키 프렉탈 삼각형의 단계가 늘어난다." << std::endl;
		std::cout << "c: 조명 색을 다른 색으로 바뀌도록 한다.  3종류의 다른 색을 적용해본다." << std::endl;
		std::cout << "p: 조명을 (1, 0, 0) (-1, 0, 0) (0, 0, 1) (0, 0, -1) 쪽의 위치로 옮긴다." << std::endl;
		std::cout << "r: 조명이 화면 중앙의 y축에 대하여 공전한다/멈춘다" << std::endl;
		std::cout << "n/f: 조명이 가까워지기/멀어지기" << std::endl;
		std::cout << "+/-: 조명의 세기 높아지기/낮춰지기" << std::endl;
		std::cout << "q: 프로그램 종료" << std::endl;

	}

	// --
	// define cb func
	// --

	void Display()
	{
		glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int mode = GL_TRIANGLES;

		glEnable(GL_DEPTH_TEST);

		for (const auto& shape : shapes)
			shape.draw(ShaderMgr.program, view, proj, mode, light);

		for (const auto& line : lines)
			line.draw(ShaderMgr.program1, view, proj, GL_LINE_LOOP);

		for (auto& snow : snows)
			snow.draw(ShaderMgr.program, view, proj, mode, light);

		sierpinski.draw(ShaderMgr.program, view, proj, mode, light);


		glutSwapBuffers();
	}

	void Mouse(const int _button, const int _state, const int _x, const int _y)
	{

	}

	void Keyboard(const unsigned char _key, const int _x, const int _y)
	{
		float dist = 0.f;
		switch (_key)
		{
		case 'r':
			mode_r = !mode_r;
			break;

		case '+':
			if (light.r >= 1.f)
				break;
			light.r += 0.1f;
			light.g += 0.1f;
			light.b += 0.1f;
			break;

		case '-':
			if (light.r <= 0.f)
				break;

			light.r -= 0.1f;
			light.g -= 0.1f;
			light.b -= 0.1f;
			break;

		case 'q':
			PostQuitMessage(0);
			break;

		case 'n':
			light.x *= 0.8f;
			light.y *= 0.8f;
			light.z *= 0.8f;
			shapes[3].clearMatrix(1);
			shapes[3].translate(1, light.pos);
			break;

		case 'f':
			light.x *= 1.25f;
			light.y *= 1.25f;
			light.z *= 1.25f;
			shapes[3].clearMatrix(1);
			shapes[3].translate(1, light.pos);
			break;

		case 's':
			mode_s = !mode_s;
			break;

		case '0':
			sierpinski.next();
			break;

		case 'm':
			if (mode_m) {
				mode_m = false;
				light.color = glm::vec3(0.f, 0.f, 0.f);
			}
			else {
				mode_m = true;
				light.color = glm::vec3(1.f, 1.f, 1.f);
			}
			break;

		case 'c':
			if (!mode_m)
				break;

			switch (mode_c)
			{
			case 0:
				mode_c++;
				light.color = glm::vec3(1.f, 0.f, 0.f);
				break;
			case 1:
				mode_c++;
				light.color = glm::vec3(0.f, 1.f, 0.f);
				break;

			case 2:
				mode_c++;
				light.color = glm::vec3(0.f, 0.f, 1.f);
				break;
			case 3:
				mode_c = 0;
				light.color = glm::vec3(1.f, 1.f, 1.f);
				break;

			default:
				break;
			}
			break;

		case 'p':
			dist = sqrtf(powf(light.pos.x, 2.f) + powf(light.pos.z, 2.f));
			light.pos.x = cosf(glm::pi<float>() * mode_p / 2.f) * dist;
			light.pos.z = sinf(glm::pi<float>() * mode_p / 2.f) * dist;
			shapes[3].clearMatrix(1);
			shapes[3].translate(1, light.pos);
			mode_p++;
			if (mode_p == 4)
				mode_p = 0;
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
		shapes[0].rotate(3, 1.f, 1.f, 1.f, 0.f);
		shapes[1].rotate(3, 1.f, 0.f, 1.f, 0.f);
		shapes[2].rotate(3, 1.f, -1.f, 1.f, 0.f);

		if (mode_r) {
			float angle = glm::radians(-1.f);
			float newX = light.x * cos(angle) - light.z * sin(angle);
			float newZ = light.x * sin(angle) + light.z * cos(angle);
			light.x = newX;
			light.z = newZ;
			shapes[3].clearMatrix(1);
			shapes[3].translate(1, light.pos);
		}

		if (mode_s) {
			for (auto& snow : snows)
				snow.update();
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

	// render (Display 함수 호출)
	glutPostRedisplay();

	glutTimerFunc(10, GameLoop, 1);
}

int main(int argc, char** argv)
{
	Window.init(argc, argv);
	Window.run();
}