#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "inputManager.hpp"


class Camera
{
public:
    glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
    glm::quat Orientation = { 1.0f, 0.0f, 0.0f, 0.0f };

    Camera(InputManager* input);

    void Update(float deltaTime);
    void LockOrientation() { m_LastMousePos = m_Input->GetMousePos(); }
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();

private:
    void updateOrientation(float deltaTime);
    void updatePosition(float deltaTime);

private:
    glm::vec3 m_Up = { 0.0f, 1.0f, 0.0f };
    glm::vec3 m_Right = { 1.0f, 0.0f, 0.0f };
    glm::vec3 m_Forward = { 0.0f, 0.0f, -1.0f };

    // Frustum
    float m_NearClip = 0.1f;
    float m_FarClip = 256.0f;
    float m_Fov = 45.0f;
    float m_AspectRatio = 16.0f / 9.0f;

    // movement
    float m_MoveSpeed = 2.5f;
    float m_Sensitivity = 0.1f;

    bool m_LockOrientation = false;

    glm::vec2 m_LastMousePos;

    InputManager* m_Input;

    static constexpr glm::vec3 WorldUp = { 0.0f, 1.0f, 0.0f };
};
