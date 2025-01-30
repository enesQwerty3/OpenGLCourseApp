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
#include "stb_image.h"
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

bool backspace = false;
bool lightOn = false;
bool rArrow = false;
int lightype = 0;
void ChangeLightSource();
void UpdateLight();

//Resize windows size if windows size changed.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//Process user inputs.
void ProcessInput(GLFWwindow* window);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

//when mouse changed position this call back function will be called
void CursorPosCallBack(GLFWwindow* window, GLdouble xPosIn, GLdouble yPosIn);

//
void ScrollCallBack(GLFWwindow* window, GLdouble offsetX, GLdouble offsetY);

//create VAO and VBO
void GenerateBuffers(GLuint& cubeVAO, GLuint& lightVAO);

//
void CreateTexture(GLuint& texture, const char* path);

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

	GLfloat angle = 0.0f;

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
	GLuint CubeVAO, LightVAO;

	//vertex and fragment shader paths
	const char* cubeVertexSourcePath = "C:\\Users\\bgyas\\source\\repos\\OpenGLCourseApp\\OpenGLCourseApp-15-Multiple Lights\\cubeVertexShaderSource.vs";
	const char* cubeFragmentSourcePath = "C:\\Users\\bgyas\\source\\repos\\OpenGLCourseApp\\OpenGLCourseApp-15-Multiple Lights\\cubeFragmentShaderSource.fs";

	const char* lightVertexSourcePath = "C:\\Users\\bgyas\\source\\repos\\OpenGLCourseApp\\OpenGLCourseApp-15-Multiple Lights\\lightVertexShaderSource.vs";
	const char* lightFragmentSourcePath = "C:\\Users\\bgyas\\source\\repos\\OpenGLCourseApp\\OpenGLCourseApp-15-Multiple Lights\\lightFragmentShaderSource.fs";


	//crate shader program
	Shader CubeShader = Shader(cubeVertexSourcePath, cubeFragmentSourcePath);
	Shader LightShader = Shader(lightVertexSourcePath, lightFragmentSourcePath);
	//generate buffers

	GenerateBuffers(CubeVAO, LightVAO);

	GLuint woodenContainer, containerFrame;

	CreateTexture(woodenContainer, "container2.png");
	CreateTexture(containerFrame, "container3.png");

	//	gl_position = Mprojection * Mview * Mmodel * Vlocation		//these matrix multipications made for drawing things the 3D plane

	//3D view
	//model matrix of cube
	glm::mat4 cubeModel(1.0f);
	//model = glm::rotate(model, glm::radians(-70.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	//model matrix of light source
	glm::mat4 lightModel(1.0f);
	glm::vec3 lightPos(0.0f, 0.0f, 2.0f);
	lightModel = glm::translate(lightModel, lightPos);
	lightModel = glm::scale(lightModel, glm::vec3(0.2f));
	glm::vec3 lightColor(1.0f);


	//view matrix
	glm::mat4 view;
	view = glm::lookAt(camera.GetPos(),	//camera position
		camera.GetPos() + camera.GetFront(),	//target
		camera.GetUp());	//up vector

	//projection matrix
	glm::mat4 projection(1.0f);		//FOV, Aspect Ratio (W/H), Nearest point, furthest point (objects between these points will be rendered)
	projection = glm::perspective(camera.GetFov(), static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 100.0f);
	//

	glfwSetKeyCallback(mainWindow, KeyCallback);

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

	//cube positons to be drawn
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

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

		//use cube shader program
		CubeShader.use();
		//3D view attributes
		CubeShader.SetMat4f("model", 1, GL_FALSE, cubeModel);
		CubeShader.SetMat4f("view", 1, GL_FALSE, view);
		CubeShader.SetMat4f("projection", 1, GL_FALSE, projection);
		
		//viewPos
		CubeShader.SetFloat3("viewPos", camera.GetPos().x, camera.GetPos().y, camera.GetPos().z);

		//light struct proprties
		//directional light properties
		CubeShader.SetFloat3("directionalLight.position", lightPos.x, lightPos.y, lightPos.z);
		CubeShader.SetFloat3("directionalLight.direction", 0.0f, 0.0f, -1.0f);
		CubeShader.SetFloat3("directionalLight.ambient", 0.2, 0.2, 0.2); // note that all light colors are set at full intensity
		CubeShader.SetFloat3("directionalLight.diffuse", lightColor.x, lightColor.y, lightColor.z);
		CubeShader.SetFloat3("directionalLight.specular", lightColor.x, lightColor.y, lightColor.z);
		CubeShader.SetFloat("directionalLight.shininess", 64.0f);

		//point light properties
		CubeShader.SetFloat3("pointLight.position", lightPos.x, lightPos.y, lightPos.z);
		CubeShader.SetFloat3("pointLight.direction", 0.0f, 0.0f, -1.0f);
		//these attenuation values for 50 meters maximum
		CubeShader.SetFloat("pointLight.kConstant", 1.0f);
		CubeShader.SetFloat("pointLight.kLinear", 0.09f);
		CubeShader.SetFloat("pointLight.kQuadratic", 0.032f);
		
		//soft spotlight properties
		CubeShader.SetFloat3("softSpotlight.position", lightPos.x, lightPos.y, lightPos.z);
		CubeShader.SetFloat3("softSpotlight.direction", 0.0f, 0.0f, -1.0f);
		CubeShader.SetFloat("softSpotlight.cutOff", glm::cos((glm::radians(12.5f))));
		CubeShader.SetFloat("softSpotlight.cutOffOuter", glm::cos((glm::radians(17.5f))));
		
		//set texture units for samplers
		CubeShader.SetInt("diffuseT", 0);
		CubeShader.SetInt("specularT", 1);

		CubeShader.SetInt("lightype", lightype);
		CubeShader.SetInt("lightOn", lightOn);;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodenContainer);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, containerFrame);

		glBindVertexArray(CubeVAO);
		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 model(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 10.0f;
			model = glm::rotate(model, glm::radians(angle * i), glm::vec3(0.2f, 1.0f, 0.5f));
			CubeShader.SetMat4f("model", 1, GL_FALSE, model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//glBindVertexArray(0);  // no need to unbind it every time 

		LightShader.use();
		LightShader.SetMat4f("model", 1, GL_FALSE, lightModel);
		LightShader.SetMat4f("view", 1, GL_FALSE, view);
		LightShader.SetMat4f("projection", 1, GL_FALSE, projection);

		glBindVertexArray(LightVAO);
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

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case(GLFW_KEY_SPACE):{
		if (action == GLFW_PRESS) {
			if (backspace)
				backspace = false;
			else
				backspace = true;
				UpdateLight();
		}
	}break;

	case(GLFW_KEY_RIGHT): {
		if (action == GLFW_PRESS && !rArrow) 
			rArrow = true;
		
		else if (action == GLFW_RELEASE && rArrow) {
			rArrow = false;
			ChangeLightSource();
		}
	}break;

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

void GenerateBuffers(GLuint& cubeVAO, GLuint& lightVAO) {
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

	//we will use same VBO for cube and light
	GLuint VBO;
	//bind cube VAO 
	glGenVertexArrays(1, &cubeVAO);
	//generates buffer on GPU's memory and stores it's id in VBO variable
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object after genereating buffers, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//unbind cubeVAO
	glBindVertexArray(0);


	glGenVertexArrays(1, &lightVAO);
	//bind VBO to lightVAO
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	//unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//ubind lightVAO
	glBindVertexArray(0);

	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void CreateTexture(GLuint& texture, const char* path) {
	//generate texture
	glGenTextures(1, &texture);	//texture unit 0
	glBindTexture(GL_TEXTURE_2D, texture);

	//set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;

	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
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

void UpdateLight() {
	if(backspace)
		lightOn = true;

	else
		lightOn = false;
}

void ChangeLightSource() {
	lightype+=1;
	lightype%=3;
}