#pragma once
#include <string>

#include "imgui.h"
#include "glad/glad.h"

float clamp(float v, float l, float h);
void add_mat(const float* first, const float* second, float* result, int size);
void sub_mat(const float* first, const float* second, float* result, int size);
void scalar_mul(float c, const float* ker, float* result, int size);
void normal_mat(const float* ker, float* result, int size);
std::string strip_extension(std::string s);
ImVec2 get_resize_size(GLsizei width, GLsizei height);
template<typename T>
void resize_image(GLsizei width, GLsizei height,
                  GLsizei max_side, T& target_width, T& target_height);
template<typename T>
T min(T a, T b);


template<typename T>
T min(T a, T b) {
	if (a < b) return a;
	return b;
}

template<typename T>
void resize_image(const GLsizei width, const GLsizei height,
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