#include "CShaderMgr.h"

#include <fstream>
#include <string>
#include <iostream>

CShaderMgr::CShaderMgr() :
	program(0), 
	program1(0)
{
}

CShaderMgr::~CShaderMgr()
{
	glDeleteProgram(program);
	glDeleteProgram(program1);
}

const bool CShaderMgr::makeProgram(const char* _vert, const char* _frag)
{
	std::ifstream file(_vert);
	std::string code_vert;
	std::string code_frag;
	std::string line;
	

	// create vertex code
	if (!file.is_open()) {
		std::cerr << "ERROR: file open failed\n" << std::endl;
		return false;
	}

	while (std::getline(file, line)) {
		code_vert += line + '\n';
	}

	file.close();


	// create frag code
	file.open(_frag);

	if (!file.is_open()) {
		return false;
	}


	while (std::getline(file, line)) {
		code_frag += line + '\n';
	}

	file.close();


	// make program
	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);


	char* code_vert_ch = const_cast<char*>(code_vert.c_str());
	char* code_frag_ch = const_cast<char*>(code_frag.c_str());

	glShaderSource(vert_shader, 1, (const GLchar**)&code_vert_ch, 0);
	glShaderSource(frag_shader, 1, (const GLchar**)&code_frag_ch, 0);

	glCompileShader(vert_shader);
	glCompileShader(frag_shader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vert_shader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader compile error\n" << errorLog << std::endl;
		return false;
	}


	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(frag_shader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader compile error\n" << errorLog << std::endl;
		return false;
	}

	program = glCreateProgram();
	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);
	glLinkProgram(program);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	return true;
}

const bool CShaderMgr::makeProgram1(const char* _vert, const char* _frag)
{
	std::ifstream file(_vert);
	std::string code_vert;
	std::string code_frag;
	std::string line;


	// create vertex code
	if (!file.is_open()) {
		std::cerr << "ERROR: file open failed\n" << std::endl;
		return false;
	}

	while (std::getline(file, line)) {
		code_vert += line + '\n';
	}

	file.close();


	// create frag code
	file.open(_frag);

	if (!file.is_open()) {
		return false;
	}


	while (std::getline(file, line)) {
		code_frag += line + '\n';
	}

	file.close();


	// make program
	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);


	char* code_vert_ch = const_cast<char*>(code_vert.c_str());
	char* code_frag_ch = const_cast<char*>(code_frag.c_str());

	glShaderSource(vert_shader, 1, (const GLchar**)&code_vert_ch, 0);
	glShaderSource(frag_shader, 1, (const GLchar**)&code_frag_ch, 0);

	glCompileShader(vert_shader);
	glCompileShader(frag_shader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vert_shader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader compile error\n" << errorLog << std::endl;
		return false;
	}


	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(frag_shader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader compile error\n" << errorLog << std::endl;
		return false;
	}

	program1 = glCreateProgram();
	glAttachShader(program1, vert_shader);
	glAttachShader(program1, frag_shader);
	glLinkProgram(program1);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	return true;
}

void CShaderMgr::useProgram(const glm::mat4& _matrix)
{
	// program use
	glUseProgram(program);

	// uniform matrix 
	unsigned int transformLocation = glGetUniformLocation(program, "transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(_matrix));


}
