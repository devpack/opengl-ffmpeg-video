#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

/*---------------------------------------------------------------------------*/

class Camera
{

public:
	Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar, float mouse_sensitivity, float keyboard_sensitivity)
	{
                this->mouse_sensitivity = mouse_sensitivity;
                this->keyboard_sensitivity = keyboard_sensitivity;
                
		this->pos = pos;
		this->front = glm::vec3(0.0f, 0.0f, -1.0f);
		this->up = glm::vec3(0.0f, 1.0f, 0.0f);
                this->worldup = this->up;

                this->yaw = -90.0f;
                this->pitch = 0.0f; 

		this->projection = glm::perspective(fov, aspect, zNear, zFar);

                updateCameraVectors();
	}

        /*-------------------------------------------------------------------*/

	inline glm::mat4 GetViewProjection() const
	{
		return this->projection * glm::lookAt(this->pos, this->pos + this->front, this->up);
	}

        /*-------------------------------------------------------------------*/

        void updateCameraVectors()
        {
                // Calculate the new Front vector
                glm::vec3 new_front;
                new_front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
                new_front.y = sin(glm::radians(this->pitch));
                new_front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
                this->front = glm::normalize(new_front);

                // Also re-calculate the Right and Up vector
                this->right = glm::normalize(glm::cross(this->front, this->worldup));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
                this->up    = glm::normalize(glm::cross(this->right, this->front));
        }

        /*-------------------------------------------------------------------*/

        void ProcessMouse(float xoffset, float yoffset, bool constrainPitch)
        {
                xoffset *= this->mouse_sensitivity;
                yoffset *= this->mouse_sensitivity;
                
                this->yaw   += xoffset;
                this->pitch -= yoffset;

                //std::cout << this->yaw << std::endl;

                // avoid screen flip (pitch is out of bounds)
                if (constrainPitch)
                {
                        if (this->pitch > 89.0f)
                                this->pitch = 89.0f;
                        if (this->pitch < -89.0f)
                                this->pitch = -89.0f;
                }

                updateCameraVectors();
        }

        /*-------------------------------------------------------------------*/

        void ProcessKeyboard(bool forward, bool backward, bool left, bool right, bool up, bool down, float deltaTime)
        {
                float velocity = this->keyboard_sensitivity * deltaTime;

                this->right = glm::normalize(glm::cross(this->up, this->front));

                if (forward)
                        this->pos += this->front * velocity;
                if (backward)
                        this->pos -= this->front * velocity;
                if (left)
                        this->pos += this->right * velocity;
                if (right)
                        this->pos -= this->right * velocity;
                if (up)
                        this->pos.y += velocity;
                if (down)
                        this->pos.y -= velocity;
        }

private:
        float mouse_sensitivity;
        float keyboard_sensitivity;

	glm::mat4 projection;

	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 worldup;

	float yaw;
	float pitch;
};
