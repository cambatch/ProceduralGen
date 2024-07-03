#pragma once

#include <vector>

#include <FastNoise/FastNoise.h>


class NoiseGen2D
{
public:
    NoiseGen2D()
        : m_Simplex(FastNoise::New<FastNoise::Simplex>()), m_Fractal(FastNoise::New<FastNoise::FractalFBm>()),
          m_Width(100), m_Height(100), m_Noise(m_Width * m_Height), m_Octaves(1), m_Lacunarity(1.0f),
          m_Gain(1.0f), m_Frequency(1.0f), m_Scale(1.0f)
    {
        m_Fractal->SetSource(m_Simplex);
        m_Fractal->SetOctaveCount(m_Octaves);
        m_Fractal->SetLacunarity(m_Lacunarity);
        m_Fractal->SetGain(m_Gain);
    }

    NoiseGen2D(size_t width, size_t height)
        : m_Simplex(FastNoise::New<FastNoise::Simplex>()), m_Fractal(FastNoise::New<FastNoise::FractalFBm>()),
          m_Width(width), m_Height(height), m_Noise(width * height), m_Octaves(1), m_Lacunarity(1.0f),
          m_Gain(1.0f), m_Frequency(1.0f), m_Scale(1.0f)
    {
        m_Fractal->SetSource(m_Simplex);
        m_Fractal->SetOctaveCount(m_Octaves);
        m_Fractal->SetLacunarity(m_Lacunarity);
        m_Fractal->SetGain(m_Gain);
    }

    NoiseGen2D(size_t width, size_t height, int octaves, float lacunarity, float gain, float frequency, float scale, int seed)
        : m_Simplex(FastNoise::New<FastNoise::Simplex>()), m_Fractal(FastNoise::New<FastNoise::FractalFBm>()),
          m_Width(width), m_Height(height), m_Noise(width * height), m_Octaves(octaves), m_Lacunarity(lacunarity),
          m_Gain(gain), m_Frequency(frequency), m_Scale(scale), m_Seed(seed)
    {
        m_Fractal->SetSource(m_Simplex);
        m_Fractal->SetOctaveCount(m_Octaves);
        m_Fractal->SetLacunarity(m_Lacunarity);
        m_Fractal->SetGain(m_Gain);
    }

    const std::vector<float>& GenerateNoise()
    {
        m_Noise.resize(m_Width * m_Height);

        float centerX = m_Width / 2.0f;
        float centerY = m_Height / 2.0f;

        m_Fractal->GenUniformGrid2D(m_Noise.data(), -centerX, -centerY, m_Width, m_Height, m_Frequency / m_Scale, m_Seed);
        return m_Noise;
    }

    const std::vector<float>& Noise() const { return m_Noise; }
    const std::vector<uint8_t>& ColorMap() const { return m_ColorMap; }
    const std::vector<float>& HeightMap() const { return m_HeightMap; }

    void SetOctaveCount(int n) { m_Fractal->SetOctaveCount(n); }
    void SetLacunarity(float lac) { m_Fractal->SetLacunarity(lac); }
    void SetFrequency(float freq) { m_Frequency = freq; }
    void SetGain(float gain) { m_Fractal->SetGain(gain); }
    void SetScale(float scale) { m_Scale = (scale > 0) ? scale : 0.0001f; }
    void SetDims(size_t width, size_t height) { m_Width = width; m_Height = height; }
    void SetSeed(int seed) { m_Seed = seed; }

    float Frequency() const { return m_Frequency; }
    float Scale() const { return m_Scale; }
    float Lacunarity() const { return m_Lacunarity; }
    float Gain() const { return m_Gain; }
    int Seed() const { return m_Seed; }
    int Octaves() const { return m_Octaves; }
    size_t Width() const { return m_Width; }
    size_t Height() const { return m_Height; }

    void CreateColorMap()
    {
        // 4 color components
        m_ColorMap.resize(m_Noise.size() * 4);

        for(size_t i = 0; i < m_Noise.size(); ++i)
        {
            float val = m_Noise[i];
            std::vector<uint8_t> color = GetColor(val);

            m_ColorMap[i * 4 + 0] = color[0];
            m_ColorMap[i * 4 + 1] = color[1];
            m_ColorMap[i * 4 + 2] = color[2];
            m_ColorMap[i * 4 + 3] = color[3];
        }
    }

    void CreateHeightMap(float heightScale)
    {
        m_HeightMap.resize(m_Noise.size());

        for(size_t i = 0; i < m_Noise.size(); ++i)
        {
            m_HeightMap[i] = m_Noise[i] * heightScale;
        }
    }

private:
    std::vector<uint8_t> GetColor(float val)
    {
        std::vector<uint8_t> color(4);

        if(val < -0.3f) color = { 0, 0, 255, 255 }; // Water
        else if(val < 0.2f) color = { 0, 255, 0, 255 }; // Grass
        else if(val < 0.6f) color = { 139, 69, 19, 255 }; // Mountain
        else color = { 255, 255, 255, 255 }; // Snow

        return color;
    }

private:
    FastNoise::SmartNode<FastNoise::Simplex> m_Simplex;
    FastNoise::SmartNode<FastNoise::FractalFBm> m_Fractal;
    size_t m_Width;
    size_t m_Height;
    std::vector<float> m_Noise;
    std::vector<float> m_HeightMap;
    std::vector<uint8_t> m_ColorMap;
    int m_Octaves;
    float m_Lacunarity;
    float m_Gain;
    float m_Frequency;
    float m_Scale;
    int m_Seed;
};
