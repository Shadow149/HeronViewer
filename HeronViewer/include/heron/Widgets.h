#pragma once
#include <imgui_internal.h>
#include <ostream>
#include <string>
#include <glm/common.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/quaternion_geometric.hpp>

ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs);

ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs);

ImVec2 operator*(const ImVec2& lhs, const float& rhs);

ImVec2 operator/(const ImVec2& lhs, const float& rhs);

ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs);


bool draw_color_selector(const char* label, float height, float* r, float* g, float* b, bool invert = true, float angle = IM_PI);
bool draw_grid_selector(const char* label, const float width, float* x, float* y, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
bool hue_wheel(float thickness, int split, int width, ImVec2 pos, float angle = IM_PI, float alpha = 255, bool skin_tone_line = false);
bool hue_grid(const int width, const ImVec2 pos, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
bool spinner(const char* label, float radius, float thickness, const ImU32& color);