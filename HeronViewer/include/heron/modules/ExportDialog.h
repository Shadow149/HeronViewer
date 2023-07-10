#pragma once

#include "Module.h"

#include "imgui_stdlib.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

#include "Console.h"
#include "Status.h"
#include <thread>

#include"implot.h"
#include "mini/ini.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#include "Image.h"


class ExportDialog : public Module
{
public:
	static ExportDialog* instance();
	static void destroy()
	{
		if (export_ == nullptr)
			return;
		delete export_;
	}
	void init() override;
	void export_image(const std::string file_name, const export_data export_data);
	void render() override;
	void cleanup() override;

	void set_image(Image* p_img) { p_img_ = p_img; }
	void set_file_name(const std::string& file_name);
	bool export_button(const char* label) const;

private:
	explicit ExportDialog(const std::string& n = "Export", const bool v = false) : Module(n, v, false), p_img_(nullptr), jpg_quality_(70)
	{
	}

public:
	std::experimental::filesystem::path EXPORT_DIR;

private:
	static ExportDialog* export_;
	Image* p_img_;

	std::string file_name_png_;
	std::string file_name_jpg_;

	std::string export_name_;
	std::string export_dir_;

	int jpg_quality_;
};
