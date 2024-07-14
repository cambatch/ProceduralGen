#include "app.hpp"

#include <iostream>

#include <vector>

#include <glad/gl.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "graphics.hpp"

#include "shader.hpp"
#include "texture.hpp"
#include "util.hpp"


std::vector<uint8_t> CreateColorMap(const std::vector<float>& heightMap);
std::vector<uint8_t> GetColor(float val);


std::vector<uint8_t> g_FullColorMap((32 * 32) * 4);
std::vector<float> g_FullNoiseMap((256*256) * 4);

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
    : m_Window(width, height, title), m_Input(m_Window.GetGLFWWindow()), m_Camera(&m_Input), m_World(32)
{
    m_Window.addInputListener(&m_Input);
    m_World.GenChunk(0, 0);
    g_FullNoiseMap = m_World.GetNoiseGen().GenerateNoise2D(0, 0, 256);
    int size = m_World.ChunkSize();

    const auto& heightMap = m_World.GetChunk(0, 0).HeightMap;
    m_FullNoiseTexture = CreateTextureFromFloats(size, size, g_FullNoiseMap);
    g_FullColorMap = CreateColorMap(g_FullNoiseMap);
    m_FullColorMapTexture = CreateTextureFromBytes(size, size, g_FullColorMap);
}

App::~App()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


void App::Run()
{
    Shader shader("assets/shaders/terrain.vert", "assets/shaders/terrain.frag");

    size_t width = m_World.ChunkSize();
    size_t height = width;

    // We have position (3) and color (3) attributes.
    std::vector<float> vertices;
    vertices.reserve(6 * (width * height));
    std::vector<uint32_t> indices;
    indices.reserve(6 * (width - 1) * (height - 1));

    auto heightMap = m_World.GetChunk(0, 0).HeightMap;
    auto colorMap = g_FullColorMap;

    float heightScale = 50.0f;

    // Generate vertices
    for(size_t y = 0; y < height; ++y)
    {
        for(size_t x = 0; x < width; ++x)
        {
            vertices.push_back(x);
            vertices.push_back(heightMap[y * width + x] * heightScale);
            vertices.push_back(y);

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
    static float scale = 1.0f;
    static int octaves = m_World.GetNoiseGen().Octaves();
    static float lacunarity = m_World.GetNoiseGen().Lacunarity();
    static float gain = m_World.GetNoiseGen().Gain();
    static int seed = m_World.GetNoiseGen().Seed();
    static int width = m_World.ChunkSize();
    static int height = m_World.ChunkSize();
    static bool changed = false;
    static bool showColorMap = false;

    ImTextureID texID;
    if(showColorMap) texID = (void*)(intptr_t)m_FullColorMapTexture->id;
    else texID = (void*)(intptr_t)m_FullNoiseTexture->id;

    ImGui::Begin("Texture window");
    ImGui::Image(texID, ImVec2(512, 512));

    if(ImGui::InputInt("Width", &width)) changed = true;
    if(ImGui::InputInt("Height", &height)) changed = true;
    if(ImGui::InputFloat("Noise Scale", &scale)) changed = true;
    if(ImGui::InputInt("Octaves", &octaves)) changed = true;
    if(ImGui::InputFloat("Lacunarity", &lacunarity)) changed = true;
    if(ImGui::InputFloat("Gain (Persistence)", &gain)) changed = true;
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
            auto& noiseGen = m_World.GetNoiseGen();
            noiseGen.SetOctaveCount(octaves);
            noiseGen.SetLacunarity(lacunarity);
            noiseGen.SetGain(gain);
            noiseGen.SetSeed(seed);
            noiseGen.SetChunkDims(width, height);

            Timer timer;

            g_FullNoiseMap = m_World.GetNoiseGen().GenerateNoise2D(0, 0, 256);
            g_FullColorMap = CreateColorMap(g_FullNoiseMap);

            std::cout << "Creation took " <<  timer.ElapsedMilli() << " Milliseconds\n";

            // Create new textures
            delete m_FullNoiseTexture;
            m_FullNoiseTexture = CreateTextureFromFloats(32, 32, g_FullNoiseMap);
            delete m_FullColorMapTexture;
            m_FullColorMapTexture = CreateTextureFromBytes(32, 32, g_FullColorMap);

            changed = false;
        }
    }

    ImGui::End();
}

std::vector<uint8_t> CreateColorMap(const std::vector<float>& heightMap)
{
    std::vector<uint8_t> colorMap(heightMap.size() * 4);

    for(size_t i = 0; i < heightMap.size(); ++i)
    {
        std::vector<uint8_t> c = GetColor(heightMap[i]);

        colorMap[i * 4 + 0] = c[0];
        colorMap[i * 4 + 1] = c[1];
        colorMap[i * 4 + 2] = c[2];
        colorMap[i * 4 + 3] = c[3];
    }
    return colorMap;
}

std::vector<uint8_t> GetColor(float val)
{
    std::vector<uint8_t> color(4);

    if(val < -0.3f) color = { 0, 0 , 255, 255 };
    else if(val < 0.2f) color = { 0, 255, 0, 255 };
    else if(val < 0.6f) color = { 139, 69, 19, 255 };
    else color = { 255, 255, 255, 255 };
    return color;
}
