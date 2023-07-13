#include "Utils.h"

float clamp(float v, const float l, const float h)
{
	if (v > h)
		v = h;
	if (v < l)
		v = l;
	return v;
}

void add_mat(const float* first, const float* second, float* result, const int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			result[(size * i) + j] = first[(size * i) + j] + second[(size * i) + j];
		}
	}
}

void sub_mat(const float* first, const float* second, float* result, const int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			result[(size * i) + j] = first[(size * i) + j] - second[(size * i) + j];
		}
	}
}

void scalar_mul(const float c, const float* ker, float* result, const int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			result[(size * i) + j] = ker[(size * i) + j] * c;
		}
	}
}

void normal_mat(const float* ker, float* result, const int size) {
	float sum = 0;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			sum += ker[(size * i) + j];
		}
	}
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			result[(size * i) + j] = ker[(size * i) + j] / sum;
		}
	}
}

std::string strip_extension(std::string s)
{
	const std::size_t p = s.find('.');
	if (p == std::string::npos)
		return "";
	std::string out = s.erase(p, s.length()-1);
	return out;
}


ImVec2 get_resize_size(const GLsizei width, const GLsizei height)
{
	ImVec2 preview_size;
	if (width > height && width > ImGui::GetWindowWidth())
	{
		preview_size.x = ImGui::GetWindowWidth();
		preview_size.y = static_cast<GLsizei>(ImGui::GetWindowWidth() * (static_cast<double>(height) / width));
	}
	else if (height > ImGui::GetWindowHeight())
	{
		const float win_height = ImGui::GetWindowHeight() - 30;
		preview_size.x = static_cast<GLsizei>(win_height * (static_cast<double>(width) / height));
		preview_size.y = win_height;
	}
	else
	{
		preview_size.x = width;
		preview_size.y = height;
	}
	return preview_size;
}



