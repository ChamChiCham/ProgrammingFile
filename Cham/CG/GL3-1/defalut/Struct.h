#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <vector>


struct SShapeData
{
	std::vector<GLfloat> coords;
	std::vector<unsigned int> indices;

	SShapeData();
	SShapeData(const std::vector<GLfloat>& _coords, const std::vector<unsigned int>& _indices);

};


struct SBuffer
{
	GLuint	VAO;
	GLuint	VBO[2];
	GLuint	EBO;

	SBuffer();

};