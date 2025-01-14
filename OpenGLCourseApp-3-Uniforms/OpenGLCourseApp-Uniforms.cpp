// OpenGLCourseApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//OpenGL
#include "GL/glew.h" //include glew.h before glfw bc it has gl.h and glfw requies gl.h
#include "GLFW/glfw3.h"
//
#include <stdio.h>
#include <string.h>
#include <iostream>

//Window width and height.
const GLint WIDTH = 800, HEIGHT = 600;

//Resize windows size if windows size changed.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//Process user inputs.
void ProcessInput(GLFWwindow* window);

void CreateTriangle(GLuint& shaderProgram, GLuint& VAO);

/*
Vertex Array Object: Vertex Array Object can be bound just like a vertex buffer object and
any subsequent vertex attribute calls from that point on will be stored inside the VAO.
Vertex Buffer Object: Vertex Buffer Object used to manage GPU's memory
from CPU bc we don't have direct access to GPU's memory.
shaderProgram: A shader program object is the final linked version of multiple shaders combined.
*/

//GLuint VAO, VBO, shaderProgram;

/*
Vertex shader soruce code orginally written in GLSL,
version 330 means version 3.3 which is GLSL version and core for core profile,
it's version has to be same with OpenGL' version,
all vertex input attributes will be defined with "in" keyword ex: in vec3 aPos,

*/
const  char* vertexShaderSource = "						\n\
#version  330  core										\n\
layout (location  =  0) in vec3 aPos;					\n\
void main()												\n\
{														\n\
gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);		\n\
}\0";

/*
Fragment shader soruce code orginally written in GLSL,
version 330 means version 3.3 which is GLSL version and core for core profile,
t's version has to be same with OpenGL' version,
all fragment output attributes will be defined with "out" keyword ex: out vec4 FragColor,

*/
const  char* fragmentShaderSource = "					\n\
#version  330  core										\n\
out vec4 FragColor;										\n\
uniform vec4 vertexColor;	// uniform input variable	\n\
void main()												\n\
{														\n\
FragColor = vertexColor;								\n\
}\0";



int main()
{
	//Set up GLFW window properties
	//OpenGL version					//for example GTX 1650 mobile uses OpenGL 4.6 version 
	glfwWindowHint(GL_MAJOR_VERSION, 3);
	glfwWindowHint(GL_MINOR_VERSION, 3);// set application version to 3.3 for minimum OpenGL version
	//GLFW_OPENGL_CORE_PROFILE means no backward compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//Initialize GLFW
	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Couldn't initialize GLFW library!" << std::endl;
		glfwTerminate();
		return 1;
	}

	//create window right after glfwInit()
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test", NULL, NULL);
	if (!mainWindow) {
		std::cout << "Couldn't create GLFW window titled: mainWindow" << std::endl;
		glfwTerminate();
		return 1;
	}

	//Get buffer size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//set context to GLEW use
	glfwMakeContextCurrent(mainWindow);

	//Allow modern extension features
	glewExperimental = GL_TRUE;

	//Initialize GLEW
	if (glewInit() != GLEW_OK) {
		std::cout << "Couldn't initialize GLEW!" << std::endl;
		glfwDestroyWindow(mainWindow);	//first destroy window bc we created a windows which named mainWindows
		glfwTerminate();	//terminate GLFW
		return 1;
	}

	//set up viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);


	//glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback); //set windows resize callback

	//shaderProgram: shader program's id.
	//VAO: Vertex Array Object's id.
	GLuint shaderProgram, VAO;
	//create 2D triangle
	CreateTriangle(shaderProgram, VAO);

	//Render loop
	while (!glfwWindowShouldClose(mainWindow)) {
		//input
		ProcessInput(mainWindow);

		//clear window with a color
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "vertexColor");
		glUniform4f(vertexColorLocation, 0.0f, 0.0f, 0.0f, 1.0f);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		//glBindVertexArray(0);  // no need to unbind it every time 
		
		//render 
		glfwSwapBuffers(mainWindow);
		//get and handle user input events
		glfwPollEvents();
	}


	glfwTerminate();	//terminate GLFW before program exits!
	std::cout << "GLFW terminated return 0" << std::endl;
	return 0;

}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE)) { //close window when esc key pressed
		glfwSetWindowShouldClose(window, 1);
	}
}

void CreateTriangle(GLuint& shaderProgram, GLuint& VAO) {
	// build and compile our shader program
	// ------------------------------------
	// vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// link shaders
	//unsigned int shaderProgram = glCreateProgram();
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, // left  
		 0.5f, -0.5f, 0.0f, // right 
		 0.0f,  0.5f, 0.0f  // top   
	};


	//unsigned int VBO, VAO;
	GLuint VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);	//generates buffer on GPU's memory and stores it's id in VBO variable

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/*
	set vertex attributes of VBO
	0 = specifies aPos attribute which we define location = 0 in vertex shader (which attribute we want to configure).
	3 = size of the vertex (for triangle its 3).
	GL_FLOAT = type of data we pass.
	GL_FALSE = set true if data need to be normalized.
	3 * sizeof(GLfloat) = stride (space between consecutive vertex).
	(void*)0 = This is the offset of where the position data begins in the buffer
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	/*
	Each vertex attribute takes its data from memory managed by a VBO
	and which VBO it takes its data from (you can have multiple VBOs)
	is determined by the VBO currently bound to GL_ARRAY_BUFFER
	when calling glVertexAttribPointer. Since the previously defned VBO
	is still bound before calling glVertexAttribPointer vertex attribute 0 is
	now associated with its vertex data
	*/
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);


	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

}