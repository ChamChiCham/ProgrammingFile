#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>


class CShaderMgr
{
private:
	GLuint program;

public:
	CShaderMgr();
	~CShaderMgr();

public:
	const bool makeProgram(const char* _vert, const char* _frag);


	friend class CWindowMgr;

};

