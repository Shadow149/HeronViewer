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

#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <future>

class Window
{
public:
	virtual ~Window() = default;
	explicit Window(int w = 1600, int h = 900);

	int init();
	static void set_background();
	void init_im_gui() const;
	static void im_gui_render_init();
	static void im_gui_clean_up();
	static void im_gui_render();
	virtual void render() = 0;

protected:
	GLFWwindow* window_{};
	int scr_width_;
	int scr_height_;
};
