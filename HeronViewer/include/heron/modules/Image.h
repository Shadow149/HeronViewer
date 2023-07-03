#pragma once
#include "Module.h"
#include "Console.h"
#include "Overlay.h"
#include "Status.h"
#include "shader.h"
#include "FreeImage.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

#include"ImGuiFileDialog.h"

#include"stb_image_write.h"

#include <thread>

#include "shader_c.h"
#include "../common/SliderValues.h"

enum
{
	RENDER_WIDTH = 1920,
	SMALL_IMG_MAX = 4000
};

class Histogram;

class Image : public Module
{
public:
	Image(const std::string& n, Histogram** hist, const bool v = true) : Module(n, v)
	{
		this->hist_ = hist;
	}

	void init() override;
	void render_to_frame_buffer() const;
	void glrender(const bool* clip, const bool* b4, const bool* black_bckgrd);
	void render() override;
	void cleanup() override;
	void bind_image();

	void unload();
	void get_image(const char* filename);
	void set_changes(SliderValues* slider_values);
	unsigned char* get_data() const;
	unsigned get_height() const;
	unsigned get_width() const;
	bool get_changed() const;
	void recompile_shader();

	void exportImage(const char* file_loc);

	float calc_curve(float t, int channel) const;

	void scale(glm::vec3 s);
	void translate(glm::vec3 t);

	bool is_bw() const
	{
		return vals_->bw;
	}

	unsigned get_vectorscope_id() const
	{
		return vectorscope_;
	}

	unsigned get_waveform_id() const
	{
		return waveform_;
	}

private:
	void render_image(const char* file_loc);
	void update_preview_size();

public:

	bool image_loaded = false;
	bool rendering = false;
	std::thread image_loader;

	unsigned histogram[256 * 4]{};
	unsigned hist_orig[256]{};
	float cdf[256]{};
	bool histogram_loaded = false;

private:
	SliderValues* vals_{};

	glm::mat4 model_ = glm::mat4(1.0f);
	glm::mat4 model1_ = glm::mat4(1.0f);
	glm::mat4 view_ = glm::mat4(1.0f);
	glm::vec3 view_pos_ = glm::vec3(0.0f);
	glm::mat4 projection_ = glm::mat4(1.0f);

	ImVec2 size;
	ImVec2 preview_size_;

	float zoom_ = 0.0f;
	float scale_factor_ = 1.0f;
	int x_ = 0;
	int y_ = 0;

	GLuint vbo_{}, vao_{}, ebo_{};
	GLuint pbo_{};

	GLsizei height_ = 1, width_ = 1, bpp_ = 0;
	GLsizei height_small_ = 1, width_small_ = 1;

	unsigned char* data_{};
	unsigned char* data_low_res_{};
	GLubyte* export_data_{};
	FIBITMAP* bitmap_{};

	bool changed_ = false;
	bool scope_rerender_ = true;
	bool loading_ = false;
	bool need_texture_change_ = false;
	bool scrolling_ = false;


	bool thread_image_loaded_loaded_ = false;
	bool exporting_ = false;

	Shader shader_ = Shader("./shaders/texture.vert", "./shaders/texture.frag");
	ComputeShader process_compute_shader_ = ComputeShader("./shaders/texture.comp");
	ComputeShader hist_compute_shader_ = ComputeShader("./shaders/histogram.comp");
	Histogram** hist_;

	std::string* shaderLoadTime{};
	std::string* imageRender{};

	std::thread renderer_;

	GLuint imgui_preview_texture_{};
	GLuint framebuffer_{};
	GLuint texture_{};
	GLuint texture_low_res_{};

	GLuint comp_texture_{};
	GLuint comp_texture_small_{};
	GLuint vectorscope_{};
	GLuint waveform_{};
	GLuint waveform_acc_{};
	GLuint vectorscope_acc_{};
	GLuint ssbo_{};
	GLuint ssbo_orig_{};

};
