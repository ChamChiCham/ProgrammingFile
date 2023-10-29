#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "Struct.h"


class CShapeDataMgr
{
private:
	std::vector<SShapeData> data;
	static CShapeDataMgr* instance;

private:
	CShapeDataMgr();

public:
	void init();
	void initObj(const char* _obj);

	SShapeData& getData(const int _idx);

	static CShapeDataMgr* getInst();

};

