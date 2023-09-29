#include "Utility.h"
#include <random>


std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dist_color(0.f, 1.f);
std::uniform_real_distribution<float> dist_pos(-1.f, 0.9f);

const SFPos posWinToGL(const int _x, const int _y)
{
	float ay = static_cast<float>(WINDOW_SIZE_Y - _y);
	float rx = static_cast<float>((_x - (WINDOW_SIZE_X / 2))) / static_cast<float>((WINDOW_SIZE_X / 2));
	float ry = static_cast<float>((ay - (WINDOW_SIZE_Y / 2))) / static_cast<float>((WINDOW_SIZE_Y / 2));
	return SFPos(rx, ry);
}
const SFPos posWinToGL(SIPos _ipos)
{
	return posWinToGL(_ipos.x, _ipos.y);
}

// GL Pos to Window Pos
const SIPos posGLToWin(const float _x, const float _y)
{
	int rx = (_x * static_cast<float>(WINDOW_SIZE_X / 2)) + static_cast<float>(WINDOW_SIZE_X / 2);
	int ry = (_y * static_cast<float>(WINDOW_SIZE_Y / 2)) + static_cast<float>(WINDOW_SIZE_Y / 2);
	ry = WINDOW_SIZE_Y - ry;
	return SIPos(rx, ry);
}
const SIPos posGLToWin(SFPos _fpos)
{
	return posGLToWin(_fpos.x, _fpos.y);
}

const SFColor colorWinToGL(const int _color)
{
	SFColor res;
	int color = _color;

	res.b = static_cast<float>(color % 0x100) / 255.f;
	color /= 0x100;
	res.g = static_cast<float>(color % 0x100) / 255.f;
	color /= 0x100;
	res.r = static_cast<float>(color % 0x100) / 255.f;

	return res;
}


// set random color
void randomColor(float& _r, float& _g, float& _b)
{
	_r = dist_color(gen);
	_g = dist_color(gen);
	_b = dist_color(gen);
}
void randomColor(SFColor& _color)
{
	randomColor(_color.r, _color.g, _color.b);
}