#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>


class CShaderMgr
{
private:
	GLuint program;

public:
	CShaderMgr();
	~CShaderMgr();

public:
	const bool makeProgram(const char* _vert, const char* _frag);
	void useProgram(const glm::mat4& _matrix);


	friend class CWindowMgr;

};

