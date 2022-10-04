#pragma once
#include "Window.h"
#include "Module.h"
#include "Image.h"
#include "MainPanel.h"
#include "Editor.h"
#include "Curve.h"
#include "Histogram.h"
#include "Console.h"
#include "Overlay.h"
#include "Preferences.h"

class FileDialog;

class Heron: public Window
{
public:
	Heron() : Window(1600, 900) {};
	Heron(int w, int h) : Window(w, h) {};
	std::string getFileDialogKey();
	std::vector<Module*> getModules();
	void render();
	void unloadImage();
	void loadImage(std::string filePath, std::string fileName);

	void saveImage();

	void setImagePath(std::string s);

private:
	glm::vec3 view_pos = glm::vec3(0);
	float view_scale = 1.0f;
	FileDialog* fileDialog = NULL;
	Image* image = NULL;
	Histogram* hist = NULL;
	Editor* editor = NULL;
	std::vector<Module*> modules;

	std::string imgFilePath;
	std::string imgFile;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	std::string* fpsMetric;
	std::string* frameTime;
	std::string* renderTime;
	std::string* imGuiRenderTime;

	bool clip = false;
	bool b4 = false;

	void processInput(GLFWwindow* window);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void static_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void calcTime();
	void initGlfw();
	void initModules();
};

