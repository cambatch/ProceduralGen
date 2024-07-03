#include "shader.hpp"

#include <iostream>
#include <vector>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include "util.hpp"


Shader::Shader(const char* vertPath, const char* fragPath)
{
    id = CreateShader(vertPath, fragPath);
}

Shader::~Shader()
{
    glDeleteProgram(id);
}

unsigned int Shader::CreateShader(const char* vertFile, const char* fragFile)
{
    unsigned int vert, frag;

    vert = glCreateShader(GL_VERTEX_SHADER);
    frag = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertCode = ReadFile(vertFile);
    std::string fragCode = ReadFile(fragFile);

    const char* vertSource = vertCode.c_str();
    const char* fragSource = fragCode.c_str();

    glShaderSource(vert, 1, &vertSource, nullptr);
    glShaderSource(frag, 1, &fragSource, nullptr);

    glCompileShader(vert);
    glCompileShader(frag);

    CheckShaderCompile(vert, GL_VERTEX_SHADER);
    CheckShaderCompile(frag, GL_FRAGMENT_SHADER);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert);
    glAttachShader(shaderProgram, frag);
    glLinkProgram(shaderProgram);

    CheckShaderLink(shaderProgram);

    return shaderProgram;
}

void Shader::Bind() const
{
    glUseProgram(id);
}


void Shader::SetUniform(const char* name, float val)
{
	glUniform1f(GetUniformLocation(*this, name), val);
}

void Shader::SetUniform(const char* name, int val)
{
	glUniform1i(GetUniformLocation(*this, name), val);
}

void Shader::SetUniform(const char* name, const glm::vec3& vec)
{
	glUniform3fv(GetUniformLocation(*this, name), 1, glm::value_ptr(vec));
}

void Shader::SetUniform(const char* name, float x, float y, float z)
{
	glUniform3f(GetUniformLocation(*this, name), x, y, z);
}

void Shader::SetUniform(const char* name, const glm::vec4& vec)
{
	glUniform4fv(GetUniformLocation(*this, name), 1, glm::value_ptr(vec));
}

void Shader::SetUniform(const char* name, float x, float y, float z, float w)
{
	glUniform4f(GetUniformLocation(*this, name), x, y, z, w);
}

void Shader::SetUniform(const char* name, const glm::mat4& mat)
{
	glUniformMatrix4fv(GetUniformLocation(*this, name), 1, GL_FALSE, glm::value_ptr(mat));
}

int Shader::GetUniformLocation(Shader& shader, const char* name) {
	auto itemIter = shader.Uniforms.find(name);

	if (itemIter != shader.Uniforms.end())
	{
		return itemIter->second;
	}

	int loc = glGetUniformLocation(shader.id, name);

	if(loc == -1)
	{
	    static bool done = false;
	    if(!done)
	    {
	        done = true;
	        std::cerr << "Uniform: " << name << " does not exist!\n";
	    }
		return loc;
	}

	shader.Uniforms[name] = loc;
	return loc;
}

void Shader::CheckShaderCompile(unsigned int shader, unsigned int type)
{
    int val;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &val);
    if(!val)
    {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &val);
        std::vector<char> message(val);
        glGetShaderInfoLog(shader, val, &val, message.data());
        std::cerr << "Error compiling " << ShaderTypeToName(type) << ": " << message.data() << '\n';
    }
}

void Shader::CheckShaderLink(unsigned int shader)
{
    int val;
    glGetProgramiv(shader, GL_LINK_STATUS, &val);
    if(!val)
    {
        glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &val);
        std::vector<char> message(val);
        glGetShaderInfoLog(shader, val, &val, message.data());
        std::cerr << "Error linking shader: " << message.data() << '\n';
    }
}


const char* Shader::ShaderTypeToName(unsigned int type)
{
    switch(type)
    {
        case GL_VERTEX_SHADER: return "vertex shader";
        case GL_FRAGMENT_SHADER: return "fragment shader";
        default: return "UNKNOWN SHADER TYPE";
    }
}
