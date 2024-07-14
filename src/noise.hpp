#pragma once


#include <FastNoise/FastNoise.h>


class NoiseGen {
public:
    NoiseGen(int octaves, float lacunarity, float gain, int seed, int width, int height)
        : m_Simplex(FastNoise::New<FastNoise::Simplex>()), m_Fractal(FastNoise::New<FastNoise::FractalFBm>()),
          m_Octaves(octaves), m_Lacunarity(lacunarity), m_Gain(gain), m_Seed(seed),
          m_ChunkWidth(width), m_ChunkHeight(height)
    {
        m_Fractal->SetSource(m_Simplex);
        m_Fractal->SetOctaveCount(octaves);
        m_Fractal->SetLacunarity(lacunarity);
        m_Fractal->SetGain(gain);
    }

    std::vector<float> GenerateNoise2D(int chunkX, int chunkY, int featureSize) const
    {
        std::vector<float> noiseVals(m_ChunkWidth * m_ChunkHeight);
        float startX = (-m_ChunkWidth / 2.0f) + (chunkX * m_ChunkWidth);
        float StartY = (-m_ChunkHeight / 2.0f) + (chunkY * m_ChunkHeight);
        float frequency = 1.0f / featureSize;

        m_Fractal->GenUniformGrid2D(noiseVals.data(), startX, StartY, m_ChunkWidth, m_ChunkHeight, frequency, m_Seed);
        return noiseVals;
    }

    void SetOctaveCount(int n) { m_Octaves = n; m_Fractal->SetOctaveCount(n); }
    void SetLacunarity(float lac) { m_Lacunarity = lac; m_Fractal->SetLacunarity(lac); }
    void SetGain(float gain) { m_Gain = gain; m_Fractal->SetGain(gain); }
    void SetSeed(int seed) { m_Seed = seed; }
    void SetChunkDims(int w, int h) { m_ChunkWidth = w; m_ChunkHeight = h; }

    float Lacunarity() const { return m_Lacunarity; }
    float Gain() const { return m_Gain; }
    int   Seed() const { return m_Seed; }
    int   Octaves() const { return m_Octaves; }
    int   Width() const { return m_ChunkWidth; }
    int   Height() const { return m_ChunkHeight; }

private:
    FastNoise::SmartNode<FastNoise::Simplex> m_Simplex;
    FastNoise::SmartNode<FastNoise::FractalFBm> m_Fractal;
    int   m_Octaves;
    float m_Lacunarity;
    float m_Gain;
    int   m_Seed;
    int   m_ChunkWidth;
    int   m_ChunkHeight;
};
