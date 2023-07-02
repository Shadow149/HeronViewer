#include "Image.h"
#include "Histogram.h"
#include "Widgets.h"

void Image::updatePreviewSize()
{
	if (width > height)
		previewSize = ImVec2(RENDER_WIDTH * ((float)width / height), RENDER_WIDTH);
	else
		previewSize = ImVec2(RENDER_WIDTH, RENDER_WIDTH * ((float)height / width));
}

void Image::unload()
{
	if (!data) {
		Console::log("No Image to unload");
		return;
	}
	Console::log("Unloading Image");
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
	imageLoaded = false;
	histogram_loaded = false;
}

void Image::getImage(const char* filename) {
	histogram_loaded = false;
	loading = true;
	Console::log("Loading Image... " + std::string(filename));

	if (bitmap)
		FreeImage_Unload(bitmap);
	if (temp)
		FreeImage_Unload(temp);
	if (data)
		free(data);
	if (data1)
		free(data1);

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	fif = FreeImage_GetFileType(filename, 0);

	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(filename);
	if (fif == FIF_UNKNOWN) {
		Console::log("IMAGE LOAD ERROR - UNKNOWN FILE TYPE");
		threadImageLoaded = true;
		return;
	}

	if (FreeImage_FIFSupportsReading(fif)) {
		if (fif == FIF_RAW)
			bitmap = FreeImage_Load(fif, filename, RAW_DISPLAY);
		else
			bitmap = FreeImage_Load(fif, filename);
	}

	if (!bitmap) {
		Console::log("IMAGE LOAD ERROR - IMAGE FAILED TO LOAD");
		threadImageLoaded = true;
		return;
	}

	const unsigned bitmap_bpp = FreeImage_GetBPP(bitmap);
	Console::log("Bitmap bpp: " + std::to_string(bitmap_bpp));

	FreeImage_FlipVertical(bitmap);
	//temp = FreeImage_ConvertTo32Bits(bitmap);
	temp = bitmap;

	//if (bitmap_bpp > 32)
	//	temp = FreeImage_ConvertTo32Bits(bitmap);
	//else
	//	temp = FreeImage_ConvertTo24Bits(bitmap);
	temp = FreeImage_ConvertTo24Bits(bitmap);

	FreeImage_AdjustGamma(temp, 2.5);

	width = FreeImage_GetWidth(temp);
	height = FreeImage_GetHeight(temp);
	bpp = FreeImage_GetBPP(temp);

	Console::log("Bitmap (TEMP) bpp: " + std::to_string(bpp));

	if (width > height && width > SMALL_IMG_MAX) {
		small_img_width = SMALL_IMG_MAX;
		small_img_height = SMALL_IMG_MAX * ((float)height / width);
	}
	else if (height > SMALL_IMG_MAX) {
		small_img_width = SMALL_IMG_MAX * ((float)width / height);
		small_img_height = SMALL_IMG_MAX;
	} else
	{
		small_img_width = width;
		small_img_height = height;
	}
	width = small_img_width;
	height = small_img_height;

	width_small = width / 4;
	height_small = height / 4;
	temp = FreeImage_Rescale(temp, width, height);

	x = (width / 32) + 1;
	Console::log("IMAGE DIMENSIONS SIZE: " + std::to_string(width) + " , " + std::to_string(height));
	y = (height / 32) + 1;
	Console::log("COMPUTE DISPATCH SIZE: " + std::to_string(x) + " , " + std::to_string(y) + " = " + std::to_string(x * y) + " WORK GROUPS");


	data = (unsigned char*)malloc(width * height * bpp * sizeof(unsigned char));
	Console::log("memcpy image bits");
	memcpy(data, FreeImage_GetBits(temp), width * height * (bpp / 8));

	data1 = (unsigned char*)malloc(width_small * height_small * bpp * sizeof(unsigned char));
	temp = FreeImage_Rescale(temp, width_small, height_small);
	memcpy(data1, FreeImage_GetBits(temp), width_small * height_small * (bpp / 8));
	Console::log("memcpy finished");

	threadImageLoaded = true;
	loading = false;
	Console::log("Image Loaded!");
}

void Image::bindImage() {
	updatePreviewSize();
	if (data)
	{
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		Console::log("Failed to load texture");
	}
	if (data1)
	{
		glBindTexture(GL_TEXTURE_2D, texture1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_small, height_small, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
	}
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, previewSize.x, previewSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	glBindTexture(GL_TEXTURE_2D, comp_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, comp_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glBindTexture(GL_TEXTURE_2D, comp_texture_small);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_small, height_small, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(2, comp_texture_small, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);


}

void Image::exportImage(const char* fileLoc) {
	exporting = true;
	Console::log("Exporting to: " + std::string(fileLoc));

	GLenum error = 0;


	export_data = (GLubyte*)malloc(width * height * (bpp / 8) * sizeof(GLfloat));
	memset(export_data, 0, width * height * (bpp / 8));
	glGenBuffers(1, &pbo);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
	glBufferData(GL_PIXEL_PACK_BUFFER, width * height * (bpp / 8) * sizeof(GLfloat), 0, GL_STREAM_READ);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, comp_texture);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	const double start = glfwGetTime();
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	Console::log("glGetTexImage time: " + std::to_string(glfwGetTime() - start));
	const auto* d = (GLfloat*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
	std::memcpy(export_data, d, sizeof(GLfloat) * width * height * (bpp / 8));

	renderer = std::thread(&Image::renderImage, this, fileLoc);

	GLubyte foo = export_data[2000];
	foo = export_data[2010];

	updatePreviewSize();

	bindImage();
}

void Image::renderImage(const char* fileLoc) {
	rendering = true;
	float start = glfwGetTime();
	//stbi_write_png(fileLoc, width, height, (bpp / 8), export_data, width * (bpp / 8));
	stbi_write_jpg(fileLoc, width, height, (bpp / 8), export_data, 100);
	free(export_data);
	Console::log("stbi_write_jpg time: " + std::to_string(glfwGetTime() - start));
	rendering = false;
}

void Image::setChanges(SliderValues* slider_values)
{
	m_pVals = slider_values;

	// TODO implement this properly
	changed = true;
}

unsigned char* Image::getData()
{
	return data;
}

unsigned int Image::getHeight()
{
	printf("%d %d %d\n", m_pVals->show_low_res, need_texture_change, scope_rerender);

	if (m_pVals->show_low_res && need_texture_change)
		return height_small;
	return height;
}

unsigned int Image::getWidth()
{
	if (m_pVals->show_low_res && need_texture_change)
		return width_small;
	return width;
}

bool Image::getChanged()
{
	return changed;
}

void Image::recompileShader()
{
	shader = Shader("C:\\Users\\Alfred Roberts\\Documents\\projects\\HeronViewer\\HeronViewer\\src\\texture.vs", "C:\\Users\\Alfred Roberts\\Documents\\projects\\HeronViewer\\HeronViewer\\src\\texture.fs");
	process_compute_shader_ = ComputeShader("C:\\Users\\Alfred Roberts\\Documents\\projects\\HeronViewer\\HeronViewer\\src\\texture.comp");
	Console::log("Shader Recompiled!");
}

void Image::init()
{

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // colors           // texture coords
		 1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	// TODO make this a function? vbo?
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	
	// The texture we're going to render to
	glGenTextures(1, &renderedTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);


	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &small_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, small_framebuffer);

	// The texture we're going to render to
	glGenTextures(1, &small_framebuffer_tex);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, small_framebuffer_tex);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, small_framebuffer_tex, 0);


	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	model = glm::translate(model, glm::vec3(0, 0, 0));
	view = glm::translate(view, glm::vec3(0.0f, -0.1f, -3.0f));
	projection = glm::ortho(-1.0f + zoom, 1.0f - zoom, -1.0f + zoom, 1.0f - zoom, 0.1f, 1000.0f);

	model1 = glm::translate(model1, glm::vec3(0, 0, 1));
	model1 = glm::scale(model1, glm::vec3(0.25f));

	glGenTextures(1, &comp_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, comp_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 0, 0, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, comp_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glGenTextures(1, &comp_texture_small);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, comp_texture_small);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 0, 0, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(2, comp_texture_small, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glGenTextures(1, &vectorscope);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, vectorscope);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 255, 255, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(4, vectorscope, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glGenTextures(1, &waveform);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, waveform);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512, 255, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(5, waveform, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glGenBuffers(1, &SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 256 * 4 * sizeof(unsigned), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SSBO);

	glGenBuffers(1, &SSBO_orig);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_orig);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 256 * sizeof(unsigned), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, SSBO_orig);

	glGenBuffers(1, &waveform_acc);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, waveform_acc);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 512 * 255 * sizeof(unsigned), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, waveform_acc);

	glGenBuffers(1, &vectorscope_acc);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vectorscope_acc);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 255 * 255 * sizeof(unsigned), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, vectorscope_acc);


	shaderLoadTime = Overlay::registerMetric();
	imageRender = Overlay::registerMetric();

}

void Image::renderToFrameBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glViewport(0, 0, previewSize.x, previewSize.y);
}

void Image::glrender(bool* clip, bool* b4, bool* black_bckgrd) {
	if (!visible) { return; }
	if (loading) return;


	if (threadImageLoaded) { 
		Console::log("Image loaded to main thread");
		bindImage();
		if (imageLoader.joinable())
			imageLoader.join();
		(*hist)->init();
		threadImageLoaded = false;
		imageLoaded = true;
		changed = true;
		//view = glm::mat4(1.0f);
		view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
		model = glm::mat4(1.0f);
		if (size.x > size.y) {
			scale(glm::vec3((float)size.y / height));
			translate(glm::vec3(-(size.x / width) * ((float)width/height), -(size.y / height), 0.0f));
		}
		else {
			scale(glm::vec3((float)size.x / width));
			translate(glm::vec3(-(size.x / width), -(size.y / height) * ((float)height / width), 0.0f));
		}
		std::fill_n(cdf, 256, 1.0f / 8.0f);
	}


	if (renderer.joinable() && !rendering) {
		Console::log("Image rendered, join to main thread");
		Status::setStatus("Image Exported!");
		renderer.join();
	}

	float start = glfwGetTime();
	renderToFrameBuffer();
	*shaderLoadTime = "Framebuffer render time: " + std::to_string(glfwGetTime() - start);
	//glMemoryBarrier(GL_ALL_BARRIER_BITS); // TODO optimise what barriers are needed

	const bool low_b4 = m_pVals->show_low_res;


	m_pVals->show_low_res |= scrolling;
	if ((m_pVals->show_low_res) && !need_texture_change) {
		need_texture_change = true;
	}
	else if (!m_pVals->show_low_res && need_texture_change) {
		scope_rerender = true;
		need_texture_change = false;
	}

	if ((!getChanged() && !scrolling && !scope_rerender) || !imageLoaded) {
		scrolling = false;
		m_pVals->show_low_res = low_b4;
		return;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, comp_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, comp_texture_small);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture1);


	process_compute_shader_.use();
	process_compute_shader_.setBool("low_res", m_pVals->show_low_res);
	process_compute_shader_.setFloatArray("low", m_pVals->low, 4);
	process_compute_shader_.setFloatArray("whites", m_pVals->whites, 4);
	process_compute_shader_.setFloatArray("mid", m_pVals->mid, 4);
	process_compute_shader_.setFloatArray("high", m_pVals->high, 4);
	process_compute_shader_.setFloatArray("contrast", m_pVals->contrast, 4);
	process_compute_shader_.setFloatArray("expo", m_pVals->expo, 4);

	process_compute_shader_.setFloat("sat_ref", m_pVals->sat_ref);

	process_compute_shader_.setFloatArray("lift", m_pVals->lift, 4);
	process_compute_shader_.setFloatArray("gamma", m_pVals->gamma, 4);
	process_compute_shader_.setFloatArray("gain", m_pVals->gain, 4);

	process_compute_shader_.setBool("inv", m_pVals->inv);
	process_compute_shader_.setBool("bw", m_pVals->bw);
	process_compute_shader_.setFloat("sat", m_pVals->sat);
	process_compute_shader_.setFloat("wb", m_pVals->wb);
	process_compute_shader_.setFloat("tint", m_pVals->tint);
	process_compute_shader_.setBool("clip", *clip);
	process_compute_shader_.setBool("b4", *b4);

	process_compute_shader_.setFloat("yiq_y", m_pVals->yiq_y);
	process_compute_shader_.setFloat("yiq_z", m_pVals->yiq_z);
	process_compute_shader_.setFloat("xyz_y", m_pVals->xyz_y);
	process_compute_shader_.setFloat("xyz_z", m_pVals->xyz_z);
	process_compute_shader_.setBool("noise_selected", m_pVals->noise_selected);
	process_compute_shader_.setFloat("noise", m_pVals->noise);

	process_compute_shader_.setFloatArray("hues", m_pVals->hues, 8);
	process_compute_shader_.setFloatArray("sats", m_pVals->sats, 8);
	process_compute_shader_.setFloatArray("lums", m_pVals->lums, 8);

	process_compute_shader_.setFloatArray("sharp_kernel", m_pVals->sharp_kernel, 9);

	process_compute_shader_.setFloatArray("cdf", cdf, 256);
	process_compute_shader_.setBool("histogram_loaded", histogram_loaded);
	glDispatchCompute(x, y, 1);

	if ((getChanged() && imageLoaded) || (!getChanged() && imageLoaded && scope_rerender)) {
		scope_rendered = false;
		glMemoryBarrier(GL_ALL_BARRIER_BITS); // TODO optimise what barriers are needed
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
		glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
		std::fill_n(histogram, 256 * 4, 0);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_orig);
		glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
		std::fill_n(hist_orig, 256, 0);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, waveform_acc);
		glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, vectorscope_acc);
		glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, comp_texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, comp_texture_small);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, vectorscope);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, waveform);

		hist_compute_shader_.use();
		hist_compute_shader_.setBool("low_res", m_pVals->show_low_res);
		hist_compute_shader_.setBool("histogram_loaded", histogram_loaded);
		hist_compute_shader_.setFloat("var_mult", m_pVals->scope_brightness);


		glDispatchCompute(x, y, 1);
		//loops++;


		glMemoryBarrier(GL_ALL_BARRIER_BITS);  // TODO optimise what barriers are needed


		glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 256 * 4 * sizeof(unsigned), (GLvoid*)histogram);
		glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
		if (!histogram_loaded)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_orig);
			glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 256 * sizeof(unsigned), (GLvoid*)hist_orig);
			glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
			float cum = 0;
			for (int i = 0; i < 256; i++)
			{
				cum += (float)hist_orig[i] / float(width * height);
				cdf[i] = cum;
			}
		}
		histogram_loaded = true;
		changed = false;
		scope_rendered = true;
		if (!getChanged() && scope_rerender)
			scope_rerender = false;
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
	glBindTexture(GL_TEXTURE_2D, comp_texture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, comp_texture_small);

	shader.use();
	shader.setBool("low_res", m_pVals->show_low_res);
	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	shader.setFloat("texelWidth", 1.0f / width);
	shader.setFloat("texelHeight", 1.0f / height);

 
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	scrolling = false;
	m_pVals->show_low_res = low_b4;
}


void Image::render()
{
	glMemoryBarrier(GL_ALL_BARRIER_BITS); // TODO optimise what barriers are needed

	float start = glfwGetTime();
	ImGui::Begin(name.c_str(), &visible, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	size = ImGui::GetWindowSize();
	updateMouseInWindow();
	if (loading) {
		ImGui::SetCursorPos(size / 2 - ImVec2(50, 50));
		Spinner("LOADING...", 100, 10, ImGui::GetColorU32(ImVec4(255, 255, 255, 255)));
	}
	else ImGui::Image((ImTextureID)renderedTexture, previewSize);
	ImGui::End();
	*imageRender = "ImGui Window Render Time: " + std::to_string(glfwGetTime() - start);
}

void Image::cleanup()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

float Image::calcCurve(float t, int chr)
{
	t *= pow(2, m_pVals->expo[chr]) * (1+m_pVals->whites[chr]);
	float g =  (4 * pow((1 - t), 3) * t * (0.25 + m_pVals->low[chr] - m_pVals->contrast[chr]))
		+ (6 * pow((1 - t), 2) * pow(t, 2) * (0.5 + m_pVals->mid[chr]))
		+ (4 * (1 - t) * pow(t, 3) * (0.75 + m_pVals->high[chr] + m_pVals->contrast[chr]))
		+ (pow(t, 4) * 1);
	return pow(m_pVals->gain[chr] * ((1.0 - m_pVals->lift[chr]) * g + m_pVals->lift[chr]), m_pVals->gamma[chr]);
}

void Image::scale(glm::vec3 s)
{
	if (!mouseInWindow) { return; }
	scale_factor = s.x;
	model = glm::scale(model, s);
	scrolling = true;
}

void Image::translate(glm::vec3 t)
{
	if (!mouseInWindow) { return; }
	view = glm::translate(view, t);
	scrolling = true;
}

void Image::resetView() {
	//scale(glm::vec3(1.0f));
	//translate(glm::vec3(0.0f));
}