#pragma once
#include <gl/glew.h>              
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include "WindowMgr.h"

namespace cb
{
	
	GLvoid Display();
	GLvoid Reshape(int w, int h);
	GLvoid Keyboard(unsigned char key, int x, int y);
	GLvoid Timer(int value);
}