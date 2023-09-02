#pragma once
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <ostream>
#include <string>
#include <glm/common.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/quaternion_geometric.hpp>

bool draw_color_selector(const char* label, float height, float* r, float* g, float* b, bool invert = true, float angle = IM_PI);
bool draw_grid_selector(const char* label, const float width, float* x, float* y, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
bool hue_wheel(float thickness, int split, int width, ImVec2 pos, float angle = IM_PI, float alpha = 255, bool skin_tone_line = false);
bool hue_grid(const int width, const ImVec2 pos, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
bool spinner(const char* label, float radius, float thickness, const ImU32& color);


// [src] https://github.com/ocornut/imgui/issues/123
// [src] https://github.com/ocornut/imgui/issues/55

// v1.23 - selection index track, value range, context menu, improve manipulation controls (D.Click to add/delete, drag to add)
// v1.22 - flip button; cosmetic fixes
// v1.21 - oops :)
// v1.20 - add iq's interpolation code
// v1.10 - easing and colors
// v1.00 - jari komppa's original
static const float CurveTerminator = -10000;
int CurveEditor(const char* label, const ImVec2& size, const int maxpoints, ImVec2* points, int* selection, const ImVec2& rangeMin = ImVec2(0, 0), const ImVec2& rangeMax = ImVec2(1, 1));
float CurveValueSmooth(float p, int maxpoints, const ImVec2* points);