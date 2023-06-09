#include "Image.h"
#include "Histogram.h"

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
}

void Image::getImage(const char* filename) {
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

	int bitmap_bpp = FreeImage_GetBPP(bitmap);
	Console::log("Bitmap bpp: " + std::to_string(bitmap_bpp));

	FreeImage_FlipVertical(bitmap);
	//temp = FreeImage_ConvertTo32Bits(bitmap);
	temp = bitmap;

	if (bitmap_bpp > 32)
		temp = FreeImage_ConvertTo32Bits(bitmap);
	else
		temp = FreeImage_ConvertTo24Bits(bitmap);

	FreeImage_AdjustGamma(temp, 2.2f);

	width = FreeImage_GetWidth(temp);
	height = FreeImage_GetHeight(temp);
	bpp = FreeImage_GetBPP(temp);

	if (width > height) {
		small_img_width = SMALL_IMG_MAX;
		small_img_height = SMALL_IMG_MAX * ((float)height / width);
	}
	else {
		small_img_width = SMALL_IMG_MAX * ((float)width / height);
		small_img_height = SMALL_IMG_MAX;
	}


	data = (unsigned char*)malloc(width * height * bpp * sizeof(unsigned char));
	Console::log("memcpy image bits");
	memcpy(data, FreeImage_GetBits(temp), static_cast<size_t>(width) * height * (bpp / 8));
	Console::log("memcpy finished");


	threadImageLoaded = true;
	Console::log("Image Loaded!");
}

void Image::bindImage() {
	updatePreviewSize();
	glBindTexture(GL_TEXTURE_2D, texture); 
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		Console::log("Failed to load texture");
	}
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, previewSize.x, previewSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
}

void Image::exportImage(const char* fileLoc) {
	exporting = true;
	
	glGenBuffers(1, &pbo);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
	glBufferData(GL_PIXEL_PACK_BUFFER, width * height * (bpp / 8) * sizeof(unsigned char), 0, GL_DYNAMIC_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	

	export_data = (unsigned char*)malloc(width * height * bpp * sizeof(unsigned char));
	memset(export_data, 0, width * height * (bpp / 8));

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	// bind Texture
	glBindTexture(GL_TEXTURE_2D, texture);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(1.0f));
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	shader.use();
	shader.setMat4("model", model);
	shader.setMat4("view", view);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	float start = glfwGetTime();
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, export_data);
	Console::log("glReadPixels time: " + std::to_string(glfwGetTime() - start));

	renderer = std::thread(&Image::renderImage, this, fileLoc);

	updatePreviewSize();

	bindImage();
}

void Image::renderImage(const char* fileLoc) {
	rendering = true;
	float start = glfwGetTime();
	//stbi_write_png(fileLoc, width, height, (bpp / 8), export_data, width * (bpp / 8));
	stbi_write_jpg(fileLoc, width, height, (bpp / 8), export_data, 70);
	free(export_data);
	Console::log("stbi_write_jpg time: " + std::to_string(glfwGetTime() - start));
	rendering = false;
}

void Image::setChanges(float* l, float* w, float* m, float* h, float* e, float* c, float* sKer, bool bw, float ht, float st, float ha, float sa,
	float sv, float vm, float sat, float wb)
{
	// TODO limits
	low = l;
	whites = w;
	mid = m;
	high = h;
	//gamma = g;
	exp = e;
	contrast = c;
	sharp_kernel = sKer;
	this->bw = bw;

	this->sat = sat;
	this->wb = wb;

	high_thresh = ht;
	shad_thresh = st;
	high_incr = ha;
	shad_incr = sa;

	shad_var = sv;
	var_mult = vm;

	// TODO implement this properly
	changed = true;
}

unsigned char* Image::getData()
{
	return data;
}

unsigned int Image::getHeight()
{
	return height;
}

unsigned int Image::getWidth()
{
	return width;
}

bool Image::getChanged()
{
	return changed;
}

void Image::recompileShader()
{
	shader = Shader("C:\\Users\\Alfred Roberts\\Documents\\projects\\HeronViewer\\HeronViewer\\src\\texture.vs", "C:\\Users\\Alfred Roberts\\Documents\\projects\\HeronViewer\\HeronViewer\\src\\texture.fs");
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
	//imageLoader = std::thread(&Image::getImage, this, "C:\\Users\\Alfred Roberts\\Pictures\\U75A1167.CR2");

	shaderLoadTime = Overlay::registerMetric();
	imageRender = Overlay::registerMetric();

	
	
}

void Image::renderToFrameBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glViewport(0, 0, previewSize.x, previewSize.y);
}

void Image::glrender(bool* clip, bool* b4) {
	if (!visible) { return; }

	if (threadImageLoaded) { 
		Console::log("Image loaded to main thread");
		bindImage();
		if (imageLoader.joinable())
			imageLoader.join();
		(*hist)->init();
		threadImageLoaded = false;
		imageLoaded = true;
		if (size.x > size.y)
			model = glm::scale(glm::mat4(1.0f), glm::vec3((float)size.y / (previewSize.y + 200)));
		else
			model = glm::scale(glm::mat4(1.0f), glm::vec3((float)size.x / (previewSize.x + 200)));
	}

	if (renderer.joinable() && !rendering) {
		Console::log("Image rendered, join to main thread");
		Status::setStatus("Image Exported!");
		renderer.join();
	}

	float start = glfwGetTime();
	renderToFrameBuffer();
	*shaderLoadTime = "Framebuffer render time: " + std::to_string(glfwGetTime() - start);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	// bind Texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// render container
	shader.use();
	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	shader.setFloatArray("low", low, 4);
	shader.setFloatArray("whites", whites, 4);
	shader.setFloatArray("mid", mid, 4);
	shader.setFloatArray("high", high, 4);
	shader.setFloatArray("contrast", contrast, 4);
	shader.setFloatArray("exp", exp, 4);
	shader.setBool("bw", bw);

	shader.setFloat("sat", sat);
	shader.setFloat("wb", wb);

	//shader.setFloatArray("sharp_kernel", sharp_kernel, 25);
	shader.setFloatArray("sharp_kernel33", sharp_kernel, 9);

	shader.setBool("clip", *clip);
	shader.setBool("b4", *b4);


	shader.setFloat("high_thresh", high_thresh);
	shader.setFloat("shad_thresh", shad_thresh);
	shader.setFloat("high_incr", high_incr);
	shader.setFloat("shad_incr", shad_incr);

	shader.setFloat("shad_var", shad_var);
	shader.setFloat("var_mult", var_mult);

	shader.setFloat("texelWidth", 1.0f / width);
	shader.setFloat("texelHeight", 1.0f / width);

 
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void Image::render()
{
	if (!visible) { return; }
	float start = glfwGetTime();
	ImGui::Begin(name.c_str(), &visible, 8 | 16);
	size = ImGui::GetWindowSize();
	updateMouseInWindow();
	ImGui::Image((void*)(intptr_t)renderedTexture, previewSize);
	ImGui::End();
	*imageRender = "Image Render Time: " + std::to_string(glfwGetTime() - start);
	changed = false;
}

void Image::cleanup()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	//free(small_data);
}

float Image::calcCurve(float t, int chr)
{
	//std::cout << exp[0] << exp[1] << exp[2] << exp[3] << std::endl;
	t *= pow(2, exp[chr]) * (1+whites[chr]);
	return (4 * pow((1 - t), 3) * t * (0.25 + low[chr] - contrast[chr])) + (6 * pow((1 - t), 2) * pow(t, 2) * (0.5 + mid[chr])) + (4 * (1 - t) * pow(t, 3) * (0.75 + high[chr] + contrast[chr])) + (pow(t, 4) * 1);
}

void Image::calcCurveHist(float v, int chr, float* red, float* green, float* blue) {
	float t = 0;
	if (chr == 1) { // red
		t = v * exp[chr];
		*red   = (4 * pow((1 - t), 3) * t * (0.25 + low[chr] - contrast[chr])) + (6 * pow((1 - t), 2) * pow(t, 2) * (0.5 + mid[chr])) + (4 * (1 - t) * pow(t, 3) * (0.75 + high[chr] + contrast[chr])) + (pow(t, 4) * 1);
		t = v * -exp[chr];
		*green = (4 * pow((1 - t), 3) * t * (0.25 + -low[chr] - -contrast[chr])) + (6 * pow((1 - t), 2) * pow(t, 2) * (0.5 + -mid[chr])) + (4 * (1 - t) * pow(t, 3) * (0.75 + -high[chr] + -contrast[chr])) + (pow(t, 4) * 1);
		t = v * -exp[chr];
		*blue  = (4 * pow((1 - t), 3) * t * (0.25 + -low[chr] - -contrast[chr])) + (6 * pow((1 - t), 2) * pow(t, 2) * (0.5 + -mid[chr])) + (4 * (1 - t) * pow(t, 3) * (0.75 + -high[chr] + -contrast[chr])) + (pow(t, 4) * 1);
	}
	if (chr == 2) { // green
		t = v * -exp[chr];
		*red = (4 * pow((1 - t), 3) * t * (0.25 + -low[chr] - -contrast[chr])) + (6 * pow((1 - t), 2) * pow(t, 2) * (0.5 + -mid[chr])) + (4 * (1 - t) * pow(t, 3) * (0.75 + -high[chr] + -contrast[chr])) + (pow(t, 4) * 1);
		t = v * exp[chr];
		*green = (4 * pow((1 - t), 3) * t * (0.25 + low[chr] - contrast[chr])) + (6 * pow((1 - t), 2) * pow(t, 2) * (0.5 + mid[chr])) + (4 * (1 - t) * pow(t, 3) * (0.75 + high[chr] + contrast[chr])) + (pow(t, 4) * 1);
		t = v * -exp[chr];
		*blue = (4 * pow((1 - t), 3) * t * (0.25 + -low[chr] - -contrast[chr])) + (6 * pow((1 - t), 2) * pow(t, 2) * (0.5 + -mid[chr])) + (4 * (1 - t) * pow(t, 3) * (0.75 + -high[chr] + -contrast[chr])) + (pow(t, 4) * 1);
	}

	if (chr == 3) { // blue
		t = v * -exp[chr];
		*red = (4 * pow((1 - t), 3) * t * (0.25 + -low[chr] - -contrast[chr])) + (6 * pow((1 - t), 2) * pow(t, 2) * (0.5 + -mid[chr])) + (4 * (1 - t) * pow(t, 3) * (0.75 + -high[chr] + -contrast[chr])) + (pow(t, 4) * 1);
		t = v * -exp[chr];
		*green = (4 * pow((1 - t), 3) * t * (0.25 + -low[chr] - -contrast[chr])) + (6 * pow((1 - t), 2) * pow(t, 2) * (0.5 + -mid[chr])) + (4 * (1 - t) * pow(t, 3) * (0.75 + -high[chr] + -contrast[chr])) + (pow(t, 4) * 1);
		t = v * exp[chr];
		*blue = (4 * pow((1 - t), 3) * t * (0.25 + low[chr] - contrast[chr])) + (6 * pow((1 - t), 2) * pow(t, 2) * (0.5 + mid[chr])) + (4 * (1 - t) * pow(t, 3) * (0.75 + high[chr] + contrast[chr])) + (pow(t, 4) * 1);
	}


}

void Image::scale(glm::vec3 s)
{
	if (!mouseInWindow) { return; }
	model = glm::scale(model, s);
}

void Image::translate(glm::vec3 t)
{
	if (!mouseInWindow) { return; }
	view = glm::translate(view, t);
}

void Image::resetView() {
	//scale(glm::vec3(1.0f));
	//translate(glm::vec3(0.0f));
}