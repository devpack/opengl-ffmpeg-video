#include "shader.h"
#include <iostream>
#include <fstream>
#include <memory>

#include <GL/glew.h>

/*---------------------------------------------------------------------------*/

Shader::Shader(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
{
    // assign our program handle a "name"
	programID = glCreateProgram();

    // creates and compiles vertex/fragment shaders
	vertexShaderID = CreateShader(LoadShader(vertexShaderFilename), GL_VERTEX_SHADER);
	fragmentShaderID = CreateShader(LoadShader(fragmentShaderFilename), GL_FRAGMENT_SHADER);

    // attach our shaders to our program
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);

    // bind attribute index 0 (coordinates) to "position"
    // attribute locations must be setup before calling glLinkProgram.
	//glBindAttribLocation(programID, 0, "position");

    // link: shader => binary code uploaded to the GPU, if there is no error
	glLinkProgram(programID);
	CheckShaderError(programID, GL_LINK_STATUS, true, "Error linking shader program");

    // checks to see whether the executables contained in program can execute given the current OpenGL state
	glValidateProgram(programID);
	CheckShaderError(programID, GL_LINK_STATUS, true, "Invalid shader program");

    //mvpID = glGetUniformLocation(program, "mvp");
}

/*---------------------------------------------------------------------------*/

Shader::~Shader()
{
    glDetachShader(programID, vertexShaderID);
    glDeleteShader(vertexShaderID);

    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(fragmentShaderID);

	glDeleteProgram(programID);
}

/*---------------------------------------------------------------------------*/

GLuint Shader::CreateShader(const std::string& text, unsigned int type)
{
    // shader handle
    GLuint shader = glCreateShader(type);

    if(shader == 0)
		std::cerr << "Error compiling shader type " << type << std::endl;

    const GLchar* p[1];
    p[0] = text.c_str();
    GLint lengths[1];
    lengths[0] = text.length();

    glShaderSource(shader, 1, p, lengths);
    glCompileShader(shader);

    CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!");

    return shader;
}

/*---------------------------------------------------------------------------*/

void Shader::setMat4(const std::string &name, const glm::mat4 &mat)
{
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

/*---------------------------------------------------------------------------*/

void Shader::setInt(const std::string &name, int value)
{ 
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value); 
}

/*---------------------------------------------------------------------------*/

void Shader::Use()
{
	glUseProgram(programID);
}

/*---------------------------------------------------------------------------*/

std::string Shader::LoadShader(const std::string& fileName)
{
    std::ifstream file;
    file.open((fileName).c_str());

    std::string output;
    std::string line;

    if(file.is_open())
    {
        while(file.good())
        {
            getline(file, line);
			output.append(line + "\n");
        }
    }
    else
    {
		std::cerr << "Unable to load shader: " << fileName << std::endl;
    }

    return output;
}

/*---------------------------------------------------------------------------*/

void Shader::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
    GLint success = 0;
    GLchar error[1024] = { 0 };

    if(isProgram)
        glGetProgramiv(shader, flag, &success);
    else
        glGetShaderiv(shader, flag, &success);

    if(success == GL_FALSE)
    {
        if(isProgram)
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        else
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);

        std::cerr << errorMessage << ": '" << error << "'" << std::endl;
    }
}
