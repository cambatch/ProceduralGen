#pragma once

#include <vector>

#include <GLFW/glfw3.h>

#include "inputListener.hpp"


struct WindowedModeParams
{
    int width;
    int height;
    int xPos;
    int yPos;
};

class Window
{
public:
    int32_t Width;
    int32_t Height;

public:
    Window(int32_t width, int32_t height, const char* title);
    ~Window();

    void PollEvents();
    void SwapBuffers();
    bool ShouldClose();
    void Close();

    void ToggleCursorVisibility();

    void BorderlessFullscreen();
    void Windowed();

    void addInputListener(InputListener* listener);

    GLFWwindow* GetGLFWWindow() const { return m_Window; }
    bool Fullscreen() const { return m_Fullscreen; }
private:
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    // static void MousePosCallback(GLFWwindow* window, double x, double y);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    static void WinPosCallback(GLFWwindow* window, int xpos, int ypos);
    static void WinSizeCallback(GLFWwindow* window, int width, int height);

private:
    GLFWwindow* m_Window;
    bool m_CursorHidden = false;
    bool m_Fullscreen = false;
    const char* m_Title;
    std::vector<InputListener*> m_InputListeners;
    WindowedModeParams m_WindowedModeParams;
};
