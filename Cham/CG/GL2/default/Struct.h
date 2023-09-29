#pragma once
#include <gl/glew.h>
#include <vector>

// --
// Define struct
// --

// SFPos: float�� ��ǥ��
struct SFPos
{
	float x;
	float y;

	SFPos();
	SFPos(const float _x, const float _y);
	SFPos& operator()(const float _x, const float _y);
};

// SIPos: int�� ��ǥ��
struct SIPos
{
	int x;
	int y;

	SIPos();
	SIPos(const int _x, const int _y);
	SIPos& operator()(const int _x, const int _y);
	SIPos operator+(const int _value);
	SIPos operator-(const int _value);

};

// SFColor: float�� color
struct SFColor
{
	float r;
	float g;
	float b;

	SFColor();
	SFColor(const float _r, const float _g, const float _b);
	SFColor& operator()(const float _r, const float _g, const float _b);
};


struct SVertex
{
	SIPos	pos;
	int		color;

	SVertex();
	SVertex(const SIPos _pos, const int _color);
	SVertex(const int _x, const int _y, const int _color);
	SVertex& operator()(const SIPos _pos, const int _color);
};

struct SBuffer
{
	int		size;
	GLuint	VAO;
	GLuint	VBO[2];
	GLuint	EBO;
	std::vector<GLfloat> VBO0_data;
	std::vector<GLfloat> VBO1_data;

	SBuffer();
	~SBuffer();
};

// SRect: SIPos 2, SFColor 1�� ������ Rect
struct SPoligon
{
	std::vector<SVertex> vertices;

};

struct STri
{
	SVertex vertex[3];

	STri();
	STri(const SVertex _fir, const SVertex _sec, const SVertex _thi);
	STri& operator()(const SVertex _fir, const SVertex _sec, const SVertex _thi);
};