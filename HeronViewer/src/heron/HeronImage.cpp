#include "HeronImage.h"

#include <iostream>

#include "cat_item.h"
#include "Console.h"
// #include "FreeImage.h"
#include "Status.h"
#include "stb_image_write.h"
#include "stb_image.h"
#include "Utils.h"
#include "GLFW/glfw3.h"

#include "libraw/libraw.h"
#include "avir.h"

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

	// TODO all hard coded
	const unsigned fi_bitmap_bpp = 24;
	bpp_ = 24;

	avir::CImageResizer<>ImageResizer( 8 );
	const char* file_c_str = filename.c_str();
	Console::log("Loading Image... %s", file_c_str);
	const double start = glfwGetTime();

	LibRaw lr = LibRaw();
	if (lr.open_file(file_c_str) < 0) {
		// TODO could not be a valid file...
		// Not a raw file, use stb to open...
		int width, height;
		unsigned char* image_data = stbi_load(file_c_str, &width, &height, NULL, 3);
		if (image_data == NULL) {
			Console::log("Failed to load image...");
			return;
		}

		resize_image(width, height, SMALL_IMG_MAX, width_, height_);
		dispatch_size_ = glm::ivec2((width_ / 32.0f) + 1, (height_ / 32.0f) + 1);
		img_data_ = static_cast<unsigned char*>(malloc(width_ * height_ * bpp_ * sizeof(ushort)));
		ImageResizer.resizeImage(image_data, width, height, 0, img_data_, width_, height_, 3, 0 );
		memcpy((void*)img_data_, image_data, width_ * height_ * (bpp_/8));

		resize_image(width, height, LOW_RES_IMG_MAX, lr_width_, lr_height_);
		lr_img_data_ = static_cast<unsigned char*>(malloc(lr_width_ * lr_height_ * bpp_ * sizeof(ushort)));
		ImageResizer.resizeImage(image_data, width, height, 0, lr_img_data_, lr_width_, lr_height_, 3, 0 );
		stbi_image_free(image_data);
	} else {

		lr.imgdata.params.use_camera_wb = 1;

		// WARNING: We are working with half sized images, for performance reasons only
		// TODO: complete a render pass on export for the full image
		lr.imgdata.params.half_size = 1;
		// Defaults are for rec709, these are for sRGB: gamm[0]=1/2.4 and gamm[1]=12.92
		// These look better....:
		lr.imgdata.params.gamm[0] = 1/2.4;
		lr.imgdata.params.gamm[1] = 12.92;
		// lr.imgdata.params.half_size = 1;

		// lr.imgdata.params.no_interpolation=1 disables demosaic
		// lr.imgdata.params.no_auto_scale=1 disables scaling from camera maximum to 64k
		// lr.imgdata.params.no_auto_bright=1; //disables auto brighten

		// Let us unpack the image
		lr.unpack();
		lr.adjust_sizes_info_only();
		lr.dcraw_process();

		libraw_processed_image_t* p_img = lr.dcraw_make_mem_image();

		if (lr.imgdata.sizes.flip == 5) {
			GLsizei temp;
			temp = width_;
			width_ = height_;
			height_ = temp;
		}

		// lr_width_ = static_cast<GLsizei>(lr.imgdata.sizes.iwidth);
		// lr_height_ = static_cast<GLsizei>(lr.imgdata.sizes.iheight);
		Console::log("Bitmap bpp: %d", bpp_);


		Console::log("memcpy image bits");
		resize_image(lr.imgdata.sizes.iwidth, lr.imgdata.sizes.iheight, SMALL_IMG_MAX, width_, height_);
		dispatch_size_ = glm::ivec2((width_ / 32.0f) + 1, (height_ / 32.0f) + 1);
		Console::log("COMPUTE DISPATCH SIZE: %d, %d", dispatch_size_.x, dispatch_size_.y);

		img_data_ = static_cast<unsigned char*>(malloc(width_ * height_ * bpp_ * sizeof(ushort)));
		ImageResizer.resizeImage(p_img->data, lr.imgdata.sizes.iwidth, lr.imgdata.sizes.iheight, 0, 
								 img_data_, width_, height_, 3, 0 );
		// lr.copy_mem_image((void*)img_data_, width_ * 3, 0);

		resize_image(lr.imgdata.sizes.iwidth, lr.imgdata.sizes.iheight, LOW_RES_IMG_MAX, lr_width_, lr_height_);
		lr_img_data_ = static_cast<unsigned char*>(malloc(lr_width_ * lr_height_ * bpp_ * sizeof(ushort)));
		ImageResizer.resizeImage(p_img->data, lr.imgdata.sizes.iwidth, lr.imgdata.sizes.iheight, 0, 
								 lr_img_data_, lr_width_, lr_height_, 3, 0 );

		// lr.copy_mem_image((void*)lr_img_data_, lr_width_ * 3, 1);
		Console::log("memcpy finished");

		// lr.dcraw_clear_mem();
		lr.recycle();
	}

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
