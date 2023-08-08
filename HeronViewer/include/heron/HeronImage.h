#pragma once
#include <thread>

#include "cat_item.h"
#include "gl_texture.h"
#include "glm.hpp"

enum
{
	SMALL_IMG_MAX = 4000,
	LOW_RES_IMG_MAX = 1000
};

typedef enum
{
	EXPORT_PNG,
	EXPORT_JPEG,
} export_type;


struct export_data
{
	export_type type;
	int quality;
};

class HeronImage
{
public:
	HeronImage() :
		loading_(false),
		finished_loading_(false),
		exporting_(false),
		image_loaded_(false),
		img_data_(nullptr),
		lr_img_data_(nullptr),
		height_(0),
		width_(0),
		lr_height_(0),
		lr_width_(0),
		dispatch_size_()
	{
	}

	void load_image(cat_item item);
	void export_image(const char* file_loc, const gl_image& image, export_data export_data);
	void finish_export();

	glm::ivec2 get_dispatch_size() const { return dispatch_size_; }
	bool is_loading() const { return loading_; }
	bool is_exporting() const { return exporting_; }
	bool is_loaded() const { return image_loaded_; }
	void unload();

	bool finished_loading()
	{
		if (finished_loading_)
		{
			finished_loading_ = false;
			return true;
		}
		return false;
	}
	bool finished_exporting() const
	{
		return renderer_.joinable() && !exporting_;
	}

	unsigned char* get_img_data() const { return img_data_; }
	unsigned char* get_lr_img_data() const { return lr_img_data_; }

	GLsizei get_width() const { return width_; }
	GLsizei get_height() const { return height_; }
	GLsizei get_lr_width() const { return lr_width_; }
	GLsizei get_lr_height() const { return lr_height_; }

	GLsizei get_size() const { return width_ * height_; }

private:
	void read_image(const std::string& filename);
	void render_image(const char* file_loc, const export_data export_data);

private:
	std::string file_location_;

	bool loading_;
	bool finished_loading_;
	bool exporting_;
	bool image_loaded_;

	std::thread image_loader_;
	std::thread renderer_;

	GLubyte* export_data_;
	unsigned char* img_data_;
	unsigned char* lr_img_data_;

	GLsizei bpp_{};
	GLsizei height_, width_;
	GLsizei lr_height_, lr_width_;

	glm::ivec2 dispatch_size_;
};