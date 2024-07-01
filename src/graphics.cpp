#include "graphics.hpp"

#include <iostream>
#include <format>
#include <fstream>
#include <sstream>
#include <vector>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>



unsigned int CreateVertexArray()
{
    unsigned int va;
    glGenVertexArrays(1, &va);
    glBindVertexArray(va);
    return va;
}

void BindVertexArray(unsigned int vao)
{
    glBindVertexArray(vao);
}

void AddVertexAttrib(unsigned int vao, int index, VertexAttribType type, unsigned int count,
                     unsigned int stride, unsigned int offset)
{
    glBindVertexArray(vao);
    glVertexAttribPointer(index, count, (unsigned int)type, GL_FALSE, stride, (void*)(uintptr_t)offset);
    glEnableVertexAttribArray(index);
}


unsigned int CreateVertexBuffer(void* data, unsigned int size)
{
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    return vbo;
}

void BindVertexBuffer(unsigned int vbo)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

unsigned int CreateIndexBuffer(void* data, unsigned int size)
{
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    return ibo;
}

void BindIndexBuffer(unsigned int ibo)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

unsigned int SizeOfType(VertexAttribType type)
{
    switch(type)
    {
        case VertexAttribType::BYTE: return 1;
        case VertexAttribType::INT: return sizeof(int);
        case VertexAttribType::UINT: return sizeof(unsigned int);
        case VertexAttribType::FLOAT: return sizeof(float);
        default: return 0;
    }
}

