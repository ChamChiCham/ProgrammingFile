#include <iostream>
#include <gl/glew.h>              
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>


// 콜백 함수들은 이후에 따로 파일로 만들자.
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

// (CALLBACK) Draw Scene
GLvoid drawScene()
{
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glutSwapBuffers();
}

// (CALLBACK) Reset Viewport
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

// singleton 구현 필요.
class CWindowManager
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


public:

	// 생성자들
	CWindowManager() {}

	CWindowManager(const CWindowManager& other) = delete;
	CWindowManager& operator=(const CWindowManager& other) = delete;


public:

	// Init GL Library
	void init(int& argc, char** argv)
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
	}

	// Run Program
	void run()
	{
		glutDisplayFunc(drawScene);
		glutReshapeFunc(Reshape);
		glutMainLoop();
	}
};

CWindowManager WindowMgr;

void main(int argc, char** argv) 
{
	WindowMgr.init(argc, argv);
	WindowMgr.run();
}
                                                