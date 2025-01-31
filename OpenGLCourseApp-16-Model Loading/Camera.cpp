#include <glm/glm.hpp>
#include "Camera.h"

Camera::Camera(glm::vec3 _pos, glm::vec3 _front, glm::vec3 _up)
{
	pos = glm::vec3(_pos);
	front = glm::vec3(_front);
	up = glm::vec3(_up);
}

void Camera::UpdateDirection(GLFWwindow* window, double xPosIn, double yPosIn)
{
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

	front = glm::normalize(direction);
}

void Camera::UpdateZoom(GLFWwindow* window, double offsetX, double offsetY)
{
	zoom -= static_cast<float>(offsetY);	//narrow down fov according to scroll y offset
	if (zoom < 1.0f)
		zoom = 1.0f;
	if (zoom > 45.0f)
		zoom = 45.0f;
	fov = zoom;
}

