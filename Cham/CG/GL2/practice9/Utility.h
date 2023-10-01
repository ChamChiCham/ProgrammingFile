#pragma once
#include "Struct.h"
#include "Define.h"

const SFPos posWinToGL(const int _x, const int _y);
const SFPos posWinToGL(SIPos _ipos);

const SIPos posGLToWin(const float _x, const float _y);
const SIPos posGLToWin(SFPos _fpos);

const SFColor colorWinToGL(const int _color);

void randomColor(float& _r, float& _g, float& _b);
void randomColor(SFColor& _color);
