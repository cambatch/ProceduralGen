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

std::vector<float> voxelVertices = {
    // Front face
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    // Back face
    0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f
};

std::vector<unsigned int> voxelIndices = {
    // Front face
    0, 1, 2, 2, 3, 0,
    // Back face
    4, 5, 6, 6, 7, 4,
    // Left face
    0, 4, 7, 7, 3, 0,
    // Right face
    1, 5, 6, 6, 2, 1,
    // Top face
    3, 2, 6, 6, 7, 3,
    // Bottom face
    0, 1, 5, 5, 4, 0 
};

App::App(int32_t width, int32_t height, const char* title)
    : m_Window(width, height, title), m_Input(m_Window.GetGLFWWindow()), m_Camera(&m_Input)
{
    m_Window.addInputListener(&m_Input);
    m_World.GenChunk(0, 0);
    // m_World.GenChunk(0, 1);
    g_FullNoiseMap = m_World.GetNoiseGen().GenerateNoise2D(0, 0, 256);
    // int size = m_World.GetChunkSize();

    // const auto& heightMap = m_World.GetChunk(0, 0).HeightMap;
    // m_FullNoiseTexture = CreateTextureFromFloats(size, size, g_FullNoiseMap);
    // g_FullColorMap = CreateColorMap(g_FullNoiseMap);
    // m_FullColorMapTexture = CreateTextureFromBytes(size, size, g_FullColorMap);
}

App::~App()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void App::Run()
{
    // Shader shader("assets/shaders/terrain.vert", "assets/shaders/terrain.frag");
    Shader shader("assets/shaders/cube.vert", "assets/shaders/cube.frag");

    unsigned int vao = CreateVertexArray();
    unsigned int vbo = CreateVertexBuffer(voxelVertices.data(), sizeof(float) * voxelVertices.size());
    unsigned int ibo = CreateIndexBuffer(voxelIndices.data(), sizeof(unsigned int) * voxelIndices.size());
    AddVertexAttrib(vao, 0, VertexAttribType::FLOAT, 3, sizeof(float) * 3, 0);

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
        glm::mat4 view = m_Camera.GetViewMatrix();
        glm::mat4 proj = m_Camera.GetProjectionMatrix();

        shader.Bind();
        shader.SetUniform("model", model);
        shader.SetUniform("view", view);
        shader.SetUniform("projection", proj);

        BindVertexArray(vao);
        auto& chunk = m_World.GetChunk(0, 0);

        const std::vector<const Chunk*> chunks = { &m_World.GetChunk(0, 0)};

        for(auto chunk : chunks)
        {
            for(int x = 0; x < 32; ++x)
            {
                for(int y = 0; y < 256; ++y)
                {
                    for(int z = 0; z < 32; ++z)
                    {
                        if((uint8_t)chunk->Blocks[x][y][z] == 1)
                        {
                            glm::mat4 model  = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
                            shader.SetUniform("model", model);
                            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
                        }
                    }
                }
            }
        }

        // DrawImGui Stuff
        // ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplGlfw_NewFrame();
        // ImGui::NewFrame();

        // if(m_GuiActive)
        // {
        //     DrawNoiseTextureWindow();
        // }

        // ImGui::Render();
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

// void App::DrawNoiseTextureWindow()
// {
//     static float scale = 1.0f;
//     static int octaves = m_World.GetNoiseGen().Octaves();
//     static float lacunarity = m_World.GetNoiseGen().Lacunarity();
//     static float gain = m_World.GetNoiseGen().Gain();
//     static int seed = m_World.GetNoiseGen().Seed();
//     // static int width = m_World.GetChunkSize();
//     // static int height = m_World.GetChunkSize();
//     static bool changed = false;
//     static bool showColorMap = false;

//     ImTextureID texID;
//     if(showColorMap) texID = (void*)(intptr_t)m_FullColorMapTexture->id;
//     else texID = (void*)(intptr_t)m_FullNoiseTexture->id;

//     ImGui::Begin("Texture window");
//     ImGui::Image(texID, ImVec2(512, 512));

//     // if(ImGui::InputInt("Width", &width)) changed = true;
//     // if(ImGui::InputInt("Height", &height)) changed = true;
//     if(ImGui::InputFloat("Noise Scale", &scale)) changed = true;
//     if(ImGui::InputInt("Octaves", &octaves)) changed = true;
//     if(ImGui::InputFloat("Lacunarity", &lacunarity)) changed = true;
//     if(ImGui::InputFloat("Gain (Persistence)", &gain)) changed = true;
//     if(ImGui::InputInt("Seed", &seed)) changed = true;

//     if(ImGui::Button("Toggle texture"))
//     {
//         showColorMap = !showColorMap;
//     }

//     if(ImGui::Button("Generate new noise map"))
//     {
//         if(changed)
//         {
//             std::cout << "Generating new noise map\n";
//             auto& noiseGen = m_World.GetNoiseGen();
//             noiseGen.SetOctaveCount(octaves);
//             noiseGen.SetLacunarity(lacunarity);
//             noiseGen.SetGain(gain);
//             noiseGen.SetSeed(seed);
//             // noiseGen.SetChunkDims(width, height);

//             Timer timer;

//             g_FullNoiseMap = m_World.GetNoiseGen().GenerateNoise2D(0, 0, 256);
//             g_FullColorMap = CreateColorMap(g_FullNoiseMap);

//             std::cout << "Creation took " <<  timer.ElapsedMilli() << " Milliseconds\n";

//             // Create new textures
//             delete m_FullNoiseTexture;
//             m_FullNoiseTexture = CreateTextureFromFloats(32, 32, g_FullNoiseMap);
//             delete m_FullColorMapTexture;
//             m_FullColorMapTexture = CreateTextureFromBytes(32, 32, g_FullColorMap);

//             changed = false;
//         }
//     }

//     ImGui::End();
// }

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
