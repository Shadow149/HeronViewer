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

#define RENDER_WIDTH 1920
#define SMALL_IMG_MAX 120

class Histogram;

class Image : public Module
{
private:
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 model1 = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::vec3 view_pos = glm::vec3(0.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	float zoom = 0.0f;
	float scale_factor = 1.0f;

	GLuint FramebufferName = 0, small_framebuffer = 1;
	GLuint renderedTexture, small_framebuffer_tex;
	unsigned int VBO, VAO, EBO;
	GLuint pbo;

	int small_img_height = 1, small_img_width = 1;
	unsigned int height = 1, width = 1, bpp = 0;
	unsigned char* data;
	unsigned char* data1;
	GLubyte* small_data;
	GLubyte* export_data;
	FIBITMAP* temp;
	FIBITMAP* bitmap;
	FIBITMAP* scaled;

	/*
	float low = LOW_DEFAULT;
	float mid = MID_DEFAULT;
	float high = HIGH_DEFAULT;
	float gamma = GAMMA_DEFAULT;
	float A = A_DEFAULT;
	float contrast = CONTRAST_DEFAULT;
	*/

	float* low;
	float* mid;
	float* high;
	float* contrast;
	float* exp;
	float* whites;

	float sat;
	float wb;

	float* sharp_kernel;
	bool bw = false;
	//float* sharp_kernel33;
	bool changed = false;

	float high_thresh = 1;
	float shad_thresh = 0;
	float high_incr = 1;
	float shad_incr = 1;

	float shad_var = 0.07;
	float var_mult = 2;

	unsigned int texture;
	unsigned int texture1;

	bool threadImageLoaded = false;
	bool exporting = false;
	GLsync sync;
	int index = 0;
	int nextIndex = 0;
	float* src;
	
	Shader shader = Shader("C:\\Users\\Alfred Roberts\\Documents\\projects\\HeronViewer\\HeronViewer\\src\\texture.vert", "C:\\Users\\Alfred Roberts\\Documents\\projects\\HeronViewer\\HeronViewer\\src\\texture.frag");
	ComputeShader computeShader = ComputeShader("C:\\Users\\Alfred Roberts\\Documents\\projects\\HeronViewer\\HeronViewer\\src\\texture.comp");
	Histogram** hist;

	std::string* shaderLoadTime;
	std::string* imageRender;
	std::string* exportStat;

	std::thread renderer;

	unsigned int comp_texture;
	GLuint SSBO;


	void renderImage(const char* fileLoc);
	void updatePreviewSize();

public:
	ImVec2 size;
	ImVec2 previewSize;

	bool imageLoaded = false;
	bool rendering = false;
	std::thread imageLoader;

	unsigned histogram[256];
	bool histogram_loaded = false;
	
	void unload();
	void getImage(const char* filename);
	void setChanges(float* l, float* w, float* m, float* h, float* e, float* c, float* sKer, bool bw, float ht, float st, float ha, float sa,
		float sv, float vm, float sat, float wb);
	unsigned char* getData();
	unsigned int getHeight();
	unsigned int getWidth();
	bool getChanged();
	void recompileShader();

	Image(std::string n, Histogram** hist, bool v = true) : Module(n, v) {
		this->hist = hist;
	}
	//~Image() { imageLoader.join(); }
	void init();
	void renderToFrameBuffer();
	void glrender(bool* clip, bool* b4, bool* black_bckgrd);
	void render();
	void cleanup();
	void bindImage();

	void exportImage(const char* fileLoc);
	void initExport();
	void exportPBO();

	float calcCurve(float t, int chr);
	void calcCurveHist(float t, int chr, float* red, float* green, float* blue);

	void scale(glm::vec3 s);
	void translate(glm::vec3 t);
	void resetView();
};

