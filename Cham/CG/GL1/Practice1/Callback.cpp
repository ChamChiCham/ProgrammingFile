#include "Callback.h"

// (CALLBACK) Draw Scene
GLvoid cb::Display()
{
	CWindowMgr::getInst()->Display();
}


// (CALLBACK) Reset Viewport
GLvoid cb::Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid cb::Keyboard(unsigned char key, int x, int y)
{
	CWindowMgr::getInst()->Keyboard(key, x, y);
}

GLvoid cb::Timer(int value)
{
	CWindowMgr::getInst()->Timer(value);
}
