#pragma once

extern const int WIDTH;
extern const int HEIGHT;

struct GLFWwindow;

class Camera {

public:
	Camera(glm::vec3 _pos, glm::vec3 _front, glm::vec3 _up);
	
	inline void SetPos(glm::vec3 _pos) { pos = _pos; }

	inline glm::vec3 GetPos() const{ return pos; }

	inline glm::vec3 GetFront() const{ return front; }

	inline glm::vec3 GetUp() const{ return up; }

	inline float GetFov() const{ return fov;}

	inline float GetMovementSpeed() const{ return cameraMovementSpeed; }

	void UpdateDirection(GLFWwindow* window, double xPosIn, double yPosIn);

	void UpdateZoom(GLFWwindow* window, double offsetX, double offsetY);

private:
	glm::vec3 pos;
	glm::vec3 front;	//somewhere in front of camera always -1 in z
	glm::vec3 up;

	bool firstMove = true;
	float lastX = static_cast<float>(WIDTH) / 2;
	float lastY = static_cast<float>(HEIGHT) / 2;
	float yaw = -90.0f, pitch = 0.0f;	//-90 degree yaw to start camera looking negative z axis and also prevent flipping at the start bc if we set 0 at the start we will be looking positive x axis!
	float fov = 45.0f;	//default fov
	float zoom = fov;	//to prevent flashing effect when first time zooming
	float cameraSensivity = 0.1f;	//mouse sensivity
	float cameraMovementSpeed = 2.5f; //for movement (wasd)

protected:

};