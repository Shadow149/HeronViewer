#pragma once
#include "catalog.h"
#include "Module.h"

class Heron;

class Gallery :
    public Module
{
public:
	Gallery(Heron* h_window, const std::string& n = "Gallery", const bool v = true)
		: Module(n, v), h_window_(h_window), catalog_(catalog::instance())

	{
	}
	void init() override;
	void render() override;
	void cleanup() override {}
private:
	Heron* h_window_;
	catalog* catalog_;
	std::map<size_t, GLuint> catalog_textures_;
	int selected_item_ = 0;
};

