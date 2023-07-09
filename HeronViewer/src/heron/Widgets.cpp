#include "Widgets.h"


long double TAU = 6.2831855;

using namespace glm;

ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
	return {lhs.x + rhs.x, lhs.y + rhs.y};
}

ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
	return {lhs.x - rhs.x, lhs.y - rhs.y};
}

ImVec2 operator*(const ImVec2& lhs, const float& rhs)
{
	return {lhs.x * rhs, lhs.y * rhs};
}

ImVec2 operator/(const ImVec2& lhs, const float& rhs)
{
	return {lhs.x / rhs, lhs.y / rhs};
}

ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs)
{
	return {lhs.x / rhs.x, lhs.y / rhs.y};
}

bool spinner(const char* label, const float radius, const float thickness, const ImU32& color)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	const ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	// Render
	window->DrawList->PathClear();

	constexpr double num_segments = 30.0;
	const double start = abs(ImSin(g.Time * 1.8) * (num_segments - 5));

	const double a_min = IM_PI * 2.0 * start / num_segments;
	constexpr double a_max = IM_PI * 2.0 * (num_segments - 3) / num_segments;

	const auto centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

	for (int i = 0; i < num_segments; i++)
	{
		const double a = a_min + (i / num_segments) * (a_max - a_min);
		window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
		                                    centre.y + ImSin(a + g.Time * 8) * radius));
	}

	window->DrawList->PathStroke(color, false, thickness);
	return true;
}

bool hue_wheel(const float thickness, const int split, const int width, const ImVec2 pos, float angle, const float alpha, const bool skin_tone_line)
{
	const ImVec2 curPos = ImGui::GetCursorScreenPos();

	const float height = width;

	const float radius = width * 0.5f;

	const double d_angle = 2 * IM_PI / static_cast<float>(split);

	const ImVec2 offset = curPos + ImVec2(radius, radius) + pos;

	const ImVec2 uv = ImGui::GetFontTexUvWhitePixel();
	ImDrawList* p_draw_list = ImGui::GetWindowDrawList();
	p_draw_list->PrimReserve(split * 6, split * 4);
	const float angle_b4 = angle;
	for (int i = 0; i < split; ++i)
	{
		const float x0 = radius * ImCos(angle);
		const float y0 = radius * ImSin(angle);

		const float x1 = radius * ImCos(angle + d_angle);
		const float y1 = radius * ImSin(angle + d_angle);

		const float x2 = (radius - thickness) * ImCos(angle + d_angle);
		const float y2 = (radius - thickness) * ImSin(angle + d_angle);

		const float x3 = (radius - thickness) * ImCos(angle);
		const float y3 = (radius - thickness) * ImSin(angle);

		p_draw_list->PrimWriteIdx(static_cast<ImDrawIdx>(p_draw_list->_VtxCurrentIdx));
		p_draw_list->PrimWriteIdx(static_cast<ImDrawIdx>(p_draw_list->_VtxCurrentIdx + 1));
		p_draw_list->PrimWriteIdx(static_cast<ImDrawIdx>(p_draw_list->_VtxCurrentIdx + 2));

		p_draw_list->PrimWriteIdx(static_cast<ImDrawIdx>(p_draw_list->_VtxCurrentIdx));
		p_draw_list->PrimWriteIdx(static_cast<ImDrawIdx>(p_draw_list->_VtxCurrentIdx + 2));
		p_draw_list->PrimWriteIdx(static_cast<ImDrawIdx>(p_draw_list->_VtxCurrentIdx + 3));

		float r0, g0, b0;
		float r1, g1, b1;
		ImGui::ColorConvertHSVtoRGB(static_cast<float>(i) / static_cast<float>(split - 1), 1.0f, 1.0f, r0, g0, b0);
		ImGui::ColorConvertHSVtoRGB(static_cast<float>((i + 1) % split) / static_cast<float>(split - 1), 1.0f, 1.0f, r1,
		                            g1, b1);

		p_draw_list->PrimWriteVtx(offset + ImVec2(x0, y0), uv, IM_COL32(r0 * 255, g0 * 255, b0 * 255, alpha));
		p_draw_list->PrimWriteVtx(offset + ImVec2(x1, y1), uv, IM_COL32(r1 * 255, g1 * 255, b1 * 255, alpha));
		p_draw_list->PrimWriteVtx(offset + ImVec2(x2, y2), uv, IM_COL32(255, 255, 255, alpha));
		p_draw_list->PrimWriteVtx(offset + ImVec2(x3, y3), uv, IM_COL32(255, 255, 255, alpha));
		angle += d_angle;
	}

	if (skin_tone_line)
	{
		angle = angle_b4 + (15.0f / 360.0f) * 2 * IM_PI;

		const float x1 = radius * ImCos(angle);
		const float y1 = radius * ImSin(angle);
		p_draw_list->AddLine(offset + ImVec2(0, 0), offset + ImVec2(x1, y1), IM_COL32_BLACK);
	}

	ImGui::GetWindowDrawList()->AddCircle(ImVec2(offset.x, offset.y), height / 2, ImColor(32, 32, 32), 0, 3);


	return false;
}

// color editor for 3 or 4 component colors
bool draw_color_selector(const char* label, const float height, float* r, float* g, float* b, const bool invert, const float angle)
{
	ImGui::PushID(label);

	if (invert)
	{
		*r = 1 - *r;
		*g = 1 - *g;
		*b = 1 - *b;
	}

	const float topPadding = 20.0f;
	float botPadding = 20.0f;
	const ImVec2 buttonStart = ImGui::GetCursorScreenPos();
	const vec2 center = vec2(buttonStart.x, buttonStart.y + topPadding) + vec2(height, height) * 0.5f;

	const ImVec2 textSize = ImGui::CalcTextSize(label);
	ImGui::RenderText(buttonStart + ImVec2(height / 2, textSize.y / 2) - textSize / 2, label);

	hue_wheel(height / 2, height, height, ImVec2(0, topPadding), angle);


	ImGui::SetCursorScreenPos(buttonStart);
	ImGui::InvisibleButton(label, ImVec2(height, height));
	ImGui::SameLine();

	const auto rgb = vec3(max(0.f, *r), max(0.f, *g), max(0.f, *b));
	auto hsv = vec3(0);
	ImGui::ColorConvertRGBtoHSV(rgb.r, rgb.g, rgb.b, hsv.r, hsv.g, hsv.b);

	float h = hsv.r;
	float s = hsv.g;
	float v = hsv.b;

	vec2 onCircle = vec2(cos(h * TAU), sin(h * TAU)) * s;

	const vec2 pos = center + onCircle * height * 0.5f;

	ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(pos.x, pos.y), 4.0f, ImColor(0, 0, 0));
	ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(pos.x, pos.y), 3.0f, ImColor(255, 255, 255));

	bool changed = false;
	bool reset = false;
	if (ImGui::IsItemActivated() && ImGui::IsMouseDoubleClicked(0))
	{
		*r = *g = *b = 1;
		reset = true;
	}
	else if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
	{
		float speed = 0.005f * exp(-3 * s);
		if (ImGui::GetIO().KeyShift)
		{
			speed = 0.001f;
		}
		onCircle = onCircle + vec2(ImGui::GetMouseDragDelta().x * speed, ImGui::GetMouseDragDelta().y * speed);
		ImGui::ResetMouseDragDelta();
		s = min(1.0f, length(onCircle));
		if (s == 0.0f)
		{
			h = 0.0f;
		}
		else
		{
			h = atan2f(onCircle.y, onCircle.x) / TAU;
			if (h < 0)
			{
				h += 1.0f;
			}
		}
		changed = true;
	}

	if (changed)
	{
		ImGui::ColorConvertHSVtoRGB(h, s, 1.0, *r, *g, *b);
	}

	if (invert)
	{
		*r = 1 - *r;
		*g = 1 - *g;
		*b = 1 - *b;
	}

	const auto size = ImVec2(height / 3, height + topPadding);
	ImGui::ItemSize(size);
	ImGui::ItemAdd(ImRect(buttonStart, buttonStart + size), ImGui::GetID(label));


	ImGui::PopID();
	return changed | reset;
}
