#pragma once
#include <string>
#include <utility>
#include <vector>

#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include "renderable.h"


class Panel : public renderable
{
public:
	virtual ~Panel() = default;
	explicit Panel(std::string name) : name(std::move(name)) {}

	void init() override {
		init_modules();
		init_panel();
	}

	void render() override {

		if (ImGui::Begin(name.c_str(), &visible_
			, ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiTabBarFlags_NoTooltip
		)) {

			ImGui::GetStyle().CellPadding = ImVec2(0, 0);

			render_panel();
			render_modules();

		}
		ImGui::End();

	}

	void cleanup() override {
		cleanup_modules();
		cleanup_panel();
	}

	void register_module(Module* _module)
	{
		modules_.push_back(_module);
	}

	std::vector<Module*> get_modules()
	{
		return modules_;
	}

private:

	virtual void init_panel() = 0;
	virtual void render_panel() = 0;
	virtual void cleanup_panel() = 0;

	void init_modules() const
	{
		for (Module* m : modules_)
		{
			m->init();
		}
	}

	void render_modules() const
	{
		for (Module* m : modules_)
		{
			//m->setStyle();
			if (!m->visible) continue;
			m->render();
		}
	}

	void cleanup_modules() const
	{
		for (Module* m : modules_)
		{
			m->cleanup();
		}
	}
public:
	std::string name;
protected:
	bool visible_ = true;
private:
	std::vector<Module*> modules_;
};
