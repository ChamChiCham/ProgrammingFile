#include "CShapeDataMgr.h"

#include <fstream>
#include <iostream>
#include <string>

#include "Define.h"

CShapeDataMgr* CShapeDataMgr::instance = nullptr;

CShapeDataMgr::CShapeDataMgr()
{
	init();
}

void CShapeDataMgr::init()
{
	data.reserve(6);

	data.push_back(SShapeData());
	data[SHAPE_DICE].coords =
	{
		1.f,	1.f,	1.f,
		-1.f,	1.f,	1.f,
		1.f,	-1.f,	1.f,
		-1.f,	-1.f,	1.f,
		1.f,	1.f,	-1.f,
		-1.f,	1.f,	-1.f,
		1.f,	-1.f,	-1.f,
		-1.f,	-1.f,	-1.f
	};

	data[SHAPE_DICE].indices =
	{
		0, 3, 1,
		0, 2, 3,
		0, 4, 2,
		2, 4, 6,
		2, 6, 3,
		3, 6, 7,
		4, 7, 6,
		4, 5, 7,
		7, 5, 3,
		1, 3, 5,
		4, 1, 5,
		0, 1, 4
	};

	data.push_back(SShapeData());
	data[SHAPE_SQUARE_PRAMID].coords =
	{
		0.f,	1.f,	0.f,
		1.f,	-1.f,	1.f,
		-1.f,	-1.f,	1.f,
		1.f,	-1.f,	-1.f,
		-1.f,	-1.f,	-1.f,
	};

	data[SHAPE_SQUARE_PRAMID].indices =
	{
		0, 2, 1,
		0, 4, 2,
		0, 3, 4,
		0, 1, 3,
		1, 2, 4,
		1, 4, 3
	};

	data.push_back(SShapeData());
	data[SHAPE_TETRA].coords =
	{
		0.f,	1.f,	0.f,
		1.f,	-1.f,	1.f,
		-1.f,	-1.f,	1.f,
		0.f,	-1.f,	-1.f
	};

	data[SHAPE_TETRA].indices =
	{
		0, 2, 1,
		0, 3, 2,
		0, 1, 3,
		1, 2, 3
	};

	data.push_back(SShapeData());
	data[SHAPE_SQUARE].coords =
	{
		1.f,	0.f,	1.f,
		-1.f,	0.f,	1.f,
		1.f,	0.f,	-1.f,
		-1.f,	0.f,	-1.f
	};

	data[SHAPE_SQUARE].indices =
	{
		0, 2, 1,
		1, 2, 3
	};

	data.push_back(SShapeData());
	data[SHAPE_TRIANGLE].coords =
	{
		1.f,	0.f,	1.f,
		-1.f,	0.f,	1.f,
		0.f,	0.f,	-1.f,
	};

	data[SHAPE_TRIANGLE].indices =
	{
		0, 2, 1
	};

	initObj("Sphere.obj");
}

void CShapeDataMgr::initObj(const char* _obj)
{
	std::ifstream file(_obj);
	std::string line;

	if (!file.is_open()) {
		std::cerr << "ERROR: file open failed named \n" << _obj << std::endl;
		return;
	}


	data.push_back(SShapeData());
	while (std::getline(file, line)) {

		if (line.substr(0, 2) == "v ") {
			float vertices[3] = {};
			sscanf_s(line.c_str(), "v %f %f %f", &vertices[0], &vertices[1], &vertices[2]);

			for (auto& vertex : vertices)
				data[SHAPE_SPHERE].coords.push_back(vertex);
		}
		else if (line.substr(0, 2) == "f ") {
			int indices[3] = {}, dummy0[3] = {}, dummy1[3] = {};
			sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&indices[0], &dummy0[0], &dummy1[0],
				&indices[1], &dummy0[1], &dummy1[1],
				&indices[2], &dummy0[2], &dummy1[2]);
			for (auto& index : indices) {
				data[SHAPE_SPHERE].indices.push_back(index - 1);
			}
		}
	}

	file.close();
}

SShapeData& CShapeDataMgr::getData(const int _idx)
{
	return data[_idx];
}

CShapeDataMgr* CShapeDataMgr::getInst()
{
	if (!instance) {
		instance = new CShapeDataMgr();
	}
	return instance;
}
