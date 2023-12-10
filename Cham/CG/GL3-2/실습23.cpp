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

	void setData(const int _shape)
	{
		data = &CShapeDataMgr::getInst()->getData(_shape);
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



	// --
	// process member Variable
	// --

	std::vector<CShape> shapes;
	std::vector<CLine> lines;
	// 전역 변수 적는 곳
	int tog_c;
	int tog_o;
	float ball_x[5];
	float ball_y[5];
	int ball_way[5];

	int open_time;

	int LDown;
	float mouse_xPos, mouse_yPos;
	float mouse_start_xPos, mouse_start_yPos;
	float mouse_end_xPos, mouse_end_yPos;

	float d = 0.f;
	float s = 0.f;

	int rotation_rad;
	int box_time;
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

		shapes.push_back(CShape());
		shapes[0].setData(SHAPE_SQUARE);
		shapes[0].setColor(0.5f, 0.0f, 0.0f);
		shapes[0].scale(0, 2.0f, 2.0f, 2.0f);
		shapes[0].translate(1, 0.0f, 2.0f, 0.0f);

		shapes.push_back(CShape());
		shapes[1].setData(SHAPE_SQUARE);
		shapes[1].setColor(0.0f, 0.5f, 0.0f);
		shapes[1].scale(0, 2.0f, 2.0f, 2.0f);
		shapes[1].rotate(1, 180.f, 1.0, 0.0, 0.0);
		shapes[1].translate(2, 0.0f, -2.0f, 0.0f);

		shapes.push_back(CShape());
		shapes[2].setData(SHAPE_SQUARE);
		shapes[2].setColor(0.5f, 0.0f, 0.5f);
		shapes[2].scale(0, 2.0f, 2.0f, 2.0f);
		shapes[2].rotate(1, -90.f, 1.0, 0.0, 0.0);
		shapes[2].translate(2, 0.0f, 0.0f, -2.0f);

		shapes.push_back(CShape());
		shapes[3].setData(SHAPE_SQUARE);
		shapes[3].setColor(0.5f, 0.5f, 0.0f);
		shapes[3].scale(0, 2.0f, 2.0f, 2.0f);
		shapes[3].rotate(1, 180.f, 1.0, 0.0, 0.0);
		shapes[3].rotate(2, 90.f, 0.0, 0.0, 1.0);
		shapes[3].translate(3, 2.0f, 0.0f, 0.0f);

		shapes.push_back(CShape());
		shapes[4].setData(SHAPE_SQUARE);
		shapes[4].setColor(0.0f, 0.5f, 0.5f);
		shapes[4].scale(0, 2.0f, 2.0f, 2.0f);
		shapes[4].rotate(1, 90.f, 0.0, 0.0, 1.0);
		shapes[4].translate(2, -2.0f, 0.0f, 0.0f);

		for (int i = 5; i < 10; i++) {
			shapes.push_back(CShape());
			shapes[i].setData(SHAPE_SPHERE);
			shapes[i].setColor(0.0f, 0.0f, 1.0f);
			shapes[i].translate(0, 0.f, -0.05f, 0.f);
			shapes[i].scale(1, 0.092837782f, 0.092837782f, 0.092837782f);
			ball_x[i - 5] = GenerateRandomPos();
			ball_y[i - 5] = GenerateRandomPos();
			shapes[i].translate(2, ball_x[i - 5], ball_y[i - 5], 0.f);
		}

		shapes.push_back(CShape());
		shapes[10].setData(SHAPE_DICE);
		shapes[10].setColor(1.0f, 0.0f, 0.0f);
		shapes[10].scale(0, 0.2f, 0.2f, 0.2f);
		shapes[10].translate(1, 0.0f, -1.8f, 0.6f);

		shapes.push_back(CShape());
		shapes[11].setData(SHAPE_DICE);
		shapes[11].setColor(1.0f, 0.0f, 0.0f);
		shapes[11].scale(0, 0.3f, 0.3f, 0.3f);
		shapes[11].translate(1, 0.0f, -1.7f, 0.0f);

		shapes.push_back(CShape());
		shapes[12].setData(SHAPE_DICE);
		shapes[12].setColor(1.0f, 0.0f, 0.0f);
		shapes[12].scale(0, 0.3f, 0.4f, 0.4f);
		shapes[12].translate(1, 0.0f, -1.6f, -0.8f);

		for (auto& shape : shapes)
			shape.updateBuffer();

		for (auto& line : lines)
			line.updateBuffer();


		// --
		// set view
		// --

		view.eye = glm::vec3(0.0f, 0.5f, 8.0f);
		view.at = glm::vec3(0.f, 0.f, 0.0f);
		view.up = glm::vec3(0.f, 1.f, 0.f);
		proj = glm::perspective(glm::radians(60.f), 1.0f, 0.1f, 20.f);


		// --
		// explain
		// --
		ReadMe();

	}

	// 함수 만드는 곳
	void ReadMe() {
		std::cout << "   8/5: z축으로 양/음방향 이동" << std::endl;
		std::cout << "   4/6: x축으로 양/음방향 이동" << std::endl;
		std::cout << "   7/9: y축에 대하여 시계/반시계 회전" << std::endl;
		std::cout << "     b: 볼 생성/바닥 닫기" << std::endl;
		std::cout << "     o: 바닥 열기" << std::endl;
		std::cout << "     c: 컬링" << std::endl;
		std::cout << "마우스: z축에 대하여 왼쪽/오른쪽 회전" << std::endl;
	}
	float GenerateRandomPos() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(-1.7, 1.7);
		return dis(gen);
	}
	void windowsToOpenGLCoords(int x, int y, int windowWidth, int windowHeight, float& mouse_xPos, float& mouse_yPos) {
		// 윈도우 좌표계의 중심을 계산
		int windowCenterX = windowWidth / 2;
		int windowCenterY = windowHeight / 2;

		// OpenGL 좌표계로 변환하고 스케일링 적용
		mouse_xPos = static_cast<float>(x - windowCenterX) / windowCenterX;
		mouse_yPos = -static_cast<float>(y - windowCenterY) / windowCenterY; // y축 방향을 반대로
	}

	// --
	// define cb func
	// --

	void Display() const
	{
		glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glEnable(GL_DEPTH_TEST);
		if (tog_c == 1) {
			glFrontFace(GL_CW);
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}

		int mode = GL_TRIANGLES;

		for (const auto& shape : shapes)
			shape.draw(ShaderMgr.program, view, proj, mode);


		glutSwapBuffers();
	}

	void Mouse(const int _button, const int _state, const int _x, const int _y)
	{
		windowsToOpenGLCoords(_x, _y, WINDOW_SIZE_X, WINDOW_SIZE_Y, mouse_xPos, mouse_yPos);
		if (_button == GLUT_LEFT_BUTTON && _state == GLUT_DOWN) {
			LDown = 1;
			mouse_start_xPos = mouse_xPos;
			mouse_start_yPos = mouse_yPos;
		}
		else if (_button == GLUT_LEFT_BUTTON && _state == GLUT_UP) {
			LDown = 0;
		}

	}

	void Keyboard(const unsigned char _key, const int _x, const int _y)
	{
		switch (_key)
		{
		case 'c':
			tog_c = !tog_c;
			break;
		case 'o':
			tog_o = 1;
			break;
		case 'b':
			view.up = glm::vec3(0.0f, 1.0f, 0.0f);
			shapes[1].clearMatrix(3);
			tog_o = 0;
			open_time = 0;
			rotation_rad = 0;
			box_time = 0;
			for (int i = 5; i < 10; ++i) {
				shapes[i].clearMatrix(3);
				shapes[i].clearMatrix(2);
				ball_way[i - 5] = 0;
				ball_x[i - 5] = GenerateRandomPos();
				ball_y[i - 5] = GenerateRandomPos();
				shapes[i].translate(2, ball_x[i - 5], ball_y[i - 5], 0.f);
			}
			for (int i = 10; i < 13; i++) {
				shapes[i].clearMatrix(2);
			}

			break;
		case '1':
			d = sqrtf(powf(view.eye.y - view.at.y, 2) + powf(view.eye.z - view.at.z, 2));
			s = atan2f(view.eye.z - view.at.z, view.eye.y - view.at.y);
			view.eye.y = view.at.y + d * cos(s + glm::radians(2.f));
			view.eye.z = view.at.z + d * sin(s + glm::radians(2.f));
			break;
		case '3':
			d = sqrtf(powf(view.eye.y - view.at.y, 2) + powf(view.eye.z - view.at.z, 2));
			s = atan2f(view.eye.z - view.at.z, view.eye.y - view.at.y);
			view.eye.y = view.at.y + d * cos(s + glm::radians(-2.f));
			view.eye.z = view.at.z + d * sin(s + glm::radians(-2.f));
			break;
		case '4':
			view.eye.x -= 0.1;
			view.at.x -= 0.1;
			break;
		case '6':
			view.eye.x += 0.1;
			view.at.x += 0.1;
			break;
		case '8':
			view.eye.z -= 0.1;
			view.at.z -= 0.1;
			break;
		case '5':
			view.eye.z += 0.1;
			view.at.z += 0.1;
			break;
		case '7':
			d = sqrtf(powf(view.eye.x - view.at.x, 2) + powf(view.eye.z - view.at.z, 2));
			s = atan2f(view.eye.z - view.at.z, view.eye.x - view.at.x);
			view.eye.x = view.at.x + d * cos(s + glm::radians(2.f));
			view.eye.z = view.at.z + d * sin(s + glm::radians(2.f));
			break;
		case '9':
			d = sqrtf(powf(view.eye.x - view.at.x, 2) + powf(view.eye.z - view.at.z, 2));
			s = atan2f(view.eye.z - view.at.z, view.eye.x - view.at.x);
			view.eye.x = view.at.x + d * cos(s + glm::radians(-2.f));
			view.eye.z = view.at.z + d * sin(s + glm::radians(-2.f));
		default:
			break;
		}
	}

	void Motion(const int _x, const int _y)
	{
		windowsToOpenGLCoords(_x, _y, WINDOW_SIZE_X, WINDOW_SIZE_Y, mouse_xPos, mouse_yPos);
		if (LDown) {
			mouse_end_xPos = mouse_xPos;
			mouse_end_yPos = mouse_yPos;
			if (mouse_end_xPos - mouse_start_xPos > 0) {
				if (rotation_rad < 60) {
					glm::mat4 M_Rotation = glm::mat4(1.0f);
					M_Rotation = glm::rotate(M_Rotation, glm::radians(1.0f), glm::vec3(0.0, 0.0, 1.0));
					view.up = glm::vec3(M_Rotation * glm::vec4(view.up, 1.0));
					rotation_rad++;
				}
			}
			else if (mouse_end_xPos - mouse_start_xPos < 0) {
				if (rotation_rad > -60) {
					glm::mat4 M_Rotation = glm::mat4(1.0f);
					M_Rotation = glm::rotate(M_Rotation, glm::radians(-1.0f), glm::vec3(0.0, 0.0, 1.0));
					view.up = glm::vec3(M_Rotation * glm::vec4(view.up, 1.0));
					rotation_rad--;
				}
			}

		}
	}

	// --
	// process func
	// --


	void updateState()
	{
		for (int i = 5; i < 10; i++) {
			if (ball_way[i - 5] == 0) {
				shapes[i].translate(3, -0.05f, 0.05f, 0.f);
				ball_x[i - 5] += -0.05;
				ball_y[i - 5] += 0.05;
			}
			else if (ball_way[i - 5] == 1) {
				shapes[i].translate(3, -0.05f, -0.05f, 0.f);
				ball_x[i - 5] += -0.05;
				ball_y[i - 5] += -0.05;
			}
			else if (ball_way[i - 5] == 2) {
				shapes[i].translate(3, 0.05f, -0.05f, 0.f);
				ball_x[i - 5] += 0.05;
				ball_y[i - 5] += -0.05;
			}
			else if (ball_way[i - 5] == 3) {
				shapes[i].translate(3, 0.05f, 0.05f, 0.f);
				ball_x[i - 5] += 0.05;
				ball_y[i - 5] += 0.05;
			}
		}
		for (int i = 5; i < 10; i++) {
			if (ball_way[i - 5] == 0 && (ball_y[i - 5] > 1.8 || ball_x[i - 5] < -1.8)) {
				ball_way[i - 5] = 1;
			}
			else if (ball_way[i - 5] == 1 && ball_x[i - 5] < -1.8) {
				ball_way[i - 5] = 2;
			}
			else if (ball_way[i - 5] == 2 && ball_y[i - 5] < -1.8) {
				if (tog_o == 0) {
					ball_way[i - 5] = 3;
				}
			}
			else if (ball_way[i - 5] == 3 && ball_x[i - 5] > 1.8) {
				ball_way[i - 5] = 0;
			}
		}
		if (tog_o) {
			open_time++;
			if (open_time < 90) {
				shapes[1].rotate(3, 0.0, -2.0, -2.0, 1.0f, 1.0, 0.0, 0.0);
			}
		}
		for (int i = 10; i < 13; i++) {
			if (tog_o) {
				if (rotation_rad > 0) {
					shapes[i].translate(2, 0.05f, -0.05f, 0.f);
				}
				else if (rotation_rad < 0) {
					shapes[i].translate(2, -0.05f, -0.05f, 0.f);
				}
				else if (rotation_rad == 0) {
					shapes[i].translate(2, 0.f, -0.05f, 0.f);
				}
			}
			else if (rotation_rad > 0) {
				if (box_time < 100) {
					shapes[i].translate(2, 0.05f, 0.f, 0.f);
					box_time++;
				}
			}
			else if (rotation_rad < 0) {
				if (box_time > -100) {
					shapes[i].translate(2, -0.05f, 0.f, 0.f);
					box_time--;
				}
			}
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