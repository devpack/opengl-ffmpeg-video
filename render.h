#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>

#include "video_light.h"

/*---------------------------------------------------------------------------*/

class Render
{
	public:
		Render(int screen_width, int screen_height, std::shared_ptr<VideoLight> video_light, bool use_frambuffer, bool use_video);
		virtual ~Render();

		void DrawScene();
		void DrawQuadScreen();

	public:
		std::shared_ptr<VideoLight> video_light;
		bool use_frambuffer;
		bool use_video;
       	int screen_width;
       	int screen_height;

	public:
		// Scene attributes
		GLuint vao;
		GLuint vbo;
		GLuint ebo;

    	unsigned int texture;

	public:
		// Custom framebuffer attributes
		unsigned int custom_framebuffer;
		unsigned int textureColorbuffer;
		unsigned int rbo;

		// quad screen 
		unsigned int quadVAO, quadVBO;
};
