#pragma once
#include "catalog.h"
#include "Module.h"
#include "Panel.h"

class Heron;

class Gallery :
    public Panel
{
public:
	Gallery(Heron* h_window, const std::string& n = "Gallery")
		: Panel(n, true), h_window_(h_window), catalog_(catalog::instance())
	{
	}
	void init_panel() override;
	void on_focus() override;
	void render_panel() override;
	void cleanup_panel() override {}
private:
	Heron* h_window_;
	catalog* catalog_;
	std::map<size_t, GLuint> catalog_textures_;
	int selected_item_ = 0;
	int cells_per_width_ = 5;
};

