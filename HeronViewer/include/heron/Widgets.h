#pragma once
#include <imgui_internal.h>
#include <ostream>
#include <string>
#include <glm/common.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/quaternion_geometric.hpp>
bool drawColorSelector(const char* label, float height, float* r, float* g, float* b, bool invert = true, float angle = IM_PI);
bool hue_wheel(float thickness, int split, int width, ImVec2 pos, float angle = IM_PI, float alpha = 255, bool skin_tone_line = false);