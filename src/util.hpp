#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>

static std::string ReadFile(const char* path)
{
    std::ifstream file(path);
    if(!file.is_open())
    {
        std::cerr << "Failed to open " << path << '\n';
        return {};
    }

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}


class Timer
{
public:
    Timer()
    {
        Start();
    }

    void Start()
    {
        m_Start = std::chrono::high_resolution_clock::now();
    }

    float Elapsed()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
    }

    float ElapsedMilli()
    {
        return Elapsed() * 1000.0f;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
};
