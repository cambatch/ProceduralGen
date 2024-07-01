#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "graphics.hpp"

#include <glad/gl.h>


struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int vao;
    unsigned int vbo;
    unsigned int ibo;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        SetupMesh();
    }

    void SetupMesh()
    {
        vao = CreateVertexArray();
        vbo = CreateVertexBuffer(vertices.data(), sizeof(Vertex) * vertices.size());
        ibo = CreateIndexBuffer(indices.data(), sizeof(unsigned int) * indices.size());

        unsigned int stride = sizeof(Vertex);
        unsigned int offset = 0;

        AddVertexAttrib(vao, 0, VertexAttribType::FLOAT, 3, stride, 0);
        offset += sizeof(vertices[0].Position);
        AddVertexAttrib(vao, 1, VertexAttribType::FLOAT, 3, stride, offset);
        offset += sizeof(vertices[0].Normal);
        AddVertexAttrib(vao, 2, VertexAttribType::FLOAT, 2, stride, offset);
    };

    void Draw(Shader& shader)
    {
        for(int i = 0; i < textures.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);

            switch(textures[i].type)
            {
                case TextureType::DIFFUSE:
                    shader.SetUniform("tex_diffuse", i);
                    break;
                case TextureType::SPECULAR:
                    break;
                case TextureType::NORMAL:
                    break;
                case TextureType::HEIGHT:
                    break;
                case TextureType::UNKNOWN:
                    break;
            }
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        BindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glActiveTexture(GL_TEXTURE0);
    }
};
