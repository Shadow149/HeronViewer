#pragma once
#include "Module.h"
#include "Heron.h"
#include "Status.h"
#include "Panel.h"

#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"ImGuiFileDialog.h"

class MainPanel final : public Panel
{
public:
	explicit MainPanel(Window* h) : Panel("Main", false)
	{
		heron_window_ = h;
	}
	void init_panel() override;
	void render_panel() override;
	void cleanup_panel() override;

private:
	Window* heron_window_;
};
