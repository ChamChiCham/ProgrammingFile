#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

struct SView
{
	glm::vec3 eye;
	glm::vec3 at;
	glm::vec3 up;

	SView();
};