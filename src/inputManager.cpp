#include "inputManager.hpp"

#include "keycodes.hpp"


InputManager::InputManager(GLFWwindow* window)
{
    m_Window = window;
    m_CurrentState.fill(false);
    m_PrevState.fill(false);
    m_CurrentMouseState.fill(false);
    m_PrevMouseState.fill(false);
}

glm::vec2 InputManager::GetMousePos()
{
    double x, y;
    glfwGetCursorPos(m_Window, &x, &y);
    return { x, y};
}

void InputManager::OnKeyPress(int key, int action)
{
    if(action == KeyState::PRESSED)
        m_CurrentState[key] = true;
    else if(action == KeyState::RELEASED)
        m_CurrentState[key] = false;
}

// This only works for when the cursor is over the window...
void InputManager::OnMouseMove(double x, double y)
{
    // std::cout << std::format("x: {}, y: {}\n", x, y);
}

void InputManager::OnMouseButton(int key, int action)
{
    if(action == KeyState::PRESSED)
        m_CurrentMouseState[key] = true;
    else if(action == KeyState::RELEASED)
        m_CurrentMouseState[key] = false;
}

void InputManager::OnMouseScroll(double xOffset, double yOffset)
{}
