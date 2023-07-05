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
