#include "Struct.h"

SShapeData::SShapeData()
{}

SShapeData::SShapeData(const std::vector<GLfloat>& _coords, const std::vector<unsigned int>& _indices) :
	coords(_coords),
	indices(_indices)
{}

SBuffer::SBuffer() : VAO{ 0 }, VBO{}, EBO{ 0 }
{}