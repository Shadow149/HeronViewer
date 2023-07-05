#include "Heron.h"

#include <stb/stb_image.h>

#include "ExportDialog.h"
#include "FileDialog.h"
#include "Graph.h"
#include "Preview.h"
#include "Vectorscope.h"
#include "Waveform.h"

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
	glViewport(0, 0, width, height);
}


void Heron::process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE)
	{
		clip_ = false;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
	{
		b4_ = false;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !key_pressed_)
	{
		black_bckgrd_ = !black_bckgrd_;
		key_pressed_ = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && key_pressed_)
	{
		key_pressed_ = false;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		clip_ = true;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
	{
		b4_ = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		editor_->undo();
	}
}

void Heron::drop_callback(GLFWwindow* window, int path_count, const char* paths[])
{
	const std::string path = paths[0];
	const std::string fileName = path.substr(path.find_last_of("/\\") + 1);
	load_image(path, fileName);
}

void Heron::scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset)
{
	constexpr double scale = 0.05;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		const auto s = glm::vec3(1 + yoffset * scale, 1 + yoffset * scale, 0);
		view_scale_ += yoffset * scale;
		image_->scale(s);
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		const auto off = glm::vec3(yoffset * scale, 0, 0);
		view_pos_ += off;
		image_->translate(off);
	}
	else
	{
		const auto off = glm::vec3(xoffset * scale, yoffset * scale, 0);
		view_pos_ += off;
		image_->translate(off);
	}
}

void Heron::set_image_path(const std::string& s)
{
	img_file_path_ = s;
}

void Heron::load_image(const std::string& file_path, const std::string& file_name)
{
	unload_image();
	Console::log("Change Image : " + file_path);
	img_file_ = file_path;
	image_->get_image(img_file_);
	editor_->update_file(file_name, file_path);
	editor_->loaded(true);
}

void Heron::unload_image() const
{
	image_->unload();
	editor_->reset();
	editor_->loaded(false);
}

void Heron::static_scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset)
{
	const auto that = static_cast<Heron*>(glfwGetWindowUserPointer(window));
	that->scroll_callback(window, xoffset, yoffset);
}

void Heron::static_drop_callback(GLFWwindow* window, const int path_count, const char* paths[])
{
	const auto that = static_cast<Heron*>(glfwGetWindowUserPointer(window));
	that->drop_callback(window, path_count, paths);
}

void Heron::init_glfw()
{
	// GLFWimage images[1];
	// images[0].pixels = stbi_load("heron.jpg", &images[0].width, &images[0].height, 0, 4); //rgba channels 
	// glfwSetWindowIcon(window, 1, images);
	// stbi_image_free(images[0].pixels);

	glfwSetWindowUserPointer(window_, this);
	glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
	glfwSetScrollCallback(window_, static_scroll_callback);
	glfwSetDropCallback(window_, static_drop_callback);
}

void Heron::init_modules()
{
	file_dialog_ = new FileDialog(this, "File Dialog");
	image_ = new Image("Image", &hist_);
	hist_ = new Histogram(image_, "Histogram");
	editor_ = new Editor(image_, "Editor");
	ExportDialog::instance()->set_image(image_);

	modules_.push_back(new MainPanel(this, "Main"));
	modules_.push_back(file_dialog_);
	modules_.push_back(editor_);
	modules_.push_back(hist_);
	modules_.push_back(new Vectorscope(image_, "Vectorscope"));
	modules_.push_back(new Waveform(image_, "Waveform"));
	modules_.push_back(new Preview(image_, "Preview"));
	modules_.push_back(new Curve(image_, "Curve"));
	modules_.push_back(new Console("Console"));
	modules_.push_back(new Overlay("Overlay"));
	modules_.push_back(ExportDialog::instance());
	modules_.push_back(PreferencesDialog::instance());
	modules_.push_back(new Graph("Node Editor"));
	modules_.push_back(image_);

	fps_metric_ = Overlay::register_metric();
	frame_time_ = Overlay::register_metric();
	render_time_ = Overlay::register_metric();
	im_gui_render_time_ = Overlay::register_metric();
}

std::vector<Module*> Heron::get_modules()
{
	return modules_;
}

void Heron::on_window_load()
{
	Console::log("PRELOADING IMAGE...");
	load_image("./images/landscape.png",
	           "landscape.png");
}

std::string Heron::get_file_dialog_key()
{
	return file_dialog_->name;
}

void Heron::save_image() const
{
	editor_->update_config_file();
}

void Heron::calc_time()
{
	const double current_frame = glfwGetTime();
	delta_time_ = current_frame - last_frame_;
	last_frame_ = current_frame;
	*fps_metric_ = "Fps: " + std::to_string(1 / delta_time_);
#ifdef SHOW_FPS
	Overlay::updateFps(1.0f / deltaTime);
#endif
	*frame_time_ = "Frame Time: " + std::to_string(delta_time_);
}

void Heron::render()
{
	init_glfw();
	init_modules();
	init_im_gui();

	for (Module* m : modules_)
	{
		m->init();
	}

	on_window_load();

	while (!glfwWindowShouldClose(window_))
	{
		calc_time();
		process_input(window_);

		float start = glfwGetTime();
		image_->glrender(&clip_, &b4_, &black_bckgrd_);
		*render_time_ = "Gl Render Time: " + std::to_string(glfwGetTime() - start);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, scr_width_, scr_height_);

		start = glfwGetTime();

		im_gui_render_init();

		for (Module* m : modules_)
		{
			//m->setStyle();
			if (!m->visible) continue; // TODO remove check in individual render funcs
			m->render();
		}

		im_gui_render();

		*im_gui_render_time_ = "ImGui Render Time: " + std::to_string(glfwGetTime() - start);

		glfwSwapBuffers(window_);
		glfwPollEvents();
	}

	im_gui_clean_up();

	for (Module* m : modules_)
	{
		m->cleanup();
	}

	glfwTerminate();
}
