// OpenGLCourseApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//OpenGL
#include "GL/glew.h" //include glew.h before glfw bc it has gl.h and glfw requies gl.h
#include "GLFW/glfw3.h"
//

//OpenGL MathLibrary
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
//

#include <stdio.h>
#include <string.h>
#include <iostream>

#include "Shader.h"

//Window width and height.
const GLint WIDTH = 800, HEIGHT = 600;

//camera attributes
glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);	//somewhere in front of camera always -1 in z
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
GLboolean firstMove = true;
GLfloat lastX = static_cast<float>(WIDTH)/2;
GLfloat lastY = static_cast<float>(HEIGHT)/2;
GLfloat yaw = -90.0f, pitch = 0.0f;	//-90 degree yaw to start camera looking negative z axis and also prevent flipping at the start bc if we set 0 at the start we will be looking positive x axis!
GLfloat fov = 45.0f;
GLfloat zoom = fov;
//time that passed since last frame
GLfloat deltaTime = 0.0f;

//Resize windows size if windows size changed.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//Process user inputs.
void ProcessInput(GLFWwindow* window);

//when mouse changed position this call back function will be called
void CursorPosCallBack(GLFWwindow* window, GLdouble xPosIn, GLdouble yPosIn);

//
void ScrollCallBack(GLFWwindow* window, GLdouble offsetX, GLdouble offsetY);

//create triangle
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

	//resize window callback
	glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback); //set windows resize callback


	//VAO: Vertex Array Object's id.
	GLuint VAO;

	//vertex and fragment shader paths
	const char* vertexSourcePath = "C:\\Users\\bgyas\\source\\repos\\OpenGLCourseApp\\OpenGLCourseApp-9.1-Walk Around\\vertexShaderSource.txt";
	const char* fragmentSourcePath = "C:\\Users\\bgyas\\source\\repos\\OpenGLCourseApp\\OpenGLCourseApp-9.1-Walk Around\\fragmentShaderSource.txt";

	//crate shader program
	Shader ShaderProgram = Shader(vertexSourcePath, fragmentSourcePath);

	//create 2D triangle
	CreateTriangle(VAO);

	//	gl_position = Mprojection * Mview * Mmodel * Vlocation		//these matrix multipications made for drawing things the 3D plane

	//3D view
	//model matrix
	glm::mat4 model(1.0f);
	//model = glm::rotate(model, glm::radians(-70.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	//view matrix
	glm::mat4 view;
	view = glm::lookAt(cameraPos,	//camera position
					   cameraPos + cameraFront,	//target
					   cameraUp);	//up vector
	

	//projection matrix
	glm::mat4 projection(1.0f);				//FOV	   Aspect Ratio (W/H)  Nearest and furthest point, objects between these points will be rendered
	projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
	//

	//set cursor posiiton call back
	glfwSetCursorPosCallback(mainWindow, CursorPosCallBack);
	//disable cursor for our window		
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//set mouse scroll call back
	glfwSetScrollCallback(mainWindow, ScrollCallBack);

	glEnable(GL_DEPTH_TEST);		//enable Z-buffer for open gl depth calculations

	GLfloat lastFrame = 0.0f;
	int fps = 0;
	GLdouble lastSecond = 1.0;

	//Render loop
	while (!glfwWindowShouldClose(mainWindow)) {
		//print delta time
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		std::cout << "delta time: " << deltaTime << std::endl;

		//print fps
		fps++;
		if (currentTime - lastSecond >= 1.0) {
			lastSecond = currentTime;
			std::cout << "fps: " << fps << std::endl;
			fps = 0;
		}

		//input
		ProcessInput(mainWindow);
		//update view to move
		view = glm::lookAt(cameraPos,	//camera position
							cameraPos + cameraFront,	//target
							cameraUp);	//up vector

		projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

		//clear window with a color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//use shader program
		ShaderProgram.use();

		glUniformMatrix4fv(glGetUniformLocation(ShaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(ShaderProgram.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(ShaderProgram.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
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

	
	float cameraSpeed = static_cast<float>(2.5 * deltaTime);
	//change camera position according to WASD input
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)	
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void CursorPosCallBack(GLFWwindow* window, GLdouble xPosIn, GLdouble yPosIn) {
	float xPos = static_cast<float>(xPosIn);
	float yPos = static_cast<float>(yPosIn);

	if (firstMove) {
		lastX = xPos;
		lastY = yPos;
		firstMove = false;
	}

	float offsetX = 0.0f;
	float offsetY = 0.0f;

	offsetX = xPos - lastX;
	offsetY = lastY - yPos;		//yPos - lastY, for reversed camera

	lastX = xPos;
	lastY = yPos;

	float sensivity = 0.1f;

	yaw += (offsetX * sensivity);
	pitch += (offsetY * sensivity);

	if (pitch > 89.0f)
		pitch = 89.f;
	else if (pitch < -89.0f)
		pitch = -89.0f;

	if (yaw > 359)
		yaw = glm::mod(yaw, 360.0f);


	glm::vec3 direction(0);
	direction.x = glm::cos(glm::radians(pitch)) * glm::cos(glm::radians(yaw));
	direction.y = glm::sin(glm::radians(pitch));
	direction.z = glm::cos(glm::radians(pitch)) * glm::sin(glm::radians(yaw));

	cameraFront = glm::normalize(direction);
}

void ScrollCallBack(GLFWwindow* window, GLdouble offsetX, GLdouble offsetY)
{
	zoom-=static_cast<float>(offsetY);	//narrow down fov according to scroll y offset
	if(zoom < 1.0f)
		zoom = 1.0f;
	if(zoom > 45.0f)
		zoom = 45.0f;
	fov = zoom;
}

void CreateTriangle(GLuint& VAO) {
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 1.0f,

	-0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

	 0.5f,  0.5f,  0.5f,  0.2f, 0.2f,
	 0.5f,  0.5f, -0.5f,  0.2f, 0.2f,
	 0.5f, -0.5f, -0.5f,  0.2f, 0.2f,
	 0.5f, -0.5f, -0.5f,  0.2f, 0.2f,
	 0.5f, -0.5f,  0.5f,  0.2f, 0.2f,
	 0.5f,  0.5f,  0.5f,  0.2f, 0.2f,

	-0.5f, -0.5f, -0.5f,  0.5f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.5f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.5f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.5f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.5f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.5f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 0.3f,
	 0.5f,  0.5f, -0.5f,  0.0f, 0.3f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.3f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.3f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.3f,
	-0.5f,  0.5f, -0.5f,  0.0f, 0.3f
	};


	//unsigned int VBO, VAO;
	GLuint VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);	//generates buffer on GPU's memory and stores it's id in VBO variable

	// bind the Vertex Array Object after genereating buffers, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

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