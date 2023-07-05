#pragma once
#include <thread>

#include "gl_texture.h"
#include "glm.hpp"

enum
{
	SMALL_IMG_MAX = 4000,
	LOW_RES_IMG_MAX = 1000
};


class HeronImage
{
public:
	HeronImage() :
		loading_(false),
		finished_loading_(false),
		img_data_(nullptr),
		lr_img_data_(nullptr),
		height_(0),
		width_(0),
		lr_height_(0),
		lr_width_(0),
		dispatch_size_()
	{
	}

	void load_image(std::string& filename);
	void export_image(const char* file_loc, const gl_image& image);
	void finish_export();
	template<typename T>
	static void resize_image(GLsizei width, GLsizei height, GLsizei max_side,
		T& target_width, T& target_height);

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
	void render_image(const char* file_loc);

private:
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

template<typename T>
void HeronImage::resize_image(const GLsizei width, const GLsizei height,
	const GLsizei max_side, T& target_width, T& target_height)
{
	if (width > height && width > max_side)
	{
		target_width = max_side;
		target_height = static_cast<GLsizei>(max_side * (static_cast<double>(height) / width));
	}
	else if (height > max_side)
	{
		target_width = static_cast<GLsizei>(max_side * (static_cast<double>(width) / height));
		target_height = max_side;
	}
	else
	{
		target_width = width;
		target_height = height;
	}
}
