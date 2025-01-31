// OpenGLCourseApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//OpenGL
#include <GL/glew.h> //include glew.h before glfw bc it has gl.h and glfw requies gl.h
#include <GLFW/glfw3.h>
//

//OpenGL MathLibrary
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//

//C++ headers
#include <stdio.h>
#include <string>
#include <iostream>
//

//Project headers
#include "Shader.h"
#include "Camera.h"
#include "stb_image.h"
#include "Model.h"
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
bool directionalLightOn = false;
bool flashlightOn = false;

//Resize windows size if windows size changed.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//Process user inputs.
void ProcessInput(GLFWwindow* window);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

//when mouse changed position this call back function will be called
void CursorPosCallBack(GLFWwindow* window, GLdouble xPosIn, GLdouble yPosIn);

//
void ScrollCallBack(GLFWwindow* window, GLdouble offsetX, GLdouble offsetY);

void UpdateView(glm::mat4& _view);

void UpdateProjection(glm::mat4& _projection);

GLuint CreateLight();

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


	//vertex and fragment shader paths
	const char* modelVertexSourcePath = "C:\\Users\\bgyas\\source\\repos\\OpenGLCourseApp\\OpenGLCourseApp-16-Model Loading\\modelVertexShaderSource.vs";
	const char* modelFragmentSourcePath = "C:\\Users\\bgyas\\source\\repos\\OpenGLCourseApp\\OpenGLCourseApp-16-Model Loading\\modelFragmentShaderSource.fs";

	//crate shader program
	Shader ModelShader = Shader(modelVertexSourcePath, modelFragmentSourcePath);
	//create model
	const char* modelPath = "C:\\Users\\bgyas\\source\\repos\\OpenGLCourseApp\\OpenGLCourseApp-16-Model Loading\\Backpack Model\\backpack.obj";
	Model Backpack = Model(modelPath);	



	//3D view
	//model matrix of backpack
	glm::mat4 backpackModel(1.0f);
	glm::vec3 backpackPos(0.0f);
	backpackModel = glm::scale(backpackModel, glm::vec3(0.5f));
	backpackModel = glm::translate(backpackModel, backpackPos);

	//view matrix
	glm::mat4 view;
	view = glm::lookAt(camera.GetPos(),	//camera position
		camera.GetPos() + camera.GetFront(),	//target
		camera.GetUp());	//up vector

	//projection matrix
	glm::mat4 projection(1.0f);		//FOV, Aspect Ratio (W/H), Nearest point, furthest point (objects between these points will be rendered)
	projection = glm::perspective(camera.GetFov(), static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 100.0f);
	//	gl_position = Mprojection * Mview * Mmodel * Vlocation		//these matrix multipications made for drawing things the 3D plane
	//3D view


	//light 
	//vertex and fragment shader path
	const char* lightVertexSourcePath = "C:\\Users\\bgyas\\source\\repos\\OpenGLCourseApp\\OpenGLCourseApp-16-Model Loading\\lightVertexShaderSource.vs";
	const char* lihtFragmentSourcePath = "C:\\Users\\bgyas\\source\\repos\\OpenGLCourseApp\\OpenGLCourseApp-16-Model Loading\\lightFragmentShaderSource.fs";

	Shader LightShader(lightVertexSourcePath, lihtFragmentSourcePath);
	
	//light model
	glm::mat4 lightModel(1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 1.0f,	5.0f);
	lightModel = glm::translate(lightModel, lightPos);
	lightModel = glm::scale(lightModel, glm::vec3(0.3f));
	
	GLuint lightVAO = CreateLight();
	//light

	glfwSetKeyCallback(mainWindow, KeyCallback);

	//set cursor posiiton call back
	glfwSetCursorPosCallback(mainWindow, CursorPosCallBack);

	//disable cursor for our window		
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//set mouse scroll call back
	glfwSetScrollCallback(mainWindow, ScrollCallBack);

	//flit textures on y axis
	stbi_set_flip_vertically_on_load(true);

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

		//std::cout << "delta time: " << deltaTime << std::endl;

		//print fps
		fps++;
		if (currentTime - lastSecond >= 1.0) {
			lastSecond = currentTime;
			std::cout << "fps: " << fps << std::endl;
			fps = 0;
		}

		//input
		ProcessInput(mainWindow);

		//update view matrice to move and look effect
		UpdateView(view);
		//update projection matrice to zoom effect
		UpdateProjection(projection);

		//clear window with a color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		//clear color and buffer bit
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//backpack shader
		ModelShader.use();
		//3D view
		ModelShader.SetMat4f("model", 1, GL_FALSE, backpackModel);
		ModelShader.SetMat4f("view", 1, GL_FALSE, view);
		ModelShader.SetMat4f("projection", 1, GL_FALSE, projection);
		//lighting
		ModelShader.SetFloat3("viewPos", camera.GetPos().x, camera.GetPos().y, camera.GetPos().z);
		ModelShader.SetBool("directionalLightOn", directionalLightOn);
		ModelShader.SetBool("flashlightOn", flashlightOn);
		//	directional light
		ModelShader.SetFloat3("directionalLight.position", lightPos.x, lightPos.y, lightPos.z);
		ModelShader.SetFloat3("directionalLight.ambient", 0.1f, 0.1f, 0.1f);
		ModelShader.SetFloat3("directionalLight.diffuse", 1.0f, 1.0f, 1.0f);
		ModelShader.SetFloat3("directionalLight.specular", 1.0f, 1.0f, 1.0f);
		ModelShader.SetFloat("directionalLight.shinines", 64.0f);
		//	flashlight
		ModelShader.SetFloat3("flashDirectioanlLight.position", camera.GetPos().x, camera.GetPos().y, camera.GetPos().z);
		ModelShader.SetFloat3("flashDirectioanlLight.ambient", 0.0f, 0.0f, 0.0f);
		ModelShader.SetFloat3("flashDirectioanlLight.diffuse", 1.0f, 1.0f, 1.0f);
		ModelShader.SetFloat3("flashDirectioanlLight.specular", 1.0f, 1.0f, 1.0f);
		ModelShader.SetFloat("flashDirectioanlLight.shinines", 64.0f);
		//	flashlight -> point light
		ModelShader.SetFloat("flashPointLight.kConstant", 1.0f);	//1.0, 0.09, 0.032 - 50 meters
		ModelShader.SetFloat("flashPointLight.kLinear", 0.09f);
		ModelShader.SetFloat("flashPointLight.kQuadratic", 0.032f);	
		ModelShader.SetFloat3("flashPointLight.position", camera.GetPos().x, camera.GetPos().y, camera.GetPos().z);
		//	flashlight -> spotlight
		ModelShader.SetFloat3("flashSoftSpotlight.position", camera.GetPos().x, camera.GetPos().y, camera.GetPos().z);
		ModelShader.SetFloat3("flashSoftSpotlight.direction", camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		ModelShader.SetFloat("flashSoftSpotlight.cutOff", glm::cos(glm::radians(12.5f)));	//0.976
		ModelShader.SetFloat("flashSoftSpotlight.cutOffOuter", glm::cos(glm::radians(17.5f)));	//0.954
		//backpack model object
		Backpack.Draw(ModelShader);

		//light shader
		LightShader.use();
		LightShader.SetMat4f("model", 1, GL_FALSE, lightModel);
		LightShader.SetMat4f("view", 1, GL_FALSE, view);
		LightShader.SetMat4f("projection", 1, GL_FALSE, projection);
		LightShader.SetBool("directionalLightOn", directionalLightOn);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

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

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case(GLFW_KEY_F): {
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !flashlightOn) {
			flashlightOn = true;
		}

		else if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && flashlightOn){
			flashlightOn = false;
		}
	}

	case(GLFW_KEY_SPACE): {
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !directionalLightOn) {
			directionalLightOn = true;
		}
		
		else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && directionalLightOn) {
			directionalLightOn = false;
		}
	}

	default:
		break;
	}
}

void CursorPosCallBack(GLFWwindow* window, GLdouble xPosIn, GLdouble yPosIn) {
	camera.UpdateDirection(window, xPosIn, yPosIn);
}

void ScrollCallBack(GLFWwindow* window, GLdouble offsetX, GLdouble offsetY) {
	camera.UpdateZoom(window, offsetX, offsetY);
}

//update view matrice
void UpdateView(glm::mat4& _view)
{
	_view = glm::lookAt(camera.GetPos(),	//camera position
		camera.GetPos() + camera.GetFront(),	//target
		camera.GetUp());	//up vector
}

//update projection matrice
void UpdateProjection(glm::mat4& _projection)
{
	_projection = glm::perspective(camera.GetFov(),	//fov
		static_cast<float>(WIDTH) / static_cast<float>(HEIGHT),	//aspect ratio
		0.1f, 100.0f);	//nearest and furthest position
}

GLuint CreateLight()
{
	GLuint VAO, VBO;
	
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 8, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}
