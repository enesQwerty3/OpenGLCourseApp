#pragma once
#ifndef SHADER_H
#define SHADER_H	

#include <glm/glm.hpp>

class Shader {
public:
	//shader ID
	int ID = 0;

	//build shader from file
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();
	//  use/activate  the  shader
	void use();

	//  utility  uniform  functions

	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetMat4f(const std::string& name, int count, bool transpose, glm::mat4 value) const;
	void SetFloat3(const std::string& name, float x, float y, float z) const;
private:

};

#endif 