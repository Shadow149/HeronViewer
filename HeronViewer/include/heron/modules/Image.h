#pragma once
#include "Module.h"
#include "Console.h"
#include "Overlay.h"
#include "Status.h"
#include "shader.h"

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

#include "cat_item.h"
#include "gl_texture.h"
#include "HeronImage.h"
#include "shader_c.h"
#include "../common/SliderValues.h"

enum
{
	RENDER_WIDTH = 1920,
};

class Histogram;

class Image : public Module
{
public:
	Image(const std::string& n, Histogram** hist, const bool v = true) : Module(n, v)
	{
		this->hist_ = hist;
	}
	~Image() override;

	void init() override;
	void render_to_frame_buffer() const;
	void export_image(const std::string::const_pointer c_str, export_data export_data);
	static void clear_background(const bool* black_bckgrd);
	void set_viewpoint();
	void glrender(const bool* clip, const bool* b4, const bool* black_bckgrd);
	void render() override;
	void cleanup() override;
	void bind_image();

	void save_preview();
	void unload();
	void get_image();
	void set_changes(SliderValues* slider_values);
	void set_changed();
	unsigned get_height() const;
	unsigned get_width() const;
	bool get_changed() const;
	void recompile_shader();

	float calc_curve(float t, int channel) const;

	void scale(glm::vec3 s);
	void translate(glm::vec3 t);

	bool is_bw() const
	{
		return vals_->bw;
	}

	unsigned get_vectorscope_id() const
	{
		return vectorscope_.get_id();
	}

	unsigned get_waveform_id() const
	{
		return waveform_.get_id();
	}
	unsigned get_image_id() const
	{
		return comp_texture_.get_id();
	};


	bool is_loaded() const { return h_image_.is_loaded(); }
	bool is_exporting() const { return h_image_.is_exporting(); }
	bool is_unsaved() const { return unsaved_; }


public:

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

	HeronImage h_image_;

	bool changed_ = false;
	bool scope_rerender_ = true;
	bool need_texture_change_ = false;
	bool scrolling_ = false;
	bool unsaved_ = false;
	bool first_render_ = true;
	double scrolling_time_ = 0;


	Shader shader_ = Shader("./shaders/texture.vert", "./shaders/texture.frag");
	ComputeShader process_compute_shader_ = ComputeShader("./shaders/texture.comp");
	ComputeShader hist_compute_shader_ = ComputeShader("./shaders/histogram.comp");
	Histogram** hist_;

	std::string* shaderLoadTime{};
	std::string* imageRender{};
	std::string* uniformTime{};
	std::string* hist_ssbo_time{};
	std::string* scopeRender{};
	std::string* textureRender{};

	std::thread renderer_;

	gl_quad quad_;

	gl_framebuffer framebuffer_;

	gl_texture texture_{1};
	gl_texture texture_low_res_{3};

	gl_image comp_texture_{0};
	gl_image comp_texture_small_{2};
	gl_image vectorscope_{4};
	gl_image waveform_{5};

	gl_ssbo ssbo_{2};
	gl_ssbo ssbo_orig_{3};
	gl_ssbo waveform_acc_{6};
	gl_ssbo vectorscope_acc_{7};

};
