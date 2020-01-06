#pragma once

#include <string>
#include <memory>

#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h>

#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
	public:
		Shader(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename);
		virtual ~Shader();

		void Use();
		void setMat4(const std::string &name, const glm::mat4 &mat);
		void setInt(const std::string &name, int value);

	private:
		std::string LoadShader(const std::string& fileName);
		void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
		GLuint CreateShader(const std::string& text, unsigned int type);

		GLuint vertexShaderID;
		GLuint fragmentShaderID;
		GLuint programID;
};
