#pragma once
#ifndef SHADER_H
#define SHADER_H	

class Shader {
public:
	//shader ID
	GLuint ID = 0;

	//build shader from file
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();
	//  use/activate  the  shader
	void use();

	//  utility  uniform  functions
	
	void SetBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const; 
	void setFloat(const std::string& name, float value) const;

private:

};

#endif 