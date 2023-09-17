#pragma once
#include <iostream>
#include <gl/glew.h>              
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include "Callback.h"

class CWindowMgr
{
private:

	// Define Int Constant
	enum {

		// Display Mode
		WINDOW_DISPLAYMODE = GLUT_DOUBLE | GLUT_RGBA,

		// Window Position
		WINDOW_POSITION_X = 100,
		WINDOW_POSITION_Y = 100,

		// Window Size
		WINDOW_SIZE_X = 250,
		WINDOW_SIZE_Y = 250,

	};

	// Define String Constant
	const char* WINDOW_TITLE = "Example1";

	// Define member Variable


	// singleton
	static CWindowMgr* inst;

private:

	CWindowMgr();

	~CWindowMgr();


public:

	static CWindowMgr* getInst();

	CWindowMgr(const CWindowMgr& other) = delete;
	CWindowMgr& operator=(const CWindowMgr& other) = delete;


public:

	// Init GL Library
	void init(int& argc, char** argv);

	// Callback Function
	void Display();




	// Run Program
	void run();


};