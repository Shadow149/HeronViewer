#include "Utils.h"

float clamp(float v, float l, float h)
{
	if (v > h)
		v = h;
	if (v < l)
		v = l;
	return v;
}

void addMat(float* first, float* second, float* result, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			result[(size * i) + j] = first[(size * i) + j] + second[(size * i) + j];
		}
	}
}

void subMat(float* first, float* second, float* result, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			result[(size * i) + j] = first[(size * i) + j] - second[(size * i) + j];
		}
	}
}

void scalarMul(float c, float* ker, float* result, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			result[(size * i) + j] = ker[(size * i) + j] * c;
		}
	}
}

void normalMat(float* ker, float* result, int size) {
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

std::string stripExtension(std::string s)
{
	std::size_t p = s.find('.');
	if (p == std::string::npos)
		return "";
	std::string out = s.erase(p, s.length()-1);
	return out;
}
