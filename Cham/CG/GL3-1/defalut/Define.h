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
	WINDOW_SIZE_X = 1280,
	WINDOW_SIZE_Y = 960,

	// shape name
	SHAPE_DICE = 0,
	SHAPE_LINE,
	SHAPE_SQUARE_PRAMID,
	SHAPE_TETRA,
	SHAPE_SQUARE,
	SHAPE_TRIANGLE,
	SHAPE_SPHERE, // 중심: 0, 0.5, 0 / 지름: 5.09116 .transform(0, 0.f, -0.05f, 0.f); .scale(1, 0.392837782.f,,);

	MAT_S = 0,
	MAT_R = 1,
	MAT_T = 2

};


constexpr const char* WINDOW_TITLE = "Example";