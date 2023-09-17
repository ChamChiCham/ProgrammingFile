#include "WindowMgr.h"

// init inst
CWindowMgr* CWindowMgr::inst = nullptr;


CWindowMgr::CWindowMgr()
{}

CWindowMgr::~CWindowMgr()
{
	delete inst;
}

// return instance
CWindowMgr* CWindowMgr::getInst()
{
	if (inst == nullptr) {
		inst = new CWindowMgr;
	}
	return inst;
}

// Init GL Library
void CWindowMgr::init(int& argc, char** argv)
{
	// init GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(WINDOW_DISPLAYMODE);
	glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);
	glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glutCreateWindow(WINDOW_TITLE);

	// init GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	// set cb func
	glutDisplayFunc(cb::Display);
	glutReshapeFunc(cb::Reshape);
}


// Callback Function

// Define Display 
void CWindowMgr::Display()
{
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}



// run program
void CWindowMgr::run()
{
	glutMainLoop();
}