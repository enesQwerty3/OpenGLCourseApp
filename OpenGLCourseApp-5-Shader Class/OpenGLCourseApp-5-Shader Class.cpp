// OpenGLCourseApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//OpenGL
#include "GL/glew.h" //include glew.h before glfw bc it has gl.h and glfw requies gl.h
#include "GLFW/glfw3.h"
//
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "Shader.h"

//Window width and height.
const GLint WIDTH = 800, HEIGHT = 600;

//Call back for resize window's size if windows size changed.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//Process user inputs.
void ProcessInput(GLFWwindow* window);

void CreateTriangle(GLuint& VAO);

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

	//set windows resize callback
	glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback);

	//shaderProgram: shader program's id.
	//VAO: Vertex Array Object's id.
	GLuint VAO;
	const char* vertexSourcePath = "C:\\Users\\bgyas\\source\\repos\\OpenGLCourseApp\\OpenGLCourseApp-5-Shader Class\\vertexShaderSource.txt";
	const char* fragmentSourcePath = "C:\\Users\\bgyas\\source\\repos\\OpenGLCourseApp\\OpenGLCourseApp-5-Shader Class\\fragmentShaderSource.txt";
	//create 2D triangle
	CreateTriangle(VAO);
	//create shader program
	Shader ShaderProgram = Shader(vertexSourcePath, fragmentSourcePath);
	
	//Render loop
	while (!glfwWindowShouldClose(mainWindow)) {
		//input
		ProcessInput(mainWindow);

		//clear window with a color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ShaderProgram.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		//glBindVertexArray(0);  // no need to unbind it every time 
		
		//render 
		glfwSwapBuffers(mainWindow);
		//get and handle user input events
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);

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

void CreateTriangle(GLuint& VAO) {
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		//  positions		//  colors	(red, green and blue)
		0.5f, -0.5f, 0.0f, 0.5f, 0.3f, 0.2f,	//  bottom right
		-0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.5f,	//  bottom  left
		0.0f, 0.5f, 0.0f, 0.3f, 0.2f, 0.5f		//  top
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	/*
	Each vertex attribute takes its data from memory managed by a VBO
	and which VBO it takes its data from (you can have multiple VBOs)
	is determined by the VBO currently bound to GL_ARRAY_BUFFER
	when calling glVertexAttribPointer. Since the previously defned VBO
	is still bound before calling glVertexAttribPointer vertex attribute 0 is
	now associated with its vertex data
	*/
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);


	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

}