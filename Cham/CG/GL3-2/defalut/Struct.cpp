#include "Struct.h"

SShapeData::SShapeData()
{}

SShapeData::SShapeData(const std::vector<GLfloat>& _coords, const std::vector<unsigned int>& _indices) :
	coords(_coords),
	indices(_indices)
{}

SBuffer::SBuffer() :
	VAO{ 0 },
	VBO{},
	EBO{ 0 }
{}

SView::SView() :
	eye{ glm::vec3(0.f, 0.f, 0.f) },
	at { glm::vec3(0.f, 0.f, 0.f) },
	up { glm::vec3(0.f, 0.f, 0.f) }
{}
