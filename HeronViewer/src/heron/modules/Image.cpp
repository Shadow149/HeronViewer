#include "Image.h"
#include "Histogram.h"
#include "Widgets.h"

void Image::update_preview_size()
{
	if (width_ > height_)
		preview_size_ = ImVec2(RENDER_WIDTH * (static_cast<float>(width_) / height_), RENDER_WIDTH);
	else
		preview_size_ = ImVec2(RENDER_WIDTH, RENDER_WIDTH * (static_cast<float>(height_) / width_));
}

void Image::unload()
{
	if (!data_)
	{
		Console::log("No Image to unload");
		return;
	}
	Console::log("Unloading Image");
	glBindTexture(GL_TEXTURE_2D, texture_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, imgui_preview_texture_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, imgui_preview_texture_, 0);
	image_loaded = false;
	histogram_loaded = false;
}

void Image::get_image(const char* filename)
{
	histogram_loaded = false;
	loading_ = true;
	Console::log("Loading Image... " + std::string(filename));

	if (bitmap_)
		FreeImage_Unload(bitmap_);
	if (data_)
		free(data_);
	if (data_low_res_)
		free(data_low_res_);

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	fif = FreeImage_GetFileType(filename, 0);

	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(filename);
	if (fif == FIF_UNKNOWN)
	{
		Console::log("IMAGE LOAD ERROR - UNKNOWN FILE TYPE");
		thread_image_loaded_loaded_ = true;
		return;
	}

	if (FreeImage_FIFSupportsReading(fif))
	{
		if (fif == FIF_RAW)
			bitmap_ = FreeImage_Load(fif, filename, RAW_DISPLAY);
		else
			bitmap_ = FreeImage_Load(fif, filename);
	}

	if (!bitmap_)
	{
		Console::log("IMAGE LOAD ERROR - IMAGE FAILED TO LOAD");
		thread_image_loaded_loaded_ = true;
		return;
	}

	const unsigned bitmap_bpp = FreeImage_GetBPP(bitmap_);
	Console::log("Bitmap bpp: " + std::to_string(bitmap_bpp));

	FreeImage_FlipVertical(bitmap_);
	bitmap_ = FreeImage_ConvertTo24Bits(bitmap_);

	FreeImage_AdjustGamma(bitmap_, 2.5);

	width_ = static_cast<GLsizei>(FreeImage_GetWidth(bitmap_));
	height_ = static_cast<GLsizei>(FreeImage_GetHeight(bitmap_));
	bpp_ = static_cast<GLsizei>(FreeImage_GetBPP(bitmap_));

	Console::log("Bitmap (TEMP) bpp: " + std::to_string(bpp_));

	GLsizei small_img_height, small_img_width;
	if (width_ > height_ && width_ > SMALL_IMG_MAX)
	{
		small_img_width = SMALL_IMG_MAX;
		small_img_height = SMALL_IMG_MAX * (static_cast<double>(height_) / width_);
	}
	else if (height_ > SMALL_IMG_MAX)
	{
		small_img_width = SMALL_IMG_MAX * (static_cast<double>(width_) / height_);
		small_img_height = SMALL_IMG_MAX;
	}
	else
	{
		small_img_width = width_;
		small_img_height = height_;
	}
	width_ = small_img_width;
	height_ = small_img_height;

	width_small_ = width_ / 4;
	height_small_ = height_ / 4;
	bitmap_ = FreeImage_Rescale(bitmap_, width_, height_);

	x_ = (width_ / 32) + 1;
	Console::log("IMAGE DIMENSIONS SIZE: " + std::to_string(width_) + " , " + std::to_string(height_));
	y_ = (height_ / 32) + 1;
	Console::log(
		"COMPUTE DISPATCH SIZE: " + std::to_string(x_) + " , " + std::to_string(y_) + " = " + std::to_string(x_ * y_) +
		" WORK GROUPS");

	data_ = static_cast<unsigned char*>(malloc(width_ * height_ * bpp_ * sizeof(unsigned char)));
	Console::log("memcpy image bits");
	memcpy(data_, FreeImage_GetBits(bitmap_), width_ * height_ * (bpp_ / 8));

	data_low_res_ = static_cast<unsigned char*>(malloc(width_small_ * height_small_ * bpp_ * sizeof(unsigned char)));
	bitmap_ = FreeImage_Rescale(bitmap_, width_small_, height_small_);
	memcpy(data_low_res_, FreeImage_GetBits(bitmap_), width_small_ * height_small_ * (bpp_ / 8));
	Console::log("memcpy finished");

	thread_image_loaded_loaded_ = true;
	loading_ = false;
	Console::log("Image Loaded!");
}

void Image::bind_image()
{
	update_preview_size();
	if (data_)
	{
		glBindTexture(GL_TEXTURE_2D, texture_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, data_);
	}
	else
	{
		Console::log("Failed to load texture");
	}
	if (data_low_res_)
	{
		glBindTexture(GL_TEXTURE_2D, texture_low_res_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_small_, height_small_, 0, GL_RGB, GL_UNSIGNED_BYTE, data_low_res_);
	}
	glBindTexture(GL_TEXTURE_2D, imgui_preview_texture_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(preview_size_.x), static_cast<GLsizei>(preview_size_.y),
	             0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, imgui_preview_texture_, 0);

	glBindTexture(GL_TEXTURE_2D, comp_texture_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindImageTexture(0, comp_texture_, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glBindTexture(GL_TEXTURE_2D, comp_texture_small_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_small_, height_small_, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindImageTexture(2, comp_texture_small_, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void Image::exportImage(const char* file_loc)
{
	exporting_ = true;
	Console::log("Exporting to: " + std::string(file_loc));

	GLenum error = 0;


	export_data_ = static_cast<GLubyte*>(malloc(width_ * height_ * (bpp_ / 8) * sizeof(GLfloat)));
	memset(export_data_, 0, width_ * height_ * (bpp_ / 8));
	glGenBuffers(1, &pbo_);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_);
	glBufferData(GL_PIXEL_PACK_BUFFER, width_ * height_ * (bpp_ / 8) * sizeof(GLfloat), nullptr, GL_STREAM_READ);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, comp_texture_);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	const double start = glfwGetTime();
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	Console::log("glGetTexImage time: " + std::to_string(glfwGetTime() - start));
	const auto* d = static_cast<GLfloat*>(glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY));
	std::memcpy(export_data_, d, sizeof(GLfloat) * width_ * height_ * (bpp_ / 8));

	renderer_ = std::thread(&Image::render_image, this, file_loc);

	GLubyte foo = export_data_[2000];
	foo = export_data_[2010];

	update_preview_size();

	bind_image();
}

void Image::render_image(const char* file_loc)
{
	rendering = true;
	const double start = glfwGetTime();
	stbi_write_jpg(file_loc, width_, height_, (bpp_ / 8), export_data_, 100);
	free(export_data_);
	Console::log("stbi_write_jpg time: " + std::to_string(glfwGetTime() - start));
	rendering = false;
}

void Image::set_changes(SliderValues* slider_values)
{
	vals_ = slider_values;

	// TODO implement this properly
	changed_ = true;
}

unsigned char* Image::get_data() const
{
	return data_;
}

unsigned Image::get_height() const
{
	if (vals_->show_low_res)
		return height_small_;
	return height_;
}

unsigned Image::get_width() const
{
	if (vals_->show_low_res)
		return width_small_;
	return width_;
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

void Image::init()
{
	model_ = glm::translate(model_, glm::vec3(0, 0, 0));
	view_ = glm::translate(view_, glm::vec3(0.0f, -0.1f, -3.0f));
	projection_ = glm::ortho(-1.0f + zoom_, 1.0f - zoom_, -1.0f + zoom_, 1.0f - zoom_, 0.1f, 1000.0f);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	constexpr float vertices[] = {
		// positions          // colors           // texture coords
		1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
		1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
		-1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left 
	};
	const unsigned indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3 // second triangle
	};

	// TODO make this a function? vbo?
	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo_);
	glGenBuffers(1, &ebo_);

	glBindVertexArray(vao_);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);
	// all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glGenTextures(1, &texture_low_res_);
	glBindTexture(GL_TEXTURE_2D, texture_low_res_);
	// all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &framebuffer_);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

	// The texture we're going to render to
	glGenTextures(1, &imgui_preview_texture_);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, imgui_preview_texture_);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, imgui_preview_texture_, 0);


	// Set the list of draw buffers.
	constexpr GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, draw_buffers); // "1" is the size of DrawBuffers


	glGenTextures(1, &comp_texture_);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, comp_texture_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 0, 0, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindImageTexture(0, comp_texture_, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glGenTextures(1, &comp_texture_small_);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, comp_texture_small_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 0, 0, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindImageTexture(2, comp_texture_small_, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glGenTextures(1, &vectorscope_);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, vectorscope_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 255, 255, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindImageTexture(4, vectorscope_, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glGenTextures(1, &waveform_);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, waveform_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512, 255, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindImageTexture(5, waveform_, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glGenBuffers(1, &ssbo_);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 256 * 4 * sizeof(unsigned), nullptr, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_);

	glGenBuffers(1, &ssbo_orig_);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_orig_);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 256 * sizeof(unsigned), nullptr, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo_orig_);

	glGenBuffers(1, &waveform_acc_);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, waveform_acc_);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 512 * 255 * sizeof(unsigned), nullptr, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, waveform_acc_);

	glGenBuffers(1, &vectorscope_acc_);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vectorscope_acc_);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 255 * 255 * sizeof(unsigned), nullptr, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, vectorscope_acc_);


	shaderLoadTime = Overlay::register_metric();
	imageRender = Overlay::register_metric();
}

void Image::render_to_frame_buffer() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
	glViewport(0, 0, preview_size_.x, preview_size_.y);
}
void Image::glrender(const bool* clip, const bool* b4, const bool* black_bckgrd)
{
	if (!visible) { return; }
	if (loading_) return;

	if (thread_image_loaded_loaded_)
	{
		Console::log("Image loaded to main thread");
		bind_image();
		if (image_loader.joinable())
			image_loader.join();
		(*hist_)->init();
		thread_image_loaded_loaded_ = false;
		image_loaded = true;
		changed_ = true;
		view_ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
		model_ = glm::mat4(1.0f);
		if (size.x > size.y)
		{
			scale(glm::vec3(size.y / height_));
			translate(glm::vec3(-(size.x / width_) * (static_cast<float>(width_) / height_), -(size.y / height_),
			                    0.0f));
		}
		else
		{
			scale(glm::vec3(size.x / width_));
			translate(glm::vec3(-(size.x / width_), -(size.y / height_) * (static_cast<float>(height_) / width_),
			                    0.0f));
		}
		std::fill_n(cdf, 256, 1.0f / 8.0f);
	}


	if (renderer_.joinable() && !rendering)
	{
		Console::log("Image rendered, join to main thread");
		Status::set_status("Image Exported!");
		renderer_.join();
	}

	const double start = glfwGetTime();
	render_to_frame_buffer();
	*shaderLoadTime = "Framebuffer render time: " + std::to_string(glfwGetTime() - start);

	const bool low_b4 = vals_->show_low_res;

	vals_->show_low_res |= scrolling_;
	if ((vals_->show_low_res) && !need_texture_change_)
	{
		need_texture_change_ = true;
	}
	else if (!vals_->show_low_res && need_texture_change_)
	{
		scope_rerender_ = true;
		need_texture_change_ = false;
	}

	if ((!get_changed() && !scrolling_ && !scope_rerender_) || !image_loaded)
	{
		scrolling_ = false;
		vals_->show_low_res = low_b4;
		return;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, comp_texture_);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, comp_texture_small_);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture_low_res_);


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
	glDispatchCompute(x_, y_, 1);

	if ((get_changed() && image_loaded) || (!get_changed() && image_loaded && scope_rerender_))
	{
		glMemoryBarrier(GL_ALL_BARRIER_BITS); // TODO optimise what barriers are needed
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
		glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
		std::fill_n(histogram, 256 * 4, 0);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_orig_);
		glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
		std::fill_n(hist_orig, 256, 0);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, waveform_acc_);
		glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, vectorscope_acc_);
		glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, comp_texture_);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, comp_texture_small_);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texture_low_res_);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, vectorscope_);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, waveform_);

		hist_compute_shader_.use();
		hist_compute_shader_.setBool("low_res", vals_->show_low_res);
		hist_compute_shader_.setBool("histogram_loaded", histogram_loaded);
		hist_compute_shader_.setFloat("var_mult", vals_->scope_brightness);


		glDispatchCompute(x_, y_, 1);

		glMemoryBarrier(GL_ALL_BARRIER_BITS); // TODO optimise what barriers are needed


		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 256 * 4 * sizeof(unsigned), static_cast<GLvoid*>(histogram));
		glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
		if (!histogram_loaded)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_orig_);
			glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 256 * sizeof(unsigned), static_cast<GLvoid*>(hist_orig));
			glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
			float cum = 0;
			for (int i = 0; i < 256; i++)
			{
				cum += static_cast<float>(hist_orig[i]) / static_cast<float>(width_ * height_);
				cdf[i] = cum;
			}
		}
		histogram_loaded = true;
		changed_ = false;
		if (!get_changed() && scope_rerender_)
			scope_rerender_ = false;
	}
	glBindTexture(GL_TEXTURE_2D, 0);


	glMemoryBarrier(GL_ALL_BARRIER_BITS); // TODO optimise what barriers are needed


	if (!*black_bckgrd)
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	else
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, comp_texture_);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, comp_texture_small_);

	shader_.use();
	shader_.setBool("low_res", vals_->show_low_res);
	shader_.setMat4("model", model_);
	shader_.setMat4("view", view_);
	shader_.setMat4("projection", projection_);

	shader_.setFloat("texelWidth", 1.0f / width_);
	shader_.setFloat("texelHeight", 1.0f / height_);


	glBindVertexArray(vao_);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

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
	if (loading_)
	{
		ImGui::SetCursorPos(size / 2 - ImVec2(50, 50));
		spinner("LOADING...", 100, 10, ImGui::GetColorU32(ImVec4(255, 255, 255, 255)));
	}
	else ImGui::Image((ImTextureID)imgui_preview_texture_, preview_size_);
	ImGui::End();
	*imageRender = "ImGui Window Render Time: " + std::to_string(glfwGetTime() - start);
}

void Image::cleanup()
{
	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(1, &vbo_);
	glDeleteBuffers(1, &ebo_);
}

float Image::calc_curve(float t, const int channel) const
{
	t *= pow(2.0f, vals_->expo[channel]) * (1.0f + vals_->whites[channel]);
	const float g = (4.0f * pow((1.0f - t), 3.0f) * t * (0.25f + vals_->low[channel] - vals_->contrast[channel]))
		+ (6.0f * pow((1 - t), 2.0f) * pow(t, 2.0f) * (0.5f + vals_->mid[channel]))
		+ (4.0f * (1.0f - t) * pow(t, 3.0f) * (0.75f + vals_->high[channel] + vals_->contrast[channel]))
		+ (pow(t, 4.0f) * 1.0f);
	return pow(vals_->gain[channel] * ((1.0 - vals_->lift[channel]) * g + vals_->lift[channel]), vals_->gamma[channel]);
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
