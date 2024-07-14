#include "camera.hpp"

#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(InputManager* input)
{
    m_Input = input;
    m_LastMousePos = m_Input->GetMousePos();
}

glm::mat4 Camera::GetProjectionMatrix()
{
    return glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearClip, m_FarClip);
}

glm::mat4 Camera::GetViewMatrix()
{
	glm::vec3 forward = Orientation * glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 camUp = Orientation * glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 target = Position + forward;

	return glm::lookAt(Position, target, camUp);
}

void Camera::Update(float deltaTime)
{
    updateOrientation(deltaTime);
    updatePosition(deltaTime);
}

void Camera::updateOrientation(float deltaTime)
{
    glm::vec2 currMousePos = m_Input->GetMousePos();
    glm::vec2 delta = currMousePos - m_LastMousePos;
    m_LastMousePos = currMousePos;

    float deltaX = delta.x * m_Sensitivity;
    float deltaY = delta.y * m_Sensitivity;

    glm::vec3 right = Orientation * glm::vec3(1.0f, 0.0f, 0.0f);
    glm::quat xRot = glm::angleAxis(glm::radians(-deltaX), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat yRot = glm::angleAxis(glm::radians(-deltaY), right);

    Orientation = glm::normalize(yRot *xRot * Orientation);

    m_Forward = glm::normalize(Orientation * glm::vec3(0.0f, 0.0f, -1.0f));
    m_Right = glm::normalize(Orientation * glm::vec3(1.0f, 0.0f, 0.0f));
    m_Up = glm::normalize(Orientation * glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::updatePosition(float deltaTime)
{
    float velocity = m_MoveSpeed * deltaTime;

    if(m_Input->KeyPressed(Key::LEFT_SHIFT))
        velocity *= 6;

    if(m_Input->KeyPressed(Key::SPACE))
        Position += WorldUp * velocity;
    if(m_Input->KeyPressed(Key::C))
        Position -= WorldUp * velocity;

    if(m_Input->KeyPressed(Key::W))
        Position += m_Forward * velocity;
    if(m_Input->KeyPressed(Key::S))
        Position -= m_Forward * velocity;
    if(m_Input->KeyPressed(Key::D))
        Position += m_Right * velocity;
    if(m_Input->KeyPressed(Key::A))
        Position -= m_Right * velocity;
}
