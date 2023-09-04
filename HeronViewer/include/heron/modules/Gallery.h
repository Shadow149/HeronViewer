#pragma once
#include "catalog.h"
#include "Module.h"
#include "Panel.h"
#include "Utils.h"

class Heron;

class Gallery :
    public Panel
{
public:
	Gallery(Heron* h_window, const std::string& n = "Gallery")
		: Panel(n, true), h_window_(h_window), catalog_(catalog::instance())
	{
		load_texture_from_file("images/heron_title.png", &cat_img_, &cat_img_width_, &cat_img_height_);
		int w, h;
		load_texture_from_file("images/shadow.png", &shadow_img_, &w, &h);
	}
	void init_panel() override;
	void on_focus() override;
	void render_panel() override;
	void cleanup_panel() override {}
private:
	Heron* h_window_;
	catalog* catalog_;
	std::map<size_t, GLuint> catalog_textures_;
	int selected_item_ = -1;
	int cells_per_width_ = 5;
	int cell_padding_ = 20;

	GLuint cat_img_;
	GLuint shadow_img_;
	int cat_img_width_;
	int cat_img_height_;
};

