#include "app.hpp"

#include <vector>

#include <glad/gl.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "graphics.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "world.hpp"


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
    Shader shader("assets/shaders/terrain.vert", "assets/shaders/terrain.frag");
    Shader waterShader("assets/shaders/water.vert", "assets/shaders/water.frag");
    Texture textureAtlas("assets/textures/TextureAtlas.png");
    Texture waterTexture("assets/textures/Water.jpg");

    const glm::vec3 lightPos = { 0.0f, 10.0f, 100.0f };

    textureAtlas.Bind();
    shader.Bind();
    shader.SetUniform("material.diffuse", 0);
    shader.SetUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shader.SetUniform("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    shader.SetUniform("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));

    waterTexture.Bind(1);
    waterShader.Bind();
    waterShader.SetUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    waterShader.SetUniform("lightPos", lightPos);
    waterShader.SetUniform("waterTexture", 1);

    SetupImgui();

    m_Camera.SetFarClip(ChunkSize * ActiveRadius * 2);

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
        shader.SetUniform("view", view);
        shader.SetUniform("projection", proj);
        shader.SetUniform("viewPos", m_Camera.Position);

        m_World.UpdateActiveChunks(m_Camera.Position);
        std::vector<const Chunk*> chunks = m_World.GetActiveChunks(m_Camera.Position);

        textureAtlas.Bind();
        for(auto chunk : chunks)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(chunk->X * ChunkSize, 0, chunk->Y * ChunkSize));
            shader.SetUniform("model", model);

            chunk->Render();
        }

        waterShader.Bind();
        waterShader.SetUniform("view", view);
        waterShader.SetUniform("projection", proj);
        waterShader.SetUniform("viewPos", m_Camera.Position);
        waterTexture.Bind(1);

        for(auto chunk : chunks)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(chunk->X * ChunkSize, 0, chunk->Y * ChunkSize));
            waterShader.SetUniform("model", model);
            chunk->RenderWater();
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
