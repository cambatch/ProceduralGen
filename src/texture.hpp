#include <glad/gl.h>
#include <stb_image.h>

#include <vector>


struct Texture
{
    Texture();
    Texture(unsigned int id, int width, int height);
    Texture(const char* path);
    Texture(Texture&& tex)
        : id(tex.id), width(tex.width), height(tex.height)
    {
        tex.id = 0;
        tex.width = 0;
        tex.height = 0;
    }

    Texture& operator=(Texture&& tex)
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

    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    unsigned int id;
    int width;
    int height;
};

Texture* CreateTextureFromFloats(int width, int height, const std::vector<float>& data);
Texture* CreateTextureFromBytes(int width, int height, const std::vector<uint8_t>& data);
