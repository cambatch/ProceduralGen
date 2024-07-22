#pragma once

#include <unordered_map>
#include <array>
#include <memory>

#include <glm/glm.hpp>
#include "graphics.hpp"
#include "noise.hpp"


// Chunks are 32x256x32
static const int ChunkSize = 32;
static const int ChunkHeight = 100;
static const int ActiveRadius = 5;


enum class BlockType : uint8_t
{
    AIR = 0,
    DIRT = 1
};


struct Chunk
{
    int32_t X;
    int32_t Y;
    std::array<std::array<std::array<BlockType, ChunkSize>, ChunkHeight>, ChunkSize> Blocks;

    std::vector<float> Vertices;
    std::vector<unsigned int> Indices;
    unsigned int Vao;
    unsigned int Vbo;
    unsigned int Ibo;

    Chunk() : X(0), Y(0), Blocks(), Vao(0), Vbo(0), Ibo(0)
    {}

    Chunk(int32_t x, int32_t y)
        : X(x), Y(y), Blocks(), Vao(0), Vbo(0), Ibo(0)
    {}

    ~Chunk()
    {
        if(Vao) glDeleteVertexArrays(1, &Vao);
        if(Vbo) glDeleteBuffers(1, &Vbo);
        if(Ibo) glDeleteBuffers(1, &Ibo);
    }

    void Render() const
    {
        glBindVertexArray(Vao);
        glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
    }

    void CreateGLObjs()
    {
        if(Vao) glDeleteVertexArrays(1, &Vao);
        if(Vbo) glDeleteBuffers(1, &Vbo);
        if(Ibo) glDeleteBuffers(1, &Ibo);
    
        Vao = CreateVertexArray();
        Vbo = CreateVertexBuffer(Vertices.data(), sizeof(float) * Vertices.size());
        Ibo = CreateIndexBuffer(Indices.data(), sizeof(unsigned int) * Indices.size());
        AddVertexAttrib(Vao, 0, VertexAttribType::FLOAT, 3, sizeof(float) * 3, 0);
    }
};

using ChunkKey = std::pair<int32_t, int32_t>;
struct HashPair
{
    size_t operator()(const std::pair<int32_t, int32_t>& p) const
    {
        auto hash1 = std::hash<int32_t>{}(p.first);
        auto hash2 = std::hash<int32_t>{}(p.second);
        return hash1 ^ hash2;
    }
};

inline void AddFace(std::vector<float>& vertices, std::vector<unsigned int>& indices, unsigned int& indexOffset, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
    vertices.insert(vertices.end(), { v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z });

    indices.insert(indices.end(), { indexOffset, indexOffset + 1, indexOffset + 2, indexOffset, indexOffset + 2, indexOffset + 3 });
    indexOffset += 4;
}


inline void GenerateGreedyMesh(Chunk& chunk)
{
    chunk.Vertices.clear();
    chunk.Indices.clear();
    unsigned int indexOffset = 0;

    std::array<std::array<std::array<bool, ChunkSize>, ChunkHeight>, ChunkSize> visited = {};

    auto addFace = [&](int x, int y, int z, glm::vec3 normal, glm::vec3 offsets[4]) {
        glm::vec3 v0 = { x + offsets[0].x, y + offsets[0].y, z + offsets[0].z };
        glm::vec3 v1 = { x + offsets[1].x, y + offsets[1].y, z + offsets[1].z };
        glm::vec3 v2 = { x + offsets[2].x, y + offsets[2].y, z + offsets[2].z };
        glm::vec3 v3 = { x + offsets[3].x, y + offsets[3].y, z + offsets[3].z };

        AddFace(chunk.Vertices, chunk.Indices, indexOffset, v0, v1, v2, v3);
    };

    for(int x = 0; x < ChunkSize; ++x)
    {
        for(int y = 0; y < ChunkHeight; ++y)
        {
            for(int z = 0; z < ChunkSize; ++z)
            {
                if(chunk.Blocks[x][y][z] == BlockType::AIR) continue;

                // Check each face of the voxel
                if(x == 0 || chunk.Blocks[x-1][y][z] == BlockType::AIR)
                {
                    glm::vec3 normal(-1, 0, 0);
                    glm::vec3 offsets[4] = {
                        {0, 0, 0}, {0, 1, 0}, {0, 1, 1}, {0, 0, 1}
                    };
                    addFace(x, y, z, normal, offsets);
                }
                if(x == ChunkSize-1 || chunk.Blocks[x+1][y][z] == BlockType::AIR)
                {
                    glm::vec3 normal(1, 0, 0);
                    glm::vec3 offsets[4] = {
                        {1, 0, 0}, {1, 1, 0}, {1, 1, 1}, {1, 0, 1}
                    };
                    addFace(x, y, z, normal, offsets);
                }
                if(y == 0 || chunk.Blocks[x][y-1][z] == BlockType::AIR)
                {
                    glm::vec3 normal(0, -1, 0);
                    glm::vec3 offsets[4] = {
                        {0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}
                    };
                    addFace(x, y, z, normal, offsets);
                }
                if(y == ChunkHeight-1 || chunk.Blocks[x][y+1][z] == BlockType::AIR)
                {
                    glm::vec3 normal(0, 1, 0);
                    glm::vec3 offsets[4] = {
                        {0, 1, 0}, {1, 1, 0}, {1, 1, 1}, {0, 1, 1}
                    };
                    addFace(x, y, z, normal, offsets);
                }
                if(z == 0 || chunk.Blocks[x][y][z-1] == BlockType::AIR)
                {
                    glm::vec3 normal(0, 0, -1);
                    glm::vec3 offsets[4] = {
                        {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}
                    };
                    addFace(x, y, z, normal, offsets);
                }
                if(z == ChunkSize-1 || chunk.Blocks[x][y][z+1] == BlockType::AIR)
                {
                    glm::vec3 normal(0, 0, 1);
                    glm::vec3 offsets[4] = {
                        {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}
                    };
                    addFace(x, y, z, normal, offsets);
                }
            }
        }
    }
    chunk.CreateGLObjs();
}

class World
{
public:
    World()
        : m_NoiseGen(6, 2.0f, 0.5f, 0, ChunkSize, ChunkSize)
    {}

    const Chunk* GetChunk(int x, int y) const { return m_ChunkMap.at(ChunkKey{x, y}).get(); }
    void GenChunk(int x, int y) { AddChunk(x, y); }

    const NoiseGen& GetNoiseGen() const { return m_NoiseGen; }
    NoiseGen& GetNoiseGen() { return m_NoiseGen; }

    void UpdateActiveChunks(const glm::vec3& camPos)
    {
        int chunkX = (int)camPos.x / ChunkSize;
        int chunkY = (int)camPos.z / ChunkSize;

        std::unordered_map<ChunkKey, std::unique_ptr<Chunk>, HashPair> newActiveChunks;

        for(int x = chunkX - ActiveRadius; x <= chunkX + ActiveRadius; ++x)
        {
            for(int y = chunkY - ActiveRadius; y <= chunkY + ActiveRadius; ++y)
            {
                ChunkKey key{x, y};
                if(m_ChunkMap.find(key) == m_ChunkMap.end())
                {
                    AddChunk(x, y);
                }
                newActiveChunks[key] = std::move(m_ChunkMap[key]);
            }
        }

        // Remove chunks that are no longer in the active range
        for(auto it = m_ChunkMap.begin(); it != m_ChunkMap.end();)
        {
            if(newActiveChunks.find(it->first) == newActiveChunks.end())
            {
                it = m_ChunkMap.erase(it);
            }
            else
            {
                ++it;
            }
        }

        m_ChunkMap = std::move(newActiveChunks);
    }

    std::vector<const Chunk*> GetActiveChunks(const glm::vec3& cameraPosition)
    {
        std::vector<const Chunk*> activeChunks;
        int32_t chunkX = static_cast<int32_t>(cameraPosition.x) / ChunkSize;
        int32_t chunkY = static_cast<int32_t>(cameraPosition.z) / ChunkSize;

        for(int x = chunkX - ActiveRadius; x <= chunkX + ActiveRadius; ++x)
        {
            for(int y = chunkY - ActiveRadius; y <= chunkY + ActiveRadius; ++y)
            {
                ChunkKey key{x, y};
                if(m_ChunkMap.find(key) != m_ChunkMap.end())
                {
                    activeChunks.push_back(m_ChunkMap[key].get());
                }
            }
        }

        return activeChunks;
    }

private:
    void AddChunk(int32_t chunkX, int32_t chunkY)
    {
        auto chunk = std::make_unique<Chunk>(Chunk(chunkX, chunkY));

        std::vector<float> heightMap = m_NoiseGen.GenerateNoise2D(chunkX, chunkY, 512);

        for(int z = 0; z < ChunkSize; ++z)
        {
            for(int x = 0; x < ChunkSize; ++x)
            {
                float height = (heightMap[z * ChunkSize + x] + 1.0f) / 2.0f;
                int intHeight = (int)(height * (ChunkHeight - 1));

                for(int y = 0; y < ChunkHeight; ++y)
                {
                    if(y <= intHeight)
                    {
                        chunk->Blocks[x][y][z] = BlockType::DIRT;
                    }
                    else
                    {
                        chunk->Blocks[x][y][z] = BlockType::AIR;
                    }
                }
            }
        }
        
        GenerateGreedyMesh(*chunk);

        m_ChunkMap[ChunkKey{chunkX, chunkY}] = std::move(chunk);
    }

private:
    NoiseGen m_NoiseGen;
    std::unordered_map<ChunkKey, std::unique_ptr<Chunk>, HashPair> m_ChunkMap;
};
