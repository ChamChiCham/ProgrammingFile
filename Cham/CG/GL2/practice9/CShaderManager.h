#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>


class CShaderManager
{
private:
	GLuint program;

public:
	CShaderManager();
	~CShaderManager();

public:
	const bool makeProgram(const char* _vert, const char* _frag);


	friend class CWindowMgr;

};

