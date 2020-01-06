#pragma once

#include <GLFW/glfw3.h>
#include <iostream>

class Input
{
	public:
		GLFWwindow* window;

	public:

		Input(GLFWwindow* w) { 
			this->window = w;
	    	glfwSetWindowUserPointer(w, this);

			//glfwSetCursorPosCallback(w, ProcessMouseCB);

			auto func = [](GLFWwindow* w, double x, double y)
			{
				static_cast<Input*>(glfwGetWindowUserPointer(w))->ProcessMouse(x, y);
			};

			glfwSetCursorPosCallback(w, func);

    		glfwSetKeyCallback(w, ProcessKeyboardCB);
		}

		virtual ~Input() {}

		void ProcessMouse(double xpos, double ypos);
		void ProcessKeyboard(int key, int scancode, int action, int mods);

		static void ProcessMouseCB(GLFWwindow* window, double xpos, double ypos)
		{
			Input* obj = static_cast<Input*>(glfwGetWindowUserPointer(window));
			obj->ProcessMouse(xpos, ypos);
		}

		static void ProcessKeyboardCB(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			Input* obj = static_cast<Input*>(glfwGetWindowUserPointer(window));
			obj->ProcessKeyboard(key, scancode, action, mods);
		}

	public:
		bool forward = false;
		bool backward = false;
		bool left = false;
		bool right = false;
		bool up = false;
		bool down = false;

		double pxpos = -1;
		double pypos = -1;

		float mdx = 0;
		float mdy = 0;

		bool stop_motion = false;

};
