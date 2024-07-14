#pragma once

#include <unordered_map>

#include "noise.hpp"


struct Chunk {
    int32_t X;
    int32_t Y;
    std::vector<float> HeightMap;

    Chunk() : X(0), Y(0), HeightMap(0) {}
    Chunk(int32_t x, int32_t y, int32_t size)
        : X(x), Y(y), HeightMap(size * size)
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
    // Chunks are 32x32
    World(int chunkSize)
        : m_ChunkSize(chunkSize), m_NoiseGen(6, 2.0f, 0.5f, 0, chunkSize, chunkSize)
    {}

    const Chunk& GetChunk(int x, int y) const { return m_ChunkMap.at(ChunkKey{x, y}); }
    void GenChunk(int x, int y) { AddChunk(x, y); }

    const NoiseGen& GetNoiseGen() const { return m_NoiseGen; }
    NoiseGen& GetNoiseGen() { return m_NoiseGen; }

    int ChunkSize() const { return m_ChunkSize; }

private:
    void AddChunk(int32_t chunkX, int32_t chunkY)
    {
        Chunk chunk(chunkX, chunkY, m_ChunkSize);

        chunk.HeightMap = m_NoiseGen.GenerateNoise2D(chunkX, chunkY, 128);
        m_ChunkMap[ChunkKey{chunkX, chunkY}] = chunk;
    }

private:
    int m_ChunkSize;
    NoiseGen m_NoiseGen;
    std::unordered_map<ChunkKey, Chunk, HashPair> m_ChunkMap;
};
