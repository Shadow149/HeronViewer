#include "Widgets.h"


long double TAU = 6.2831855;

using namespace glm;

ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
	return { lhs.x + rhs.x, lhs.y + rhs.y };
}

ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
	return { lhs.x - rhs.x, lhs.y - rhs.y };
}

ImVec2 operator*(const ImVec2& lhs, const float& rhs)
{
	return { lhs.x * rhs, lhs.y * rhs };
}

ImVec2 operator/(const ImVec2& lhs, const float& rhs)
{
	return { lhs.x / rhs, lhs.y / rhs };
}

ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs)
{
	return { lhs.x / rhs.x, lhs.y / rhs.y };
}

bool hue_wheel(float thickness, int split, int width, ImVec2 pos, float angle, float alpha, bool skin_tone_line)
{
	ImVec2 curPos = ImGui::GetCursorScreenPos();

	float const height = width;

	float radius = width * 0.5f;

	const float dAngle = 2 * IM_PI / ((float)split);

	ImVec2 offset = curPos + ImVec2(radius, radius) + pos;

	ImVec2 const uv = ImGui::GetFontTexUvWhitePixel();
	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	pDrawList->PrimReserve(split * 6, split * 4);
	float angle_b4 = angle;
	for (int i = 0; i < split; ++i)
	{
		float x0 = radius * ImCos(angle);
		float y0 = radius * ImSin(angle);

		float x1 = radius * ImCos(angle + dAngle);
		float y1 = radius * ImSin(angle + dAngle);

		float x2 = (radius - thickness) * ImCos(angle + dAngle);
		float y2 = (radius - thickness) * ImSin(angle + dAngle);

		float x3 = (radius - thickness) * ImCos(angle);
		float y3 = (radius - thickness) * ImSin(angle);

		pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx));
		pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx + 1));
		pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx + 2));

		pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx));
		pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx + 2));
		pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx + 3));

		float r0, g0, b0;
		float r1, g1, b1;
		ImGui::ColorConvertHSVtoRGB(((float)i) / ((float)(split - 1)), 1.0f, 1.0f, r0, g0, b0);
		ImGui::ColorConvertHSVtoRGB(((float)((i + 1) % split)) / ((float)(split - 1)), 1.0f, 1.0f, r1, g1, b1);

		pDrawList->PrimWriteVtx(offset + ImVec2(x0, y0), uv, IM_COL32(r0 * 255, g0 * 255, b0 * 255, alpha));
		pDrawList->PrimWriteVtx(offset + ImVec2(x1, y1), uv, IM_COL32(r1 * 255, g1 * 255, b1 * 255, alpha));
		pDrawList->PrimWriteVtx(offset + ImVec2(x2, y2), uv, IM_COL32(255, 255, 255, alpha));
		pDrawList->PrimWriteVtx(offset + ImVec2(x3, y3), uv, IM_COL32(255, 255, 255, alpha));
		angle += dAngle;
	}
	//ImGui::PopID();

	if (skin_tone_line) {
		angle = angle_b4 + (15.0f / 360.0f) * 2 * IM_PI;

		float x1 = radius * ImCos(angle);
		float y1 = radius * ImSin(angle);
		pDrawList->AddLine(offset + ImVec2(0, 0), offset + ImVec2(x1, y1), IM_COL32_BLACK);
	}

	ImGui::GetWindowDrawList()->AddCircle(ImVec2(offset.x, offset.y), height / 2, ImColor(32, 32, 32), 0, 3);


	return false;
}

// color editor for 3 or 4 component colors
bool drawColorSelector(const char* label, float height, float* r, float* g, float* b, bool invert, float angle) {
	ImGui::PushID(label);

	if (invert) {
		*r = 1 - *r;
		*g = 1 - *g;
		*b = 1 - *b;
	}

	float topPadding = 20.0f;
	float botPadding = 20.0f;
	ImVec2 buttonStart = ImGui::GetCursorScreenPos();
	vec2 center = vec2(buttonStart.x, buttonStart.y + topPadding) + vec2(height, height) * 0.5f;

	ImVec2 textSize = ImGui::CalcTextSize(label);
	ImGui::RenderText(buttonStart + ImVec2(height / 2, textSize.y/2) - textSize/2, label);

	hue_wheel(height / 2, height, height, ImVec2(0, topPadding), angle);


	ImGui::SetCursorScreenPos(buttonStart);
	ImGui::InvisibleButton(label, ImVec2(height, height)); ImGui::SameLine();

	vec3 rgb = vec3(max(0.f, *r), max(0.f, *g), max(0.f, *b));
	vec3 hsv = vec3(0);
	ImGui::ColorConvertRGBtoHSV(rgb.r, rgb.g, rgb.b, hsv.r, hsv.g, hsv.b);

	float h = hsv.r;
	float s = hsv.g;
	float v = hsv.b;

	vec2 onCircle = vec2(cos(h * TAU), sin(h * TAU)) * s;

	vec2 pos = center + onCircle * height * 0.5f;

	ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(pos.x, pos.y), 4.0f, ImColor(0, 0, 0));
	ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(pos.x, pos.y), 3.0f, ImColor(255, 255, 255));

	bool changed = false;
	if (ImGui::IsItemActivated() && ImGui::IsMouseDoubleClicked(0))
	{
		*r = *g = *b = 1;
	}
	else if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
		float speed = 0.005f * exp(-3 * s);
		if (ImGui::GetIO().KeyShift) {
			speed = 0.001f;
		}
		onCircle = onCircle + vec2(ImGui::GetMouseDragDelta().x * speed, ImGui::GetMouseDragDelta().y * speed);
		ImGui::ResetMouseDragDelta();
		s = min(1.0f, length(onCircle));
		if (s == 0.0f) {
			h = 0.0f;
		}
		else {
			h = atan2f(onCircle.y, onCircle.x) / TAU;
			if (h < 0) {
				h += 1.0f;
			}
		}
		changed = true;
	}

	if (changed) {
		ImGui::ColorConvertHSVtoRGB(h, s, 1.0, *r, *g, *b);
	}

	if (invert) {
		*r = 1 - *r;
		*g = 1 - *g;
		*b = 1 - *b;
	}

	ImVec2 size = ImVec2(height/3, height + topPadding);
	ImGui::ItemSize(size);
	ImGui::ItemAdd(ImRect(buttonStart, buttonStart + size), ImGui::GetID(label));


	ImGui::PopID();
	return changed;
}