#include "WindowMgr.h"
#include "Callback.h"
#include <random>
#include <iostream>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> random_color(0.f, 1.f);



// init inst
CWindowMgr* CWindowMgr::inst = nullptr;


CWindowMgr::CWindowMgr() :
	color{ 1.f, 1.f, 1.f, 1.f },
	is_timer{ false }
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
	glutKeyboardFunc(cb::Keyboard);
}


// Callback Function

// Define Display 
void CWindowMgr::Display()
{
	glClearColor(color[0], color[1], color[2], color[3]);
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

void CWindowMgr::Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'c':
		color[0] = 0.f; color[1] = 1.f; color[2] = 1.f;
		break;

	case 'm':
		color[0] = 1.f; color[1] = 0.f; color[2] = 1.f;
		break;

	case 'y':
		color[0] = 1.f; color[1] = 1.f; color[2] = 0.f;
		break;

	case 'w':
		color[0] = 1.f; color[1] = 1.f; color[2] = 1.f;
		break;

	case 'k':
		color[0] = 0.f; color[1] = 0.f; color[2] = 0.f;
		break;

	case 'a':
		for (int i = 0; i < 3; ++i)
			color[i] = random_color(gen);
		break;

	case 't':
		glutTimerFunc(100, cb::Timer, 1);
		is_timer = true;
		break;

	case 's':
		is_timer = false;
		break;

	case 'q':
		glutLeaveMainLoop();
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void CWindowMgr::Timer(int value)
{
	for (int i = 0; i < 3; ++i)
		color[i] = random_color(gen);

	glutPostRedisplay();

	if (is_timer)
		glutTimerFunc(100, cb::Timer, 1);
}


// run program
void CWindowMgr::run()
{
	glutMainLoop();
}