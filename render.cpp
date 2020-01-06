#include "render.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

/*---------------------------------------------------------------------------*/

Render::Render(int screen_width, int screen_height, std::shared_ptr<VideoLight> video_light, bool use_frambuffer, bool use_video)
{
	this->use_frambuffer = use_frambuffer;
	this->screen_width = screen_width;
	this->screen_height = screen_height;
	this->video_light = video_light;
	this->use_video = use_video;

	// Scene
	// -----

    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };

    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

	// our buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

	// all states saved into our vao
    glBindVertexArray(vao);

	// bind vertex into vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
	// texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

	// unbind our VAO as the current used object: so any operation that would affect a VAO will not affect this particular VAO anymore
	glBindVertexArray(0);

    // load and create a texture 
    // -------------------------
	
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    
	// set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
	// set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(use_video) {
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		std::cout << "Render " << this->video_light->codec_ctx->width << "x" << this->video_light->codec_ctx->height << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->video_light->codec_ctx->width, this->video_light->codec_ctx->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}
	else {
		// load image, create texture and generate mipmaps
		
		int width, height, nrChannels;
		
		unsigned char *data = stbi_load("../image.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}

	// FRAMBUFFER OSR: see https://learnopengl.com/Advanced-OpenGL/Framebuffers
	if(use_frambuffer) {

		/*
		1. Render the scene into a color texture attached to our new custom framebuffer object (bound as the active framebuffer)

			"When attaching a texture to a framebuffer, all rendering commands will write to the texture as if it was a 
			normal color/depth or stencil buffer. The advantage of using textures is that the result of all rendering 
			operations will be stored as a texture image that we can then easily use in our shaders"

		2. Bind to the default framebuffer

		3. Draw a quad that spans the entire screen with the new framebuffer's color buffer as its texture

			For the "quad screen" vertex shader if we supply the coordinates as normalized device coordinates
			we can directly specify them as the output of the vertex shader (no need to apply mvp matrix)
		*/

		float quad_screen[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f
		};

		// screen quad VAO
		// ---------------

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);

		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_screen), &quad_screen, GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		// framebuffer configuration
		// -------------------------

		glGenFramebuffers(1, &custom_framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, custom_framebuffer);
		
		// create a color attachment texture
		glGenTextures(1, &textureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

		// we pass NULL as the texture's data parameter. For this texture, we're only allocating memory and not 
		// actually filling it. Filling the texture will happen as soon as we render to the framebuffer
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// attach it to the framebuffer (GL_COLOR_ATTACHMENT0: the type of attachment we're going to attach : here a color attachment)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
		
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_width, screen_height); // use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
		
		// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindVertexArray(0);

	}

}

/*---------------------------------------------------------------------------*/

Render::~Render()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);

	if(this->use_frambuffer) {
		glDeleteVertexArrays(1, &quadVAO);
		glDeleteBuffers(1, &quadVBO);
		glDeleteBuffers(1, &rbo);
	}
}

/*---------------------------------------------------------------------------*/

void Render::DrawScene()
{
	// bind our VAO as the current used object: so any operation that would affect a VAO will affect this particular VAO
	glBindVertexArray(vao);
    
	// bind Texture
    glBindTexture(GL_TEXTURE_2D, texture);

	// video ?
	if(use_video) {
		// readVideoFrame() set the texture with glTexSubImage2D
		this->video_light->readVideoFrame();
	}

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//glDrawElements(GL_LINES, this->nb_vertices, GL_UNSIGNED_INT, 0);
	
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, this->nb_vertices);
	
	//glDrawArrays(GL_POINTS, 0, this->nb_vertices);

	// unbind our VAO as the current used object: so any operation that would affect a VAO will not affect this particular VAO anymore
	glBindVertexArray(0);
}

/*---------------------------------------------------------------------------*/

void Render::DrawQuadScreen()
{
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}
