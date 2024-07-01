#pragma once

#include <array>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "inputListener.hpp"
#include "keycodes.hpp"


class InputManager final : public InputListener
{
public:
    InputManager(GLFWwindow* window);

    void Update() {
        m_PrevState = m_CurrentState;
    }

    bool KeyPressed(Key key) {
        return m_CurrentState[key];
    }
    bool KeyJustPressed(Key key) {
        return !m_PrevState[key] && m_CurrentState[key];
    }
    bool KeyHeld(Key key) {
        return m_PrevState[key] && m_CurrentState[key];
    }
    bool KeyReleased(Key key) {
        return m_PrevState[key] && !m_CurrentState[key];
    }

    glm::vec2 GetMousePos();
    float GetMouseX() { return GetMousePos().x; }
    float GetMouseY() { return GetMousePos().y; }

public:
    void OnKeyPress(int key, int action) override;
    void OnMouseMove(double x, double y) override;
    void OnMouseButton(int key, int action) override;
    void OnMouseScroll(double xOffset, double yOffset) override;

private:
    std::array<bool, Key::KEY_LAST + 1> m_CurrentState;
    std::array<bool, Key::KEY_LAST + 1> m_PrevState;

    std::array<bool, MouseKey::LAST + 1> m_CurrentMouseState;
    std::array<bool, MouseKey::LAST + 1> m_PrevMouseState;

    GLFWwindow* m_Window;
};
