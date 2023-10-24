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


struct SView
{
	glm::vec3 eye = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 at = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 up = glm::vec3(0.f, 0.f, 0.f);
};


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
	std::vector<glm::mat4> mats;
	SShapeData* data;


public:

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

	void draw(const unsigned int _program, const SView& _view, const glm::mat4& _proj) const
	{
		glUseProgram(_program);
		glBindVertexArray(buffer.VAO);
		
		glm::mat4 MM = glm::mat4(1.f);
		for (auto& mat: mats) {
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
		glDrawElements(GL_TRIANGLES, data->indices.size(), GL_UNSIGNED_INT, 0);
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

	// bg color
	glm::vec3 bg_color = glm::vec3(1.f, 1.f, 1.f);

	// shape data list
	std::vector<SShapeData> shape_data;



	// --
	// process member Variable
	// --

	std::vector<CShape> shapes;

	bool mode_depth = true;
	bool mode_rotate_y = false;
	bool mode_p = false;
	bool mode_t = false;
	int mode_f_value = 0;
	bool mode_f = false;
	int mode_s_value = 0;
	bool mode_s = false;
	int mode_b_value = 0;
	bool mode_b = false;
	int mode_o_value = 0;
	bool mode_o = false;
	int mode_r_value = 0;
	bool mode_r = false;

	bool mode = false;

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



		// --
		// make shapes
		// --
		shapes.push_back(CShape());
		shapes[0].setData(shape_data[SHAPE_LINE]);
		for (int i = 0; i < 8; ++i)
			shapes[0].colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));


		shapes.push_back(CShape());
		shapes[1].setData(shape_data[SHAPE_LINE]);
		for (int i = 0; i < 8; ++i)
			shapes[1].colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		shapes[1].rotate(0, 90.f, 0.f, 0.f, 1.f);

		shapes.push_back(CShape());
		shapes[2].setData(shape_data[SHAPE_LINE]);
		for (int i = 0; i < 8; ++i)
			shapes[2].colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		shapes[2].rotate(0, 90.f, 0.f, 1.f, 0.f);

		makeDice();
		//makeTetra();


		for (auto& shape : shapes)
			shape.updateBuffer();


		// --
		// set view
		// --

		view.eye = glm::vec3(1.0f, 1.0f, 1.0f);
		view.at = glm::vec3(0.f, 0.f, 0.f);
		view.up = glm::vec3(0.f, 1.f, 0.f);

		std::cout << "q: 육면체로 변경" << std::endl;
		std::cout << "e: 사면체로 변경" << std::endl;
		std::cout << "h: 은면제거 설정/해제" << std::endl;
		std::cout << "y: y축에 대하여 자전한다/멈춘다" << std::endl;
		std::cout << "p: 직각 투영/원근 투영" << std::endl << std::endl;

		std::cout << "육면체 애니메이션" << std::endl;
		std::cout << "t: 육면체의 윗면 애니메이션 시작/정지" << std::endl;
		std::cout << "f: 육면체의 앞면을 연다/앞면을 닫는다" << std::endl;
		std::cout << "s: 육면체의 옆면을 연다/닫는다." << std::endl;
		std::cout << "b: 육면체의 뒷면을 연다/닫는다." << std::endl << std::endl;

		std::cout << "사각뿔 애니메이션" << std::endl;
		std::cout << "o: 사각뿔의  모든 면들이 함께 열린다/닫는다." << std::endl;
		std::cout << "R: 사각뿔의 각 면이 한 개씩 열린다/닫는다." << std::endl;

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

		if (mode_depth)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		glm::mat4 proj = glm::mat4(1.f);

		if (!mode_p)
			proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 4.0f);
		else
			proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 10.0f);


		for (auto& shape : shapes)
			shape.draw(ShaderMgr.program, view, proj);


		glutSwapBuffers();
	}

	void Mouse(const int _button, const int _state, const int _x, const int _y)
	{

	}

	void Keyboard(const unsigned char _key, const int _x, const int _y)
	{
		
		switch (_key)
		{
		case 'h':
			mode_depth = !mode_depth;
			break;
		case 'y':
			mode_rotate_y = !mode_rotate_y;
			break;
		case 't':
			if (mode)
				break;
			mode_t = !mode_t;
			break;
		case 'f':
			if (mode)
				break;
			if (mode_f_value != 0)
				break;
			mode_f = !mode_f;
			mode_f_value = 90;
			break;
		case 's':
			if (mode)
				break;
			if (mode_s_value != 0)
				break;
			mode_s = !mode_s;
			mode_s_value = 90;
			break;
		case 'b':
			if (mode)
				break;
			if (mode_b_value != 0)
				break;
			mode_b = !mode_b;
			mode_b_value = 90;
			break;
		case 'o':
			if (!mode)
				break;
			if (mode_o_value != 0)
				break;
			mode_o = !mode_o;
			mode_o_value = 90;
			break;

		case 'r':
			if (!mode)
				break;
			if (mode_r_value != 0)
				break;
			mode_r = !mode_r;
			mode_r_value = 360;
			break;

		case 'q':
			makeDice();
			for (auto& shape : shapes)
				shape.updateBuffer();
			mode = false;
			break;

		case 'e':
			makeTetra();
			for (auto& shape : shapes)
				shape.updateBuffer();
			mode = true;
			break;

		case 'p':
			if (mode_p) {
				mode_p = false;
				view.eye.z -= 3.0f;
			}
			else {
				mode_p = true;
				view.eye.z += 3.0f;
			}
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

	void makeDice()
	{
		int size = shapes.size();
		for (int i = 0; i < size - 3; ++i) {
			shapes.pop_back();
		}

		for (int i = 0; i < 6; ++i)
			shapes.push_back(CShape());

		// 위
		shapes[3].setData(shape_data[SHAPE_SQUARE]);
		for (int i = 0; i < 4; ++i)
			shapes[3].colors.push_back(glm::vec3(1.f, 0.f, 0.f));
		shapes[3].scale(0, glm::vec3(0.3f, 0.3f, 0.3f));
		shapes[3].translate(2, glm::vec3(0.f, 0.3f, 0.f));

		// 아래
		shapes[4].setData(shape_data[SHAPE_SQUARE]);
		for (int i = 0; i < 4; ++i)
			shapes[4].colors.push_back(glm::vec3(0.f, 1.f, 0.f));
		shapes[4].scale(0, glm::vec3(0.3f, 0.3f, 0.3f));
		shapes[4].rotate(1, 180.f, glm::vec3(0.f, 0.f, 1.f));
		shapes[4].translate(2, glm::vec3(0.f, -0.3f, 0.f));

		// 오
		shapes[5].setData(shape_data[SHAPE_SQUARE]);
		for (int i = 0; i < 4; ++i)
			shapes[5].colors.push_back(glm::vec3(0.f, 0.f, 1.f));
		shapes[5].scale(0, glm::vec3(0.3f, 0.3f, 0.3f));
		shapes[5].rotate(1, 90.f, glm::vec3(0.f, 0.f, 1.f));
		shapes[5].translate(2, 0.f, 0.3f, 0.f);
		shapes[5].rotate(3, 0.f, 0.f, 0.f, 1.f);
		shapes[5].translate(4, glm::vec3(0.3f, -0.3f, 0.f));


		// 왼

		shapes[6].setData(shape_data[SHAPE_SQUARE]);
		for (int i = 0; i < 4; ++i)
			shapes[6].colors.push_back(glm::vec3(1.f, 1.f, 0.f));
		shapes[6].scale(0, glm::vec3(0.3f, 0.3f, 0.3f));
		shapes[6].rotate(1, -90.f, glm::vec3(0.f, 0.f, 1.f));
		shapes[6].translate(2, glm::vec3(-0.3f, 0.f, 0.f));

		// 앞
		shapes[7].setData(shape_data[SHAPE_SQUARE]);
		for (int i = 0; i < 4; ++i)
			shapes[7].colors.push_back(glm::vec3(0.f, 1.f, 1.f));
		shapes[7].scale(0, glm::vec3(0.3f, 0.3f, 0.3f));
		shapes[7].rotate(1, 90.f, glm::vec3(1.f, 0.f, 0.f));
		shapes[7].translate(2, glm::vec3(0.f, 0.f, 0.3f));

		// 뒤
		shapes[8].setData(shape_data[SHAPE_SQUARE]);
		for (int i = 0; i < 4; ++i)
			shapes[8].colors.push_back(glm::vec3(1.f, 0.f, 1.f));
		shapes[8].scale(0, glm::vec3(0.3f, 0.3f, 0.3f));
		shapes[8].rotate(1, -90.f, glm::vec3(1.f, 0.f, 0.f));
		shapes[8].translate(2, glm::vec3(0.f, 0.f, -0.3f));

	}

	void makeTetra()
	{
		int size = shapes.size();
		for (int i = 0; i < size - 3; ++i) {
			shapes.pop_back();
		}

		for (int i = 0; i < 5; ++i)
			shapes.push_back(CShape());

		shapes[3].setData(shape_data[SHAPE_SQUARE]);
		shapes[3].scale(0, 0.3f, 0.3f, 0.3f);
		for (int i = 0; i < 4; ++i)
			shapes[3].colors.push_back(glm::vec3(1.f, 0.f, 0.f));

		// 정면
		shapes[4].setData(shape_data[SHAPE_TRIANGLE]);
		shapes[4].scale(0, 0.3f, 0.3f, 0.3f);
		shapes[4].rotate(1, 0.f, 0.f, 1.f, 0.f);
		shapes[4].translate(2, 0.f, 0.f, -0.3f);
		shapes[4].rotate(3, 60.f, 1.f, 0.f, 0.f);
		shapes[4].translate(4, 0.f, 0.f, 0.3f);
		for (int i = 0; i < 3; ++i)
			shapes[4].colors.push_back(glm::vec3(0.f, 1.f, 0.f));

		// 왼쪽
		shapes[5].setData(shape_data[SHAPE_TRIANGLE]);
		shapes[5].scale(0, 0.3f, 0.3f, 0.3f);
		shapes[5].rotate(1, 90.f, 0.f, 1.f, 0.f);
		shapes[5].translate(2, -0.3f, 0.f, 0.f);
		shapes[5].rotate(3, -60.f, 0.f, 0.f, 1.f);
		shapes[5].translate(4, +0.3f, 0.f, 0.f);
		for (int i = 0; i < 3; ++i)
			shapes[5].colors.push_back(glm::vec3(0.f, 0.f, 1.f));

		shapes[6].setData(shape_data[SHAPE_TRIANGLE]);
		shapes[6].scale(0, 0.3f, 0.3f, 0.3f);
		shapes[6].rotate(1, 180.f, 0.f, 1.f, 0.f);
		shapes[6].translate(2, 0.f, 0.f, 0.3f);
		shapes[6].rotate(3, -60.f, 1.f, 0.f, 0.f);
		shapes[6].translate(4, 0.f, 0.f, -0.3f);
		for (int i = 0; i < 3; ++i)
			shapes[6].colors.push_back(glm::vec3(1.f, 1.f, 0.f));

		// 왼쪽
		shapes[7].setData(shape_data[SHAPE_TRIANGLE]);
		shapes[7].scale(0, 0.3f, 0.3f, 0.3f);
		shapes[7].rotate(1, 270.f, 0.f, 1.f, 0.f);
		shapes[7].translate(2, 0.3f, 0.f, 0.f);
		shapes[7].rotate(3, 60.f, 0.f, 0.f, 1.f);
		shapes[7].translate(4, -0.3f, 0.f, 0.f);
		for (int i = 0; i < 3; ++i)
			shapes[7].colors.push_back(glm::vec3(0.f, 1.f, 1.f));


	}



	void updateState()
	{
		if (mode_t) {
			shapes[3].rotate(1, 1.f, glm::vec3(0.f, 0.f, 1.f));
		}

		if (mode_f_value > 0) {
			if (mode_f) {
				shapes[5].rotate(3, -1.f, 0.f, 0.f, 1.f);
			}
			else {
				shapes[5].rotate(3, 1.f, 0.f, 0.f, 1.f);
			}
			mode_f_value--;
		}

		if (mode_s_value > 0) {
			if (mode_s) {
				shapes[7].translate(2, 0.f, 0.00666667f, 0.f);
				shapes[8].translate(2, 0.f, 0.00666667f, 0.f);
			}
			else {
				shapes[7].translate(2, 0.f, -0.00666667f, 0.f);
				shapes[8].translate(2, 0.f, -0.00666667f, 0.f);
			}
			mode_s_value--;
		}

		if (mode_b_value > 0) {
			if (mode_b) {
				shapes[6].scale(0, 0.97f, 0.97f, 0.97f);
				shapes[6].translate(2, 0.00333333f, 0.0f, 0.0f);
			}
			else {
				shapes[6].scale(0, 1.030927835f, 1.030927835f, 1.030927835f);
				shapes[6].translate(2, -0.00333333f, 0.0f, 0.0f);
			}
			mode_b_value--;
		}

		if (mode_rotate_y) {
			if (!mode) {
				for (int i = 3; i < 9; ++i) {
					if (i == 5) {
						shapes[i].rotate(5, 1.f, 0.f, 1.f, 0.f);
						continue;
					}
					shapes[i].rotate(3, 1.f, 0.f, 1.f, 0.f);
				}
			}
			else {
				for (int i = 3; i < 8; ++i) {
					if (i == 3) {
						shapes[i].rotate(1, 1.f, 0.f, 1.f, 0.f);
						continue;
					}
					shapes[i].rotate(5, 1.f, 0.f, 1.f, 0.f);
				}
			}
		}

		if (mode_o_value > 0) {
			if (mode_o) {
				shapes[4].rotate(3, 2.666666667f, 1.f, 0.f, 0.f);
				shapes[5].rotate(3, -2.666666667f, 0.f, 0.f, 1.f);
				shapes[6].rotate(3, -2.666666667f, 1.f, 0.f, 0.f);
				shapes[7].rotate(3, 2.666666667f, 0.f, 0.f, 1.f);
			}
			else {
				shapes[4].rotate(3, -2.666666667f, 1.f, 0.f, 0.f);
				shapes[5].rotate(3, 2.666666667f, 0.f, 0.f, 1.f);
				shapes[6].rotate(3, 2.666666667f, 1.f, 0.f, 0.f);
				shapes[7].rotate(3, -2.666666667f, 0.f, 0.f, 1.f);
			}
			mode_o_value--;
		}

		if (mode_r_value > 0) {
			if (mode_r) {
				if (mode_r_value > 270) {
					shapes[4].rotate(3, 2.666666667f, 1.f, 0.f, 0.f);
				}
				else if (mode_r_value > 180) {
					shapes[5].rotate(3, -2.666666667f, 0.f, 0.f, 1.f);
				}
				else if (mode_r_value > 90) {
					shapes[6].rotate(3, -2.666666667f, 1.f, 0.f, 0.f);
				}
				else {
					shapes[7].rotate(3, 2.666666667f, 0.f, 0.f, 1.f);
				}
			}
			else {
				if (mode_r_value > 270) {
					shapes[4].rotate(3, -2.666666667f, 1.f, 0.f, 0.f);
				}
				else if (mode_r_value > 180) {
					shapes[5].rotate(3, 2.666666667f, 0.f, 0.f, 1.f);
				}
				else if (mode_r_value > 90) {
					shapes[6].rotate(3, 2.666666667f, 1.f, 0.f, 0.f);
				}
				else {
					shapes[7].rotate(3, -2.666666667f, 0.f, 0.f, 1.f);
				}
			}
			mode_r_value--;
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