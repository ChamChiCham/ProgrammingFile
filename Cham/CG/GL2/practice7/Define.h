#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

	// --
	// Define variable
	// --

enum
{

	// Display Mode
	WINDOW_DISPLAYMODE = GLUT_DOUBLE | GLUT_RGBA,

	// Window Position
	WINDOW_POSITION_X = 100,
	WINDOW_POSITION_Y = 100,

	// Window Size
	WINDOW_SIZE_X = 800,
	WINDOW_SIZE_Y = 600

};


constexpr const char* WINDOW_TITLE = "Example";