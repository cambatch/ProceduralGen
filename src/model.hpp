#pragma once

#include <string_view>
#include <iostream>
#include <format>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "graphics.hpp"
#include "mesh.hpp"


class Model
{
public:
    std::vector<Texture> Textures;
    std::vector<Mesh> Meshes;
    bool GammaCorrection;

    std::string directory;

    Model(const char* path, bool gamma = false)
    {
        LoadModel(path);
    }

    void Draw(Shader& shader)
    {
        for(auto& mesh : Meshes)
        {
            mesh.Draw(shader);
        }
    }

private:
    void LoadModel(std::string_view path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << std::format("Error loading model: '{}'\n", path);
            return;
        }

        directory = (std::string)path.substr(0, path.find_last_of('/'));
        ProcessNode(scene->mRootNode, scene);
    }

    void ProcessNode(aiNode* node, const aiScene* scene)
    {
        for(unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            Meshes.push_back(processMesh(mesh, scene));
        }

        for(unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        
        // walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> texs = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, texs);
    }
    
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType texType)
    {
        std::vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            // std::string path = "assets/models/backpack/" + (std::string)str.C_Str();

            Texture texture = CreateTexture((directory + '/' +  str.C_Str()).c_str());

            // Texture texture = CreateTexture(str.C_Str());
            texture.type = texType;
            Textures.push_back(texture);
        }
        return textures;
    }};


