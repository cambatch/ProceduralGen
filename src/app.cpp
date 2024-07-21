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

        m_World.UpdateActiveChunks(m_Camera.Position);
        std::vector<const Chunk*> chunks = m_World.GetActiveChunks(m_Camera.Position);

        for(auto chunk : chunks)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(chunk->X * ChunkSize, 0, chunk->Y * ChunkSize));
            shader.SetUniform("model", model);

            chunk->Render();
        }

        // DrawImGui Stuff
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

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
