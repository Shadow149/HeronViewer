#include "HeronImage.h"

#include <iostream>

#include "Console.h"
#include "FreeImage.h"
#include "Status.h"
#include "stb_image_write.h"
#include "GLFW/glfw3.h"

void HeronImage::load_image(std::string& filename)
{
	std::cout << filename << std::endl;
	image_loader_ = std::thread(&HeronImage::read_image, this, filename);
}

void HeronImage::finish_export()
{
	Console::log("Image rendered, join to main thread");
	Status::set_status("Image Exported!");
	renderer_.join();
}

void HeronImage::unload()
{
	if (img_data_)
		free(img_data_);
	else if (!image_loaded_) { Console::log("No Image to unload"); return; }
	if (lr_img_data_)
		free(lr_img_data_);
	image_loaded_ = false;
	if (image_loader_.joinable()) image_loader_.join();
	Console::log("Unloading Image");
}

void HeronImage::read_image(const std::string& filename)
{
	finished_loading_ = false;
	loading_ = true;

	Console::log("Loading Image... " + filename);
	FIBITMAP* fi_bitmap = nullptr;

	const char* f_c_str = filename.c_str();

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(f_c_str, 0);

	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(f_c_str);
	if (fif == FIF_UNKNOWN)
	{
		Console::log("IMAGE LOAD ERROR - UNKNOWN FILE TYPE");
		return;
	}

	if (FreeImage_FIFSupportsReading(fif))
	{
		if (fif == FIF_RAW)
			fi_bitmap = FreeImage_Load(fif, f_c_str, RAW_DISPLAY);
		else
			fi_bitmap = FreeImage_Load(fif, f_c_str);
	}

	if (!fi_bitmap)
	{
		Console::log("IMAGE LOAD ERROR - IMAGE FAILED TO LOAD");
		return;
	}

	const unsigned fi_bitmap_bpp = FreeImage_GetBPP(fi_bitmap);
	Console::log("Bitmap bpp: " + std::to_string(fi_bitmap_bpp));
	FreeImage_FlipVertical(fi_bitmap);
	fi_bitmap = FreeImage_ConvertTo24Bits(fi_bitmap);
	FreeImage_AdjustGamma(fi_bitmap, 2.5);

	width_ = static_cast<GLsizei>(FreeImage_GetWidth(fi_bitmap));
	height_ = static_cast<GLsizei>(FreeImage_GetHeight(fi_bitmap));
	bpp_ = static_cast<GLsizei>(FreeImage_GetBPP(fi_bitmap));
	Console::log("Bitmap bpp: " + std::to_string(bpp_));

	GLsizei small_img_height, small_img_width;
	resize_image(width_, height_, SMALL_IMG_MAX, small_img_width, small_img_height);
	width_ = small_img_width;
	height_ = small_img_height;
	Console::log("IMAGE DIMENSIONS SIZE: " + std::to_string(width_) + " , " + std::to_string(height_));

	resize_image(width_, height_, LOW_RES_IMG_MAX, lr_width_, lr_height_);
	fi_bitmap = FreeImage_Rescale(fi_bitmap, width_, height_);
	FIBITMAP* lr_fi_bitmap = FreeImage_Rescale(fi_bitmap, lr_width_, lr_height_);

	dispatch_size_ = glm::ivec2((width_ / 32) + 1, (height_ / 32) + 1);
	Console::log(
		"COMPUTE DISPATCH SIZE: " + std::to_string(dispatch_size_.x) + " , " + std::to_string(dispatch_size_.y));

	Console::log("memcpy image bits");
	img_data_ = static_cast<unsigned char*>(malloc(width_ * height_ * bpp_ * sizeof(unsigned char)));
	memcpy(img_data_, FreeImage_GetBits(fi_bitmap), width_ * height_ * (bpp_ / 8));

	lr_img_data_ = static_cast<unsigned char*>(malloc(lr_width_ * lr_height_ * bpp_ * sizeof(unsigned char)));
	memcpy(lr_img_data_, FreeImage_GetBits(lr_fi_bitmap), lr_width_ * lr_height_ * (bpp_ / 8));
	Console::log("memcpy finished");

	if (fi_bitmap)
		FreeImage_Unload(fi_bitmap);
	if (lr_fi_bitmap)
		FreeImage_Unload(lr_fi_bitmap);

	Console::log("Image Loaded!");
	loading_ = false;
	finished_loading_ = true;
	image_loaded_ = true;
}

void HeronImage::export_image(const char* file_loc, const gl_image& image)
{
	exporting_ = true;
	Console::log("Exporting to: " + std::string(file_loc));

	export_data_ = static_cast<GLubyte*>(malloc(width_ * height_ * (bpp_ / 8) * sizeof(GLfloat)));
	memset(export_data_, 0, width_ * height_ * (bpp_ / 8));

	const double start = glfwGetTime();
	gl_pbo pbo{};
	pbo.gen(width_ * height_ * (bpp_ / 8) * sizeof(GLfloat));
	image.get_data_via_pbo(&pbo, export_data_);
	Console::log("export time: " + std::to_string(glfwGetTime() - start));

	renderer_ = std::thread(&HeronImage::render_image, this, file_loc);
}

void HeronImage::render_image(const char* file_loc)
{
	const double start = glfwGetTime();
	stbi_write_jpg(file_loc, width_, height_, (bpp_ / 8), export_data_, 100);
	free(export_data_);
	Console::log("stbi_write_jpg time: " + std::to_string(glfwGetTime() - start));
	exporting_ = false;
}
