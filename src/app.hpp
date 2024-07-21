#pragma once

#include <GLFW/glfw3.h>

#include "inputManager.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "texture.hpp"
#include "world.hpp"


class App
{
public:
	App(int32_t width, int32_t height, const char* title);
	~App();

	void Run();

private:
	void ProcessAppInput();

	void SetupImgui();

private:
	Window m_Window;
	InputManager m_Input;
	Camera m_Camera;
	Texture* m_FullNoiseTexture;
	Texture* m_FullColorMapTexture;

	float m_DeltaTime;
	float m_PrevFrame;

	bool m_GuiActive = false;

	World m_World;
};
