#include "Struct.h"

// SFPos
SFPos::SFPos() : x(0.f), y(0.f) {}

SFPos::SFPos(const float _x, const float _y) : x(_x), y(_y) {}

SFPos& SFPos::operator()(const float _x, const float _y)
{
	x = _x;
	y = _y;
	return *this;
}

// SIPos
SIPos::SIPos() : x(0), y(0) {}

SIPos::SIPos(const int _x, const int _y) : x(_x), y(_y) {}

SIPos& SIPos::operator()(const int _x, const int _y)
{
	x = _x;
	y = _y;
	return *this;
}

SIPos SIPos::operator+(const int _value)
{
	return SIPos(x + _value, y + _value);
}

SIPos SIPos::operator-(const int _value)
{
	return SIPos(x - _value, y - _value);
}


// SFColor:
SFColor::SFColor() : r(0.f), g(0.f), b(0.f) {}

SFColor::SFColor(const float _r, const float _g, const float _b) : r(_r), g(_g), b(_b) {}

SFColor& SFColor::operator()(const float _r, const float _g, const float _b)
{
	r = _r;
	g = _g;
	b = _b;
	return *this;
}



//SVertex
SVertex::SVertex() : pos{}, color(0) {}

SVertex::SVertex(const SIPos _pos, const int _color) : pos(_pos), color(_color) {}

SVertex::SVertex(const int _x, const int _y, const int _color) : pos(SIPos(_x, _y)), color(_color) {}

SVertex& SVertex::operator()(const SIPos _pos, const int _color)
{
	pos = _pos;
	color = _color;
	return *this;
}

STri::STri() : vertex{} {}

STri::STri(const SVertex _fir, const SVertex _sec, const SVertex _thi) : vertex{_fir, _sec, _thi} {}

STri& STri::operator()(const SVertex _fir, const SVertex _sec, const SVertex _thi)
{
	vertex[0] = _fir;
	vertex[1] = _sec;
	vertex[2] = _thi;
	return *this;
}

SBuffer::SBuffer() : size{ 0 }, VAO{ 0 }, VBO{ 0, 0 }, EBO{ 0 }, VBO0_data{}, VBO1_data{} {}

SBuffer::~SBuffer()
{
}

SPoligon& SPoligon::operator()(const SVertex _targ)
{
	vertices.push_back(_targ);
	return *this;
}

SPoligon& SPoligon::operator()(const int _x, const int _y, const int _color)
{
	vertices.push_back(SVertex(_x, _y, _color));
	return *this;
}
