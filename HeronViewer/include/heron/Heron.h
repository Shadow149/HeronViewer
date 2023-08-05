#pragma once

#include "Window.h"
#include "Module.h"
#include "Image.h"
#include "MainPanel.h"
#include "Editor.h"
#include "Curve.h"
#include "Histogram.h"
#include "Console.h"
#include "Gallery.h"
#include "Overlay.h"
#include "Panel.h"
#include "PreferencesDialog.h"

class FileDialog;

const std::string HERON_VERSION = "0.1.0.1";

class Heron final : public Window
{
public:
	Heron() : Window(1600, 900){}

	Heron(const int w, const int h) : Window(w, h){}
	~Heron() override;

	std::string get_file_dialog_key();
	std::vector<Module*> get_modules();
	void on_window_load();
	void render() override;
	void load_image(const std::string& file_path, const std::string& file_name) const;
	void unload_image() const;

	void save_image() const;
	bool write_image_caches() const;
	void set_image_path(const std::string& s);
	void load_item(cat_item item) const;

private:
	void process_input(GLFWwindow* window);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void drop_callback(GLFWwindow* window, int path_count, const char* paths[]);
	static void static_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void static_drop_callback(GLFWwindow* window, int path_count, const char* paths[]);

	void calc_time();
	void init_glfw();
	void init_modules();

private:
	glm::vec3 view_pos_ = glm::vec3(0);
	float view_scale_ = 1.0f;
	FileDialog* file_dialog_ = nullptr;
	Image* image_ = nullptr;
	Histogram* hist_ = nullptr;
	Editor* editor_ = nullptr;

	std::vector<Panel*> panels_;
	Panel* editor_panel_ = nullptr;
	Panel* gallery_panel_ = nullptr;

	std::string img_file_path_;
	std::string img_file_;

	float delta_time_ = 0.0f;
	float last_frame_ = 0.0f;
	std::string* fps_metric_{};
	std::string* frame_time_{};
	std::string* render_time_{};
	std::string* im_gui_render_time_{};

	bool key_pressed_ = false;
	bool clip_ = false;
	bool b4_ = false;
	bool black_bckgrd_ = false;
};
