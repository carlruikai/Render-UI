#pragma once
#include <vector>
#include<GL/glew.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 30.0f;
const GLfloat SENSITIVITY = 0.25f;
const GLfloat ZOOM = 45.0f;

class Camera {
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	glm::vec3 rotate_axis;

	GLfloat Yaw;
	GLfloat Pitch;
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 axis = glm::vec3(1.0, 0.0, 0.0), GLfloat yaw = YAW, GLfloat pitch = PITCH) :  MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		this->Position = position;
		this->WorldUp = up;
		this->rotate_axis = axis;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
	}



	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->Position, glm::vec3(0.0f, 0.0f, 0.0f), this->Up);
	}

	glm::vec3 GetUpVector()
	{
		return this->Up;
	}

	glm::vec3 GetRightVector()
	{
		return this->Right;
	}
	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->MovementSpeed * deltaTime;
		if (direction == FORWARD)
		{
			glm::mat4 trans(1.0f);
			trans = glm::rotate(trans, glm::radians(-velocity), this->rotate_axis);
			this->Position = glm::vec3(trans * glm::vec4(this->Position, 1.0f));
			this->updateCameraVectors();
		}
		if (direction == BACKWARD)
		{
			glm::mat4 trans(1.0f);
			trans = glm::rotate(trans, glm::radians(velocity), this->rotate_axis);
			this->Position = glm::vec3(trans * glm::vec4(this->Position, 1.0f));
			this->updateCameraVectors();
		}
		if (direction == LEFT)
		{
			glm::mat4 trans(1.0f);
			trans = glm::rotate(trans, glm::radians(-velocity), glm::vec3(0.0, 1.0, 0.0));
			this->Position = glm::vec3(trans * glm::vec4(this->Position, 1.0f));
			this->rotate_axis = glm::vec3(trans * glm::vec4(this->rotate_axis, 1.0f));
			this->updateCameraVectors();
		}
		if (direction == RIGHT)
		{
			glm::mat4 trans(1.0f);
			trans = glm::rotate(trans, glm::radians(velocity), glm::vec3(0.0, 1.0, 0.0));
			this->Position = glm::vec3(trans * glm::vec4(this->Position, 1.0f));
			this->rotate_axis = glm::vec3(trans * glm::vec4(this->rotate_axis, 1.0f));
			this->updateCameraVectors();
		}
	}

	//// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	//void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
	//{
	//	xoffset *= this->MouseSensitivity;
	//	yoffset *= this->MouseSensitivity;

	//	this->Yaw += xoffset;
	//	this->Pitch += yoffset;

	//	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	//	if (constrainPitch)
	//	{
	//		if (this->Pitch > 89.0f)
	//			this->Pitch = 89.0f;
	//		if (this->Pitch < -89.0f)
	//			this->Pitch = -89.0f;
	//	}

	//	// Update Front, Right and Up Vectors using the updated Eular angles
	//	this->updateCameraVectors();
	//}

	// processes input received from a mouse scroll-wheel event. only requires input on the vertical wheel-axis
	void ProcessMouseScroll(GLfloat yoffset)
	{
		if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
			this->Zoom -= yoffset * 0.1;
		if (this->Zoom <= 1.0f)
			this->Zoom = 1.0f;
		if (this->Zoom >= 45.0f)
			this->Zoom = 45.0f;
	}
private:
	void updateCameraVectors()
	{
		glm::vec3 front;
		//front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		//front.y = sin(glm::radians(this->Pitch));
		//front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		front = glm::vec3(0.0, 0.0, 0.0) - this->Position;
		this->Front = glm::normalize(front);
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));
	}
};