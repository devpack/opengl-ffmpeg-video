#include "input.h"

void Input::ProcessMouse(double xpos, double ypos)
    {
		//std::cout << xpos << std::endl;

		if(pxpos != -1) {
			mdx = xpos - pxpos;
		}
		if(pypos != -1) {
			mdy = ypos - pypos;
		}

		pxpos = xpos;
		pypos = ypos;

		//std::cout << mdx << std::endl;
    }

void Input::ProcessKeyboard(int key, int scancode, int action, int mods)
    {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(this->window, GL_TRUE);
		}

		// KEY PRESS
		if (glfwGetKey(this->window, GLFW_KEY_UP) == GLFW_PRESS) {
			forward = true;
		}
		if (glfwGetKey(this->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			backward = true;
		}
		if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			stop_motion = !stop_motion;
		}
		if (glfwGetKey(this->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			left = true;
		}
		if (glfwGetKey(this->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			right = true;
		}
		if (glfwGetKey(this->window, GLFW_KEY_Q) == GLFW_PRESS) {
			up = true;
		}
		if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS) {
			down = true;
		}

		// KEY RELEASE
		if (glfwGetKey(this->window, GLFW_KEY_UP) == GLFW_RELEASE) {
			forward = false;
		}
		if (glfwGetKey(this->window, GLFW_KEY_DOWN) == GLFW_RELEASE) {
			backward = false;
		}
		if (glfwGetKey(this->window, GLFW_KEY_LEFT) == GLFW_RELEASE) {
			left = false;
		}
		if (glfwGetKey(this->window, GLFW_KEY_RIGHT) == GLFW_RELEASE) {
			right = false;
		}
		if (glfwGetKey(this->window, GLFW_KEY_Q) == GLFW_RELEASE) {
			up = false;
		}
		if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_RELEASE) {
			down = false;
		}
    }