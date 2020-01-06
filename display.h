#pragma once

#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/*---------------------------------------------------------------------------*/

class MyDisplay
{
public:
       int screen_width;
       int screen_height;

       GLFWwindow* mainWindow;
       GLFWmonitor** monitors;

       int nbMonitor;

public:
       MyDisplay(int screen_width, int screen_height, bool fullscreen, bool vsync);
       virtual ~MyDisplay();

       void SetNativeFullscreen(bool fullscreen);
       void SetNativeFullscreenWindows(bool fullscreen);

       void Clear(float r, float g, float b, float a);
       void SwapBuffers();
};

