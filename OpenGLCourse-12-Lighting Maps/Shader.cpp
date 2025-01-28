#include<GL/glew.h>
#include <glm/glm.hpp>
#include "glm/gtc/type_ptr.hpp"

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>

using namespace std;
#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	//  1.  retrieve  the  vertex/fragment  source  code  from  filePath
	string vertexCode;
	string fragmentCode;
	ifstream vShaderFile;
	ifstream fShaderFile;
	//  ensure  ifstream  objects  can  throw  exceptions:
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit); fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
		//  open  files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		stringstream  vShaderStream, fShaderStream;
		//  read  file’s  buffer  contents  into  streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//  close  file  handlers
		vShaderFile.close();
		fShaderFile.close();
		//  convert  stream  into  string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (ifstream::failure  e)
	{
		cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
	}
	//source code of vertex shader
	const  char* vShaderCode = vertexCode.c_str();
	//source code of fragment shader
	const  char* fShaderCode = fragmentCode.c_str();

	//compile shaders
	GLchar eLog[1024];
	GLint result;

	GLuint vertexShader, fragmentShader;
	//vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

	if (!result) {
		glGetShaderInfoLog(vertexShader, 1024, NULL, eLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << eLog << endl;
	}

	//fragment shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);

	if (!result) {
		glGetShaderInfoLog(fragmentShader, 1024, NULL, eLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << eLog << endl;
	}

	//create program, attach shaders to program and link program
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(ID, 1024, NULL, eLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << eLog << endl;
	}
	// delete shaders; they’re linked into our program and no longer necessary
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::SetBool(const string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetInt(const string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat(const string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetMat4f(const std::string& name, int count, bool transpose, glm::mat4 value) const
{
	glProgramUniformMatrix4fv(ID, glGetUniformLocation(ID, name.c_str()), count, transpose, glm::value_ptr(value));
}

void Shader::SetFloat3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

