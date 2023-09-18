#include <iostream>
#include <gl/glew.h>              
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include "WindowMgr.h"

void main(int argc, char** argv) 
{
	CWindowMgr::getInst()->init(argc, argv);
	CWindowMgr::getInst()->run();
}
                                                