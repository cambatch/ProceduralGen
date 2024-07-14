#include "window.hpp"

#include "inputListener.hpp"

#include <glad/gl.h>
#include <iostream>


Window::Window(int32_t width, int32_t height, const char* title)
    : m_Title(title)
{
    if(!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!\n";
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if(!m_Window)
    {
        std::cerr << "Failed to create window!\n";
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_Window);

    // Callbacks
    glfwSetFramebufferSizeCallback(m_Window, Window::FramebufferSizeCallback);
    glfwSetKeyCallback(m_Window, Window::KeyCallback);
    // glfwSetCursorPosCallback(m_Window, WindowManager::MousePosCallback);
    glfwSetMouseButtonCallback(m_Window, Window::MouseButtonCallback);
    glfwSetScrollCallback(m_Window, Window::MouseScrollCallback);
    glfwSetWindowPosCallback(m_Window, Window::WinPosCallback);
    glfwSetWindowSizeCallback(m_Window, Window::WinSizeCallback);
    glfwSetWindowUserPointer(m_Window, this);

    if(glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    // Vsync
    glfwSwapInterval(1);

    int version = gladLoadGL(glfwGetProcAddress);
    if(version == 0) {
        std::cerr << "Failed to initlaize OpenGL context!\n";
        exit(EXIT_FAILURE);
    }
    std::cout << "OpenGL version: " << GLAD_VERSION_MAJOR(version) << '.' << GLAD_VERSION_MINOR(version) << '\n';

    glEnable(GL_DEPTH_TEST);

    int winX, winY;
    glfwGetWindowPos(m_Window, &winX, &winY);

    m_WindowedModeParams = { 
        .width = width,
        .height = height,
        .xPos = winX,
        .yPos = winY
    };
}

Window::~Window()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Window::PollEvents()
{
    glfwPollEvents();
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(m_Window);
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(m_Window);
}

void Window::Close()
{
    glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
}

void Window::ToggleCursorVisibility()
{
    if(m_CursorHidden)
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    m_CursorHidden = !m_CursorHidden;
}

void Window::BorderlessFullscreen()
{
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    // m_Fullscreen needs to be set before glfwSetWindowMonitor since the call triggers the window size callback to be called
    // and that messes with the windowed mode parameters, so we set it first to be able to check if we should ignore the resize.
    m_Fullscreen = true;
    glfwSetWindowMonitor(m_Window, primaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);
}

void Window::Windowed() {
    glfwSetWindowMonitor(m_Window, nullptr,
                         m_WindowedModeParams.xPos, m_WindowedModeParams.yPos,
                         m_WindowedModeParams.width, m_WindowedModeParams.height, 0);
    m_Fullscreen = false;
}

void Window::addInputListener(InputListener* listener)
{
    m_InputListeners.push_back(listener);
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    auto win = (Window*)glfwGetWindowUserPointer(window);
    win->Width = width;
    win->Height = height;
    glViewport(0, 0, width, height);
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto win = (Window*)glfwGetWindowUserPointer(window);
    for(auto listener : win->m_InputListeners)
    {
        listener->OnKeyPress(key, action);
    }
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto win = (Window*)glfwGetWindowUserPointer(window);
    for(auto listener : win->m_InputListeners)
    {
        listener->OnMouseButton(button, action);
    }
}

// void WindowManager::MousePosCallback(GLFWwindow* window, double x, double y)
// {
//     auto win = (WindowManager*)glfwGetWindowUserPointer(window);
//     for(auto listener : win->m_InputListeners)
//     {
//         listener->OnMouseMove(x, y);
//     }
// }

void Window::MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    auto win = (Window*)glfwGetWindowUserPointer(window);
    for(auto listener : win->m_InputListeners)
    {
        listener->OnMouseScroll(xOffset, yOffset);
    }
}

void Window::WinPosCallback(GLFWwindow* window, int xpos, int ypos)
{
    auto win = (Window*)glfwGetWindowUserPointer(window);

    if(win->m_Fullscreen) return;

    win->m_WindowedModeParams.xPos = xpos;
    win->m_WindowedModeParams.yPos = ypos;
}

void Window::WinSizeCallback(GLFWwindow* window, int width, int height)
{
    auto win = (Window*)glfwGetWindowUserPointer(window);

    if(win->m_Fullscreen) return;

    win->m_WindowedModeParams.width = width;
    win->m_WindowedModeParams.height = height;
}
