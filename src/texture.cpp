#include "texture.hpp"

#include <glad/gl.h>

#include <iostream>
#include <vector>


Texture::Texture()
{
    id = 0;
    width = 0;
    height = 0;
}

Texture::Texture(unsigned int id, int width, int height)
{
    this->id = id;
    this->width = width;
    this->height = height;
}

Texture::Texture(const char* path)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned int id;
    glGenTextures(1, &id);

    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    if(data)
    {
        unsigned int format;
        switch(channels)
        {
            case 1: format = GL_RED; break;
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
        }

        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cerr << "Failed to load texture at path: " << path << '\n';
    }

    stbi_image_free(data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}
Texture& Texture::operator=(Texture&& tex)
{
    if(this != &tex)
    {
        glDeleteTextures(1, &id);
        id = tex.id;
        width = tex.width;
        height = tex.height;

        tex.id = 0;
        tex.width = 0;
        tex.height = 0;
    }
    return *this;
}

void Texture::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Caller owns texture.
Texture* CreateTextureFromFloats(int width, int height, const std::vector<float>& data)
{
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, data.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return new Texture(id, width, height);
}

Texture* CreateTextureFromBytes(int width, int height, const std::vector<uint8_t>& data)
{
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return new Texture(id, width, height);
}
