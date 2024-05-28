#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
enum CameraMovement
{
	Left,
	Right,
	Up,
	Down,
	Backward,
	Forward
};

class Camera {
public:
	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 target;
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);

	float fov = 45.0f;
	float yaw = 90.0f;
	float pitch = 0.0f;
	float depthNear = 1.f;
	float depthFar = 3000.0f;
	float MouseSensitivity = 0.5f;
	float MovementSpeed = 100.0f;
	float radius;

	Camera();
	glm::mat4 viewMatrix();
	glm::mat4 projectionMatrix(float height, float width);
	glm::mat4 orthoProjectionMatrix();

	void processMouseMovement(float xoffset, float yoffset);
	void updatePosition(CameraMovement direction, float deltaTime);

	glm::vec3 right();
	glm::vec3 up2(){return glm::normalize(glm::cross(glm::normalize(position - target), right())); }
	void updateCameraVectors();
	void zoom(float zoom);
};

