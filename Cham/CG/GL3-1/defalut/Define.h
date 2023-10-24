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
	WINDOW_DISPLAYMODE = GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH,

	// Window Position
	WINDOW_POSITION_X = 100,
	WINDOW_POSITION_Y = 100,

	// Window Size
	WINDOW_SIZE_X = 800,
	WINDOW_SIZE_Y = 600,

	// shape name
	SHAPE_DICE = 0,
	SHAPE_LINE = 1,
	SHAPE_SQUARE_PRAMID = 2,
	SHAPE_TETRA = 3,

	MAT_S = 0,
	MAT_R = 1,
	MAT_T = 2

};


constexpr const char* WINDOW_TITLE = "Example";