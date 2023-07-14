#include "Image.h"

#include "catalog.h"
#include "cat_item.h"
#include "Histogram.h"
#include "serialise.h"
#include "Widgets.h"



void Image::unload()
{
	h_image_.unload();

	texture_.gen(0, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	texture_low_res_.gen(0, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	comp_texture_.gen(0, 0, GL_RGBA32F, GL_RGBA, GL_FLOAT, nullptr);
	comp_texture_small_.gen(0, 0, GL_RGBA32F, GL_RGBA, GL_FLOAT, nullptr);

	histogram_loaded = false;
}

void Image::get_image()
{
	h_image_.load_image(*catalog::instance()->get_current_item());
}

void Image::bind_image()
{
	const GLsizei width = h_image_.get_width();
	const GLsizei height = h_image_.get_height();
	const GLsizei lr_width = h_image_.get_lr_width();
	const GLsizei lr_height = h_image_.get_lr_height();
	const unsigned char* img_data = h_image_.get_img_data();
	const unsigned char* lr_img_data = h_image_.get_lr_img_data();

	resize_image<float>(width, height, RENDER_WIDTH,
		preview_size_.x, preview_size_.y);


	if (img_data)
	{
		texture_.init(width, height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, img_data);
	}
	else
	{
		Console::log("Failed to load texture");
	}
	if (lr_img_data)
	{
		texture_low_res_.init(lr_width, lr_height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, lr_img_data);
	}

	framebuffer_.init_texture(static_cast<GLsizei>(preview_size_.x), static_cast<GLsizei>(preview_size_.y), GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	comp_texture_.init(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, nullptr);
	comp_texture_small_.init(lr_width, lr_height, GL_RGBA32F, GL_RGBA, GL_FLOAT, nullptr);

	catalog::instance()->get_current_item()->hprev_width = lr_width;
	catalog::instance()->get_current_item()->hprev_height = lr_height;

}

void Image::set_changes(SliderValues* slider_values)
{
	vals_ = slider_values; 
	changed_ = true; // TODO implement this properly
}

void Image::set_changed()
{
	changed_ = true;
}

unsigned Image::get_height() const
{
	if (vals_->show_low_res)
		return h_image_.get_lr_height();
	return h_image_.get_height();
}

unsigned Image::get_width() const
{
	if (vals_->show_low_res)
		return h_image_.get_lr_width();
	return h_image_.get_width();
}

bool Image::get_changed() const
{
	return changed_;
}

void Image::recompile_shader()
{
	shader_ = Shader(R"(C:\Users\Alfred Roberts\Documents\projects\HeronViewer\HeronViewer\src\texture.vs)",
	                 R"(C:\Users\Alfred Roberts\Documents\projects\HeronViewer\HeronViewer\src\texture.fs)");
	process_compute_shader_ = ComputeShader(
		R"(C:\Users\Alfred Roberts\Documents\projects\HeronViewer\HeronViewer\src\texture.comp)");
	Console::log("Shader Recompiled!");
}

Image::~Image()
{
	h_image_.unload();
}

void Image::init()
{
	model_ = glm::translate(model_, glm::vec3(0, 0, 0));
	view_ = glm::translate(view_, glm::vec3(0.0f, -0.1f, -3.0f));
	projection_ = glm::ortho(-1.0f + zoom_, 1.0f - zoom_, -1.0f + zoom_, 1.0f - zoom_, 0.1f, 1000.0f);

	quad_.gen();

	texture_.gen(0, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	texture_low_res_.gen(0, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	framebuffer_.gen(0, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	comp_texture_.gen(0, 0, GL_RGBA32F, GL_RGBA, GL_FLOAT, nullptr);
	comp_texture_small_.gen(0, 0, GL_RGBA32F, GL_RGBA, GL_FLOAT, nullptr);

	vectorscope_.gen(255, 255, GL_RGBA32F, GL_RGBA, GL_FLOAT, nullptr);
	waveform_.gen(512, 255, GL_RGBA32F, GL_RGBA, GL_FLOAT, nullptr);

	ssbo_.gen(256 * 4 * sizeof(unsigned));
	ssbo_orig_.gen(256 * sizeof(unsigned));
	waveform_acc_.gen(512 * 255 * sizeof(unsigned));
	vectorscope_acc_.gen(255 * 255 * sizeof(unsigned));


	shaderLoadTime = Overlay::register_metric();
	imageRender = Overlay::register_metric();
}

void Image::render_to_frame_buffer() const
{
	framebuffer_.bind();
	glViewport(0, 0, preview_size_.x, preview_size_.y);
}

void Image::export_image(const std::string::const_pointer c_str, const export_data export_data)
{
	h_image_.export_image(c_str, comp_texture_, export_data);
}

void Image::clear_background(const bool* black_bckgrd)
{
	if (!*black_bckgrd)
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	else
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Image::set_viewpoint()
{
	const GLsizei width = h_image_.get_width();
	const GLsizei height = h_image_.get_height();
	view_ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	model_ = glm::mat4(1.0f);
	if (size.x > size.y)
	{
		scale(glm::vec3(size.y / height));
		translate(glm::vec3(-(size.x / width) * (static_cast<float>(width) / height), -(size.y / height),
			0.0f));
	}
	else
	{
		scale(glm::vec3(size.x / width));
		translate(glm::vec3(-(size.x / width), -(size.y / height) * (static_cast<float>(height) / width),
		                    0.0f));
	}
}

void Image::glrender(const bool* clip, const bool* b4, const bool* black_bckgrd)
{
	if (!visible || h_image_.is_loading()) return;

	if (h_image_.finished_loading())
	{
		Console::log("Image loaded to main thread");
		bind_image();
		if (image_loader.joinable())
			image_loader.join();
		(*hist_)->init();
		set_viewpoint();
		std::fill_n(cdf, 256, 1.0f / 8.0f);
		changed_ = true;
		need_prev_write_ = true;
	}


	if (h_image_.finished_exporting())
	{
		h_image_.finish_export();
	}

	const bool low_b4 = vals_->show_low_res;

	if (get_changed())	need_prev_write_ = true;

	vals_->show_low_res |= scrolling_;
	if ((vals_->show_low_res) && !need_texture_change_)
	{
		need_texture_change_ = true;
	}
	else if (!vals_->show_low_res && need_texture_change_)
	{
		scope_rerender_ = true;
		need_texture_change_ = false;

		if (need_prev_write_) {
			Console::log("Getting low res");
			auto* export_data = static_cast<GLfloat*>(malloc(h_image_.get_lr_width() * h_image_.get_lr_height() * 3 * sizeof(GLfloat)));
			memset(export_data, 0, h_image_.get_lr_width() * h_image_.get_lr_height() * 3);
			const double start = glfwGetTime();
			gl_pbo pbo{};
			pbo.gen(h_image_.get_lr_width() * h_image_.get_lr_height() * 3 * sizeof(GLfloat));
			comp_texture_small_.get_data_via_pbo(&pbo, export_data);
			Console::log("Export time: %f", glfwGetTime() - start);

			// TODO s_write unnessassarily copies data into another buffer...
			if (s_prev_write(export_data, catalog::instance()->get_current_item()->hprev_location, h_image_.get_lr_width() * h_image_.get_lr_height() * 3 * sizeof(GLfloat)) < 0)
			{
				Console::log("Unable to update preview...");
			}
			free(export_data);
			need_prev_write_ = false;
		}
	}

	if ((!get_changed() && !scrolling_ && !scope_rerender_) || !h_image_.is_loaded())
	{
		scrolling_ = false;
		vals_->show_low_res = low_b4;
		return;
	}

	const double start = glfwGetTime();
	render_to_frame_buffer();
	*shaderLoadTime = "Framebuffer render time: " + std::to_string(glfwGetTime() - start);

	comp_texture_.bind();
	texture_.bind();
	comp_texture_small_.bind();
	texture_low_res_.bind();

	process_compute_shader_.use();
	process_compute_shader_.setBool("low_res", vals_->show_low_res);
	process_compute_shader_.setFloatArray("low", vals_->low, 4);
	process_compute_shader_.setFloatArray("whites", vals_->whites, 4);
	process_compute_shader_.setFloatArray("mid", vals_->mid, 4);
	process_compute_shader_.setFloatArray("high", vals_->high, 4);
	process_compute_shader_.setFloatArray("contrast", vals_->contrast, 4);
	process_compute_shader_.setFloatArray("expo", vals_->expo, 4);

	process_compute_shader_.setFloat("sat_ref", vals_->sat_ref);

	process_compute_shader_.setFloatArray("lift", vals_->lift, 4);
	process_compute_shader_.setFloatArray("gamma", vals_->gamma, 4);
	process_compute_shader_.setFloatArray("gain", vals_->gain, 4);

	process_compute_shader_.setBool("inv", vals_->inv);
	process_compute_shader_.setBool("bw", vals_->bw);
	process_compute_shader_.setFloat("sat", vals_->sat);
	process_compute_shader_.setFloat("wb", vals_->wb);
	process_compute_shader_.setFloat("tint", vals_->tint);
	process_compute_shader_.setBool("clip", *clip);
	process_compute_shader_.setBool("b4", *b4);

	process_compute_shader_.setFloat("yiq_y", vals_->yiq_y);
	process_compute_shader_.setFloat("yiq_z", vals_->yiq_z);
	process_compute_shader_.setFloat("xyz_y", vals_->xyz_y);
	process_compute_shader_.setFloat("xyz_z", vals_->xyz_z);
	process_compute_shader_.setBool("noise_selected", vals_->noise_selected);
	process_compute_shader_.setFloat("noise", vals_->noise);

	process_compute_shader_.setFloatArray("hues", vals_->hues, 8);
	process_compute_shader_.setFloatArray("sats", vals_->sats, 8);
	process_compute_shader_.setFloatArray("lums", vals_->lums, 8);

	process_compute_shader_.setFloatArray("sharp_kernel", vals_->sharp_kernel, 9);

	process_compute_shader_.setFloatArray("cdf", cdf, 256);
	process_compute_shader_.setBool("histogram_loaded", histogram_loaded);

	const glm::ivec2 dispatch_size = h_image_.get_dispatch_size();
	glDispatchCompute(dispatch_size.x, dispatch_size.y, 1);


	if (get_changed() || (!get_changed() && scope_rerender_))
	{
		glMemoryBarrier(GL_ALL_BARRIER_BITS); // TODO optimise what barriers are needed

		ssbo_.clear(GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT);
		std::fill_n(histogram, 256 * 4, 0);

		ssbo_orig_.clear(GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT);
		std::fill_n(hist_orig, 256, 0);

		waveform_acc_.clear(GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT);
		vectorscope_acc_.clear(GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT);


		vectorscope_.bind();
		waveform_.bind();

		hist_compute_shader_.use();
		hist_compute_shader_.setBool("low_res", vals_->show_low_res);
		hist_compute_shader_.setBool("histogram_loaded", histogram_loaded);
		hist_compute_shader_.setFloat("var_mult", vals_->scope_brightness);

		glDispatchCompute(dispatch_size.x, dispatch_size.y, 1);

		glMemoryBarrier(GL_ALL_BARRIER_BITS); // TODO optimise what barriers are needed

		ssbo_.get_data(histogram);
		glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
		if (!histogram_loaded)
		{
			ssbo_orig_.get_data(hist_orig);
			glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
			float cum = 0;
			for (int i = 0; i < 256; i++)
			{
				cum += static_cast<float>(hist_orig[i]) / static_cast<float>(h_image_.get_size());
				cdf[i] = cum;
			}
		}
		histogram_loaded = true;
		changed_ = false;
		if (!get_changed() && scope_rerender_)
			scope_rerender_ = false;
	}

	glMemoryBarrier(GL_ALL_BARRIER_BITS); // TODO optimise what barriers are needed

	clear_background(black_bckgrd);

	shader_.use();
	shader_.setBool("low_res", vals_->show_low_res);
	shader_.setMat4("model", model_);
	shader_.setMat4("view", view_);
	shader_.setMat4("projection", projection_);

	shader_.setFloat("texelWidth", 1.0f / h_image_.get_width());
	shader_.setFloat("texelHeight", 1.0f / h_image_.get_height());

	quad_.draw();
	glBindTexture(GL_TEXTURE_2D, 0); //unbind

	scrolling_ = false;
	vals_->show_low_res = low_b4;
}


void Image::render()
{
	glMemoryBarrier(GL_ALL_BARRIER_BITS); // TODO optimise what barriers are needed

	const double start = glfwGetTime();
	ImGui::Begin(name.c_str(), &visible, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	size = ImGui::GetWindowSize();
	update_mouse_in_window();
	if (h_image_.is_loading())
	{
		ImGui::SetCursorPos(size / 2 - ImVec2(50, 50));
		spinner("LOADING...", 100, 10, ImGui::GetColorU32(ImVec4(255, 255, 255, 255)));
	}
	else ImGui::Image((ImTextureID)framebuffer_.get_texture_id(), preview_size_);
	ImGui::End();
	*imageRender = "ImGui Window Render Time: " + std::to_string(glfwGetTime() - start);
}

void Image::cleanup()
{
	quad_.cleanup();
}

float Image::calc_curve(float t, const int channel) const
{
	t *= pow(2.0f, vals_->expo[channel]) * (1.0f + vals_->whites[channel]);
	const float g = (4.0f * pow((1.0f - t), 3.0f) * t * (0.25f + vals_->low[channel]))
		+ (6.0f * pow((1 - t), 2.0f) * pow(t, 2.0f) * (0.5f + vals_->mid[channel]))
		+ (4.0f * (1.0f - t) * pow(t, 3.0f) * (0.75f + vals_->high[channel]))
		+ (pow(t, 4.0f) * 1.0f);
	const float contrast = vals_->contrast[channel] * ((sin(2*3.14*t)) / 4.0f);
	return pow(vals_->gain[channel] * ((1.0 - vals_->lift[channel]) * g + vals_->lift[channel]), vals_->gamma[channel]) + contrast;
}

void Image::scale(const glm::vec3 s)
{
	if (!mouse_in_window_) { return; }
	scale_factor_ = s.x;
	model_ = glm::scale(model_, s);
	scrolling_ = true;
}

void Image::translate(const glm::vec3 t)
{
	if (!mouse_in_window_) { return; }
	view_ = glm::translate(view_, t);
	scrolling_ = true;
}
