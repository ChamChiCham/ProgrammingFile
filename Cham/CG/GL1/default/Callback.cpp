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
