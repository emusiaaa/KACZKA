#include "Camera.h"
#include <cmath>
#include <iostream>

Camera::Camera()
{
	position = glm::vec3(0, 0, 600);
	up = glm::vec3(0, 1, 0);
	target = glm::vec3(0, 0, 0);
	radius = glm::length(position - target);
	updateCameraVectors();
}

glm::mat4 Camera::viewMatrix()
{
	glm::mat4 lookAt = glm::mat4();
	glm::vec3 direction = glm::normalize(position - target);
	glm::vec3 right = this->right();
	
	glm::vec3 up2 = glm::normalize(glm::cross(direction, right));
	lookAt[0][0] = right.x;
	lookAt[0][1] = right.y;
	lookAt[0][2] = right.z;
	lookAt[0][3] = - glm::dot(right, position);
	lookAt[1][0] = up2.x;
	lookAt[1][1] = up2.y;
	lookAt[1][2] = up2.z;
	lookAt[1][3] = - glm::dot(up2, position);
	lookAt[2][0] = direction.x;
	lookAt[2][1] = direction.y;
	lookAt[2][2] = direction.z;
	lookAt[2][3] = - glm::dot(direction, position);
	lookAt[3][3] = 1;
	//lookAt[0][0] = right.x;
	//lookAt[1][0] = right.y;
	//lookAt[2][0] = right.z;
	//lookAt[0][1] = up2.x;
	//lookAt[1][1] = up2.y;
	//lookAt[2][1] = up2.z;
	//lookAt[0][2] = -front.x;
	//lookAt[1][2] = -front.y;
	//lookAt[2][2] = -front.z;
	//lookAt[3][0] = -glm::dot(right, position);
	//lookAt[3][1] = -glm::dot(up2, position);
	//lookAt[3][2] =  glm::dot(front, position);
	//lookAt[3][3] =  1;
	return glm::transpose(lookAt);
	
	//return glm::lookAt(position, position +  front, up);
}

glm::mat4 Camera::projectionMatrix(float height, float width)
{
	glm::mat4 perspective = glm::mat4();
	float ctg = 1.0f / tan(glm::radians(fov) / 2.0f);
	perspective[0][0] = ctg / (width/ height);
	perspective[1][1] = ctg;
	perspective[2][2] = - (depthFar + depthNear) / (depthFar - depthNear);
	perspective[2][3] = -1.0f;
	perspective[3][2] = (-2.0f*depthFar * depthNear) / (depthFar - depthNear);
	return perspective;
	//return glm::perspective(glm::radians(fov), width / height, depthNear, depthFar);
}

glm::mat4 Camera::orthoProjectionMatrix()
{
	return glm::mat4();
}

void Camera::processMouseMovement(float xoffset, float yoffset)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f) pitch = 89.0f;
	else if (pitch < -89.0f) pitch = -89.0f;

	yaw = fmod(yaw, 360);

	updateCameraVectors();
}

void Camera::updatePosition(CameraMovement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;

	switch (direction)
	{
	case Left:
		target -= right() * velocity;
		break;

	case Right:
		target += right() * velocity;
		break;

	case Forward:
		target -= glm::cross(right(), glm::normalize(position-target)) * velocity;
		break;

	case Backward:
		target += glm::cross(right(), glm::normalize(position - target)) * velocity;
		break;
	}
	position.y = 0;

	updateCameraVectors();
}

glm::vec3 Camera::right()
{
	return glm::normalize(glm::cross(up, glm::normalize(position - target)));
}

void Camera::updateCameraVectors()
{
	position.x = target.x + radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	position.y = target.y + radius * sin(glm::radians(pitch));
	position.z = target.z + radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	/*front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(front);
	up = glm::normalize(glm::cross(glm::cross(front, glm::vec3(0,1,0)), front));*/
}

void Camera::zoom(float zoom)
{
	float radius = glm::length(position - target) - zoom;
	front.x = radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = radius * sin(glm::radians(pitch));
	front.z = radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);
}
