#pragma once
#include "Console.h"

#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include <implot.h>
#include <imnodes.h>

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include "FreeImage.h"
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <future>

class Window {
public:
	Window(int w = 1600, int h = 900);

	int init();
	void setBackground();
	void initImGui();
	void ImGuiRenderInit();
	void ImGuiCleanUp();
	void ImGuiRender();
	virtual void render() = 0;

protected:
	GLFWwindow* window;
	unsigned int SCR_WIDTH;
	unsigned int SCR_HEIGHT;
};