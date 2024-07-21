#pragma once

#include <unordered_map>
#include <array>
#include <iostream>

#include "noise.hpp"


// Chunks are 32x256x32
static const int ChunkSize = 32;
static const int ChunkHeight = 256;


enum class BlockType : uint8_t {
    AIR = 0,
    DIRT = 1
};


struct Chunk {
    int32_t X;
    int32_t Y;
    std::array<std::array<std::array<BlockType, ChunkSize>, ChunkHeight>, ChunkSize> Blocks;

    Chunk() : X(0), Y(0), Blocks() {}
    Chunk(int32_t x, int32_t y)
        : X(x), Y(y), Blocks()
    {}
};

using ChunkKey = std::pair<int32_t, int32_t>;
struct HashPair
{
    size_t operator()(const std::pair<int32_t, int32_t>& p) const {
        auto hash1 = std::hash<int32_t>{}(p.first);
        auto hash2 = std::hash<int32_t>{}(p.second);
        return hash1 ^ hash2;
    }
};


class World {
public:
    World()
        : m_NoiseGen(6, 2.0f, 0.5f, 0, ChunkSize, ChunkSize)
    {}

    const Chunk& GetChunk(int x, int y) const { return m_ChunkMap.at(ChunkKey{x, y}); }
    void GenChunk(int x, int y) { AddChunk(x, y); }

    const NoiseGen& GetNoiseGen() const { return m_NoiseGen; }
    NoiseGen& GetNoiseGen() { return m_NoiseGen; }

    

private:
    void AddChunk(int32_t chunkX, int32_t chunkY)
    {
        Chunk chunk(chunkX, chunkY);

        std::vector<float> heightMap = m_NoiseGen.GenerateNoise2D(chunkX, chunkY, 128);

        for(int z = 0; z < ChunkSize; ++z)
        {
            for(int x = 0; x < ChunkSize; ++x)
            {
                float height = (heightMap[z * ChunkSize + x] + 1.0f) / 2.0f;
                int intHeight = (int)(height * (ChunkHeight - 1));

                for(int y = 0; y < ChunkHeight; ++y)
                {
                    if(y <= intHeight) {
                        chunk.Blocks[x][y][z] = BlockType::DIRT;
                    } else {
                        chunk.Blocks[x][y][z] = BlockType::AIR;
                    }
                }
            }
        }
        
        m_ChunkMap[ChunkKey{chunkX, chunkY}] = chunk;

        std::cout << "Generated chunk " << chunkX << ' ' << chunkY << std::endl;
    }

private:
    NoiseGen m_NoiseGen;
    std::unordered_map<ChunkKey, Chunk, HashPair> m_ChunkMap;
};
