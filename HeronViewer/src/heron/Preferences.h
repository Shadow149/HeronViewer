#pragma once

#include "Module.h"

#include "imgui_stdlib.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

#include "Console.h"
#include "Status.h"
#include <thread>

#include"implot.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#include "mini/ini.h"

#define PREFERENCES_FILE "preferences.ini"

class Preferences : public Module
{
private:
	static Preferences* pref;

	bool readingIni = false, writingIni = false;
	std::thread prefThread;
	std::string exportDirStr;

	Preferences(std::string n = "Preferences", bool v = false) : Module(n, v, false) { };

	void saveSettings();
	void writeSettings();
	void writeToBuffers();
	void readSettings();

public:
	std::experimental::filesystem::path EXPORT_DIR;

	static Preferences* instance();
	void init();
	void render();
	void cleanup();
};

