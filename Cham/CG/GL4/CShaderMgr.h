#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>


class CShaderMgr
{
private:
	GLuint program;
	GLuint program1;

public:
	CShaderMgr();
	~CShaderMgr();

public:
	const bool makeProgram(const char* _vert, const char* _frag);
	const bool makeProgram1(const char* _vert, const char* _frag);
	void useProgram(const glm::mat4& _matrix);


	friend class CWindowMgr;

};

