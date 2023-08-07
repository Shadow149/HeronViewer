#include "HeronImage.h"

#include <iostream>

#include "cat_item.h"
#include "Console.h"
#include "FreeImage.h"
#include "Status.h"
#include "stb_image_write.h"
#include "Utils.h"
#include "GLFW/glfw3.h"

void HeronImage::load_image(cat_item item)
{
	file_location_ = std::string(item.file_location);
	image_loader_ = std::thread(&HeronImage::read_image, this, file_location_);
}

void HeronImage::finish_export()
{
	Console::log("Image rendered, join to main thread");
	Status::set_status("Image Exported!");
	renderer_.join();
}

void HeronImage::unload()
{
	if (img_data_) {
		free(img_data_);
		img_data_ = nullptr;
	}
	else if (!image_loaded_) { Console::log("No Image to unload"); return; }
	if (lr_img_data_) {
		free(lr_img_data_);
		lr_img_data_ = nullptr;
	}
	image_loaded_ = false;
	if (image_loader_.joinable()) image_loader_.join();
	Console::log("Unloading Image");
}

void HeronImage::read_image(const std::string& filename)
{
	finished_loading_ = false;
	loading_ = true;

	const char* file_c_str = filename.c_str();
	Console::log("Loading Image... %s", file_c_str);
	const double start = glfwGetTime();
	FIBITMAP* fi_bitmap = nullptr;

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(file_c_str, 0);

	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(file_c_str);
	if (fif == FIF_UNKNOWN)
	{
		Console::log("IMAGE LOAD ERROR - UNKNOWN FILE TYPE");
		return;
	}

	if (FreeImage_FIFSupportsReading(fif))
	{
		if (fif == FIF_RAW) {
			fi_bitmap = FreeImage_Load(fif, file_c_str, RAW_DEFAULT);
			fi_bitmap = FreeImage_TmoDrago03(fi_bitmap, 2.5);
			fi_bitmap = FreeImage_ConvertTo24Bits(fi_bitmap);
			FreeImage_AdjustGamma(fi_bitmap, 5);
		}
		else {
			fi_bitmap = FreeImage_Load(fif, file_c_str);
			fi_bitmap = FreeImage_ConvertTo24Bits(fi_bitmap);
			FreeImage_AdjustGamma(fi_bitmap, 2.5);
		}
	}

	if (!fi_bitmap)
	{
		Console::log("IMAGE LOAD ERROR - IMAGE FAILED TO LOAD");
		return;
	}

	const unsigned fi_bitmap_bpp = FreeImage_GetBPP(fi_bitmap);
	Console::log("Bitmap bpp: %d", fi_bitmap_bpp);
	FreeImage_FlipVertical(fi_bitmap);

	width_ = static_cast<GLsizei>(FreeImage_GetWidth(fi_bitmap));
	height_ = static_cast<GLsizei>(FreeImage_GetHeight(fi_bitmap));

	width_ = static_cast<GLsizei>(FreeImage_GetWidth(fi_bitmap));
	height_ = static_cast<GLsizei>(FreeImage_GetHeight(fi_bitmap));
	bpp_ = static_cast<GLsizei>(FreeImage_GetBPP(fi_bitmap));
	Console::log("Bitmap bpp: %d", bpp_);

	GLsizei small_img_height, small_img_width;
	resize_image(width_, height_, SMALL_IMG_MAX, small_img_width, small_img_height);
	width_ = small_img_width;
	height_ = small_img_height;
	Console::log("IMAGE DIMENSIONS SIZE: %d, %d", width_, height_);

	resize_image(width_, height_, LOW_RES_IMG_MAX, lr_width_, lr_height_);
	fi_bitmap = FreeImage_Rescale(fi_bitmap, width_, height_);
	FIBITMAP* lr_fi_bitmap = FreeImage_Rescale(fi_bitmap, lr_width_, lr_height_);

	dispatch_size_ = glm::ivec2((width_ / 32.0f) + 1, (height_ / 32.0f) + 1);
	Console::log("COMPUTE DISPATCH SIZE: %d, %d", dispatch_size_.x, dispatch_size_.y);

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
	Console::log("Image load time : %f", glfwGetTime() - start);
	loading_ = false;
	finished_loading_ = true;
	image_loaded_ = true;
}

void HeronImage::export_image(const char* file_loc, const gl_image& image, export_data export_data)
{
	exporting_ = true;
	Console::log("Exporting to: %s", file_loc);

	export_data_ = static_cast<GLubyte*>(malloc(width_ * height_ * (bpp_ / 8) * sizeof(GLfloat)));
	memset(export_data_, 0, width_ * height_ * (bpp_ / 8));

	const double start = glfwGetTime();
	gl_pbo pbo{};
	pbo.gen(width_ * height_ * (bpp_ / 8) * sizeof(GLfloat));
	image.get_data_via_pbo(&pbo, export_data_);
	Console::log("Export time: %f", glfwGetTime() - start);

	renderer_ = std::thread(&HeronImage::render_image, this, file_loc, export_data);
}

void HeronImage::render_image(const char* file_loc, const export_data export_data)
{
	const double start = glfwGetTime();
	const export_type type = export_data.type;
	switch(type)
	{
		case EXPORT_JPEG:
			stbi_write_jpg(file_loc, width_, height_, (bpp_ / 8), export_data_, export_data.quality);
			break;
		case EXPORT_PNG:
			stbi_write_png(file_loc, width_, height_, bpp_/8, export_data_, width_ * (bpp_ / 8));
			break;
	}
	free(export_data_);
	export_data_ = nullptr;
	Console::log("stbi time: %f", glfwGetTime() - start);
	exporting_ = false;
}
