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

//C++ headers
#include <stdio.h>
#include <string.h>
#include <iostream>
//

//Project headers
#include "Shader.h"
#include "Camera.h"
//


//Window width and height.
extern const int WIDTH = 800;
extern const int HEIGHT = 600;

//camera object that used for view and projection matrices
Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f), //camera positon
	glm::vec3(0.0f, 0.0f, -1.0f), //camera front
	glm::vec3(0.0f, 1.0f, 0.0f)); //camera up

//time that passed since last frame
GLfloat deltaTime = 0.0f;
//frames per second
int fps = 0;

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

void UpdateView(glm::mat4& _view);

void UpdateProjection(glm::mat4& _projection);

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

	//Get buffer size information to set view port
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
	const char* vertexSourcePath = "C:\\Users\\bgyas\\source\\repos\\OpenGLCourseApp\\OpenGLCourseApp-9.2-Camera Class\\vertexShaderSource.txt";
	const char* fragmentSourcePath = "C:\\Users\\bgyas\\source\\repos\\OpenGLCourseApp\\OpenGLCourseApp-9.2-Camera Class\\fragmentShaderSource.txt";

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
	view = glm::lookAt(camera.GetPos(),	//camera position
		camera.GetPos() + camera.GetFront(),	//target
		camera.GetUp());	//up vector

	//projection matrix
	glm::mat4 projection(1.0f);		//FOV, Aspect Ratio (W/H), Nearest point, furthest point (objects between these points will be rendered)
	projection = glm::perspective(camera.GetFov(), static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 100.0f);
	//

	//set cursor posiiton call back
	glfwSetCursorPosCallback(mainWindow, CursorPosCallBack);

	//disable cursor for our window		
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//set mouse scroll call back
	glfwSetScrollCallback(mainWindow, ScrollCallBack);

	//enable Z-buffer for opengl depth calculations
	glEnable(GL_DEPTH_TEST);		

	//to calculate delta time and fps
	GLfloat lastFrame = 0.0f;
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
		
		//update view to move and look effect
		UpdateView(view);
		//update projection to zoom effect
		UpdateProjection(projection);

		//clear window with a color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//clear color and buffer bit
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


	float cameraSpeed = static_cast<float>(camera.GetMovementSpeed() * deltaTime);
	glm::vec3 cameraPos = camera.GetPos();
	glm::vec3 cameraFront = camera.GetFront();
	glm::vec3 cameraUp = camera.GetUp();

	//change camera position according to WASD input
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.SetPos(cameraPos + (cameraSpeed * cameraFront));

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.SetPos(cameraPos - (cameraSpeed * cameraFront));

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.SetPos(cameraPos + (glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed));
	
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.SetPos(cameraPos - (glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed));
}

void CursorPosCallBack(GLFWwindow* window, GLdouble xPosIn, GLdouble yPosIn) {
	camera.UpdateDirection(window, xPosIn, yPosIn);
}

void ScrollCallBack(GLFWwindow* window, GLdouble offsetX, GLdouble offsetY) {
	camera.UpdateZoom(window, offsetX, offsetY);
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

void UpdateView(glm::mat4& _view)
{
	_view = glm::lookAt(camera.GetPos(),	//camera position
		camera.GetPos() + camera.GetFront(),	//target
		camera.GetUp());	//up vector
}

void UpdateProjection(glm::mat4& _projection)
{
	_projection = glm::perspective(camera.GetFov(),	//fov
								   static_cast<float>(WIDTH) / static_cast<float>(HEIGHT),	//aspect ratio
								   0.1f, 100.0f);	//nearest and furthest position
}
