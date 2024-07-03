#pragma once

#include <unordered_map>

#include <glm/glm.hpp>

class Shader
{
public:
    Shader(const char* vertPath, const char* fragPath);
    ~Shader();

    unsigned int id;

    void Bind() const;

    void SetUniform(const char* name, float val);
    void SetUniform(const char* name, int val);

    void SetUniform(const char* name, const glm::vec3& vec);
    void SetUniform(const char* name, float x, float y, float z);

    void SetUniform(const char* name, const glm::vec4& vec);
    void SetUniform(const char* name, float x, float y, float z, float w);

    void SetUniform(const char* name, const glm::mat4& mat);

    std::unordered_map<const char*, int> Uniforms;

private:
    unsigned int CreateShader(const char* vertFile, const char* fragFile);
    void CheckShaderCompile(unsigned int shader, unsigned int type);
    void CheckShaderLink(unsigned int shader);
    int GetUniformLocation(Shader& shader, const char* name);
    const char* ShaderTypeToName(unsigned int type);
};
