#pragma once
#include <string>

float clamp(float v, float l, float h);
void add_mat(const float* first, const float* second, float* result, int size);
void sub_mat(const float* first, const float* second, float* result, int size);
void scalar_mul(float c, const float* ker, float* result, int size);
void normal_mat(const float* ker, float* result, int size);
std::string strip_extension(std::string s);