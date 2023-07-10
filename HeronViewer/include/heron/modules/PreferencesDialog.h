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


#define PREFERENCES_FILE "preferences.ini"

class PreferencesDialog : public Module
{
public:
	static PreferencesDialog* instance();
	static void destroy()
	{
		if (pref_ == nullptr)
			return;
		pref_->pref_thread_.detach();
		delete pref_;
	}

	void init() override;
	void render() override;
	void cleanup() override {};

private:
	explicit PreferencesDialog(const std::string& n = "Preferences", const bool v = false) : Module(n, v, false) {}

	void save_settings();
	void write_settings();
	void write_to_buffers();
	void read_settings();

public:
	std::experimental::filesystem::path EXPORT_DIR;
private:
	static PreferencesDialog* pref_;

	bool reading_ini_ = false, writing_ini_ = false;
	std::thread pref_thread_;
	std::string export_dir_str_;

};
