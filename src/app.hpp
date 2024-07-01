#pragma once

#include <GLFW/glfw3.h>

#include "inputManager.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "noise.hpp"


class App
{
public:
	App(int32_t width, int32_t height, const char* title);
	~App();

	void Run();

private:
	void ProcessAppInput();

	void SetupImgui();

	void DrawNoiseTextureWindow();

private:
	Window m_Window;
	InputManager m_Input;
	Camera m_Camera;
	NoiseGen2D m_NoiseGen;
	Texture* m_NoiseTexture;
	Texture* m_ColorMapTexture;

	float m_DeltaTime;
	float m_PrevFrame;

	bool m_GuiActive = false;
};
