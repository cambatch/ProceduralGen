#pragma once

#include <glm/glm.hpp>
#include <glad/gl.h>


enum class VertexAttribType
{
    // Values from opengl (gl.h)
    INVALID = 0,
    BYTE = 0x1400,
    INT =  0x1404,
    UINT = 0x1405,
    FLOAT = 0x1406,
};

enum class TextureType
{
    UNKNOWN = 0,
    DIFFUSE,
    SPECULAR,
    NORMAL,
    HEIGHT,
};

unsigned int CreateShader(const char* vertFile, const char* fragFile);
void BindShader(unsigned int shader);



unsigned int CreateVertexArray();
void BindVertexArray(unsigned int vao);
void AddVertexAttrib(unsigned int vao, int index, VertexAttribType type, unsigned int count,
                     unsigned int stride, unsigned int offset);



unsigned int CreateVertexBuffer(void* data, unsigned int size);
void BindVertexBuffer(unsigned int vbo);



unsigned int CreateIndexBuffer(void* data, unsigned int size);
void BindIndexBuffer(unsigned int ibo);



