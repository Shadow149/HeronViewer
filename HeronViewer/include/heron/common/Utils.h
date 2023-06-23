#pragma once
#include <string>

float clamp(float v, float l, float h);
void addMat(float* first, float* second, float* result, int size);
void subMat(float* first, float* second, float* result, int size);
void scalarMul(float c, float* ker, float* result, int size);
void normalMat(float* ker, float* result, int size);
std::string stripExtension(std::string s);