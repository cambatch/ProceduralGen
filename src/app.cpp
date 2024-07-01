#include "app.hpp"

#include <algorithm>
#include <iostream>

#include <vector>

#include <glad/gl.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "graphics.hpp"

#include "shader.hpp"
#include "util.hpp"

void NormalizeNoiseValues(std::vector<float>& vals);

std::vector<unsigned int> cubeIndices = {
    0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
};

std::vector<float> cubeVertices = {
    -1.0, -1.0,  1.0,
     1.0, -1.0,  1.0,
    -1.0,  1.0,  1.0,
     1.0,  1.0,  1.0,
    -1.0, -1.0, -1.0,
     1.0, -1.0, -1.0,
    -1.0,  1.0, -1.0,
     1.0,  1.0, -1.0,
};

App::App(int32_t width, int32_t height, const char* title)
    : m_Window(width, height, title), m_Input(m_Window.GetGLFWWindow()), m_Camera(&m_Input)
{
    m_Window.addInputListener(&m_Input);

    m_NoiseGen = NoiseGen2D(500, 500, 6, 3.0f, 0.5f, 0.007f, 1.0f, 0);
    m_NoiseGen.GenerateNoise();
    m_NoiseGen.CreateColorMap();
    m_NoiseGen.CreateHeightMap(100);
    m_NoiseTexture = CreateTextureFromFloats(m_NoiseGen.Width(), m_NoiseGen.Height(), m_NoiseGen.Noise());
    m_ColorMapTexture = CreateTextureFromBytes(m_NoiseGen.Width(), m_NoiseGen.Height(), m_NoiseGen.ColorMap());
}

App::~App()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


void App::Run()
{
    // unsigned int vao = CreateVertexArray();
    // unsigned int vbo = CreateVertexBuffer(cubeVertices.data(), sizeof(float) * cubeVertices.size());
    // unsigned int ibo = CreateIndexBuffer(cubeIndices.data(), sizeof(unsigned int) * cubeIndices.size());
    // AddVertexAttrib(vao, 0, VertexAttribType::FLOAT, 3, sizeof(float) * 3, 0);

    // Shader shader("assets/shaders/cube.vert", "assets/shaders/cube.frag");

    Shader shader("assets/shaders/terrain.vert", "assets/shaders/terrain.frag");

    size_t width = m_NoiseGen.Width();
    size_t height = m_NoiseGen.Height();

    // We have position (3) and color (3) attributes.
    std::vector<float> vertices;
    vertices.reserve(6 * (width * height));
    std::vector<uint32_t> indices;
    indices.reserve(6 * (width - 1) * (height - 1));

    // size_t vertIndex = 0;

    auto heightMap = m_NoiseGen.HeightMap();
    auto colorMap = m_NoiseGen.ColorMap();

    // Generate vertices
    for(size_t y = 0; y < height; ++y)
    {
        for(size_t x = 0; x < width; ++x)
        {
            // vertices[vertIndex++] = x;
            // vertices[vertIndex++] = heightMap[y * width + x];
            // vertices[vertIndex++] = y;
            vertices.push_back(x);
            vertices.push_back(heightMap[y * width + x]);
            vertices.push_back(y);

            // vertices[vertIndex++] = (float)colorMap[(y * width + x) * 4 + 0] / 255.0f;
            // vertices[vertIndex++] = (float)colorMap[(y * width + x) * 4 + 1] / 255.0f;
            // vertices[vertIndex++] = (float)colorMap[(y * width + x) * 4 + 2] / 255.0f;
            vertices.push_back(colorMap[(y * width + x) * 4 + 0] / 255.0f);
            vertices.push_back(colorMap[(y * width + x) * 4 + 1] / 255.0f);
            vertices.push_back(colorMap[(y * width + x) * 4 + 2] / 255.0f);
        }
    }

    // Generate indices
    for(size_t y = 0; y < height - 1; ++y)
    {
        for(size_t x = 0; x < width - 1; ++x)
        {
            unsigned int topLeft = y * width + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (y + 1) * width + x;
            unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    unsigned int vao = CreateVertexArray();
    unsigned int vbo = CreateVertexBuffer(vertices.data(), sizeof(float) * vertices.size());
    unsigned int ibo = CreateIndexBuffer(indices.data(), sizeof(uint32_t) * indices.size());
    AddVertexAttrib(vao, 0, VertexAttribType::FLOAT, 3, sizeof(float) * 6, 0);
    AddVertexAttrib(vao, 1, VertexAttribType::FLOAT, 3, sizeof(float) * 6, sizeof(float) * 3);

    SetupImgui();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while(!m_Window.ShouldClose())
    {
        float currFrame = glfwGetTime();
        m_DeltaTime = currFrame - m_PrevFrame;
        m_PrevFrame = currFrame;

        m_Input.Update();
        m_Window.PollEvents();
        ProcessAppInput();

        if(m_GuiActive)
        {
            m_Camera.LockOrientation();
        }
        else
        {
            m_Camera.Update(m_DeltaTime);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model(1.0);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        glm::mat4 view = m_Camera.GetViewMatrix();
        glm::mat4 proj = m_Camera.GetProjectionMatrix();

        shader.Bind();
        shader.SetUniform("model", model);
        shader.SetUniform("view", view);
        shader.SetUniform("projection", proj);

        BindVertexArray(vao);
        // glDrawElements(GL_TRIANGLE_STRIP, cubeIndices.size(), GL_UNSIGNED_INT, 0);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


        // DrawImGui Stuff
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(m_GuiActive)
        {
            DrawNoiseTextureWindow();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        m_Window.SwapBuffers();
    }
}

void App::ProcessAppInput()
{
    if(m_Input.KeyJustPressed(Key::ESCAPE))
    {
        m_GuiActive = !m_GuiActive;
    }

    if(m_Input.KeyJustPressed(Key::COMMA))
    {
        static bool isWireframe = false;
        if(isWireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        isWireframe = !isWireframe;
    }

    if(m_Input.KeyJustPressed(Key::INSERT))
    {
        m_Window.ToggleCursorVisibility();
        m_Camera.LockOrientation();
    }

    if(m_Input.KeyJustPressed(Key::END))
    {
        if(m_Window.Fullscreen())
        {
            m_Window.Windowed();
        }
        else
        {
            m_Window.BorderlessFullscreen();
        }
        m_Camera.LockOrientation();
    }
}

void App::SetupImgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_Window.GetGLFWWindow(), true);
    ImGui_ImplOpenGL3_Init();
}

void App::DrawNoiseTextureWindow()
{
    static float scale = m_NoiseGen.Scale();
    static int octaves = m_NoiseGen.Octaves();
    static float lacunarity = m_NoiseGen.Lacunarity();
    static float gain = m_NoiseGen.Gain();
    static float frequency = m_NoiseGen.Frequency();
    static int seed = m_NoiseGen.Seed();
    static int width = m_NoiseGen.Width();
    static int height = m_NoiseGen.Height();
    static bool changed = false;
    static bool showColorMap = false;

    ImTextureID texID;
    if(showColorMap) texID = (void*)(intptr_t)m_ColorMapTexture->id;
    else texID = (void*)(intptr_t)m_NoiseTexture->id;

    ImGui::Begin("Texture window");
    ImGui::Image(texID, ImVec2(m_NoiseGen.Width(), m_NoiseGen.Height()));

    if(ImGui::InputInt("Width", &width)) changed = true;
    if(ImGui::InputInt("Height", &height)) changed = true;
    if(ImGui::InputFloat("Noise Scale", &scale)) changed = true;
    if(ImGui::InputInt("Octaves", &octaves)) changed = true;
    if(ImGui::InputFloat("Lacunarity", &lacunarity)) changed = true;
    if(ImGui::InputFloat("Gain (Persistence)", &gain)) changed = true;
    if(ImGui::InputFloat("Frequency", &frequency)) changed = true;
    if(ImGui::InputInt("Seed", &seed)) changed = true;

    if(ImGui::Button("Toggle texture"))
    {
        showColorMap = !showColorMap;
    }

    if(ImGui::Button("Generate new noise map"))
    {
        if(changed)
        {
            std::cout << "Generating new noise map\n";
            m_NoiseGen.SetDims(width, height);
            m_NoiseGen.SetScale(scale);
            m_NoiseGen.SetOctaveCount(octaves);
            m_NoiseGen.SetLacunarity(lacunarity);
            m_NoiseGen.SetGain(gain);
            m_NoiseGen.SetFrequency(frequency);
            m_NoiseGen.SetSeed(seed);
            Timer timer;

            m_NoiseGen.GenerateNoise();
            m_NoiseGen.CreateColorMap();
            std::cout << "Creation took " <<  timer.ElapsedMilli() << " Milliseconds\n";

            // Create new textures
            delete m_NoiseTexture;
            m_NoiseTexture = CreateTextureFromFloats(m_NoiseGen.Width(), m_NoiseGen.Height(), m_NoiseGen.Noise());
            delete m_ColorMapTexture;
            m_ColorMapTexture = CreateTextureFromBytes(m_NoiseGen.Width(), m_NoiseGen.Height(), m_NoiseGen.ColorMap());

            changed = false;
        }
    }

    ImGui::End();
}

void NormalizeNoiseValues(std::vector<float>& noiseVals)
{
    float minVal = *std::min_element(noiseVals.begin(), noiseVals.end());
    float maxVal = *std::max_element(noiseVals.begin(), noiseVals.end());

    for(float& val : noiseVals)
    {
        val = (val - minVal) / (maxVal - minVal);
    }
}

