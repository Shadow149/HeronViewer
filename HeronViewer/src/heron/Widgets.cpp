#include "Widgets.h"

long double TAU = 6.2831855;

using namespace glm;

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

bool hue_grid(const int width, const ImVec2 pos, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left)
{
	const ImVec2 curPos = ImGui::GetCursorScreenPos();

	ImDrawList* p_draw_list = ImGui::GetWindowDrawList();
	const ImVec2 offset = curPos + ImVec2(width, width) + pos;

	p_draw_list->AddRectFilledMultiColor(curPos + pos, ImVec2(offset.x, offset.y), col_upr_left, col_upr_right, col_bot_right, col_bot_left);

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
	//hue_grid(height, ImVec2(0, topPadding), ImColor(255, 0, 0), ImColor(0, 255, 0), ImColor(0, 255, 0), ImColor(0, 0, 0));

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

// Will return x, y values between -1 and 1
bool draw_grid_selector(const char* label, const float width, float* x, float* y, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left){
	ImGui::PushID(label);

	const float topPadding = 20.0f;
	float botPadding = 20.0f;
	const ImVec2 buttonStart = ImGui::GetCursorScreenPos() + ImVec2(ImGui::GetWindowWidth() / 4, 0);
	const vec2 center = vec2(buttonStart.x, buttonStart.y + topPadding) + vec2(width, width) * 0.5f;

	const ImVec2 textSize = ImGui::CalcTextSize(label);
	ImGui::RenderText(buttonStart + ImVec2(width / 2, textSize.y / 2) - textSize / 2, label);

	hue_grid(width, ImVec2(ImGui::GetWindowWidth() / 4, topPadding), col_upr_left, col_upr_right, col_bot_right, col_bot_left);

	ImGui::SetCursorScreenPos(buttonStart);
	ImGui::InvisibleButton(label, ImVec2(width, width));
	ImGui::SameLine();

	vec2 onCircle = vec2(*x, *y);

	const vec2 pos = center + onCircle * width * 0.5f;

	ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(pos.x, pos.y), 4.0f, ImColor(0, 0, 0));
	ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(pos.x, pos.y), 3.0f, ImColor(255, 255, 255));

	bool changed = false;
	bool reset = false;
	if (ImGui::IsItemActivated() && ImGui::IsMouseDoubleClicked(0))
	{
		*x = *y = 0;
		reset = true;
	}
	else if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
	{
		float speed = 0.005f * exp(-2 * sqrt(*x**x + *y**y));
		if (ImGui::GetIO().KeyShift)
		{
			speed = 0.001f;
		}
		onCircle = onCircle + vec2(ImGui::GetMouseDragDelta().x * speed, ImGui::GetMouseDragDelta().y * speed);
		ImGui::ResetMouseDragDelta();
		*x = clamp(onCircle.x, -1.0f, 1.0f);
		*y = clamp(onCircle.y, -1.0f, 1.0f);
		changed = true;
	}

	const auto size = ImVec2(width / 3, width + topPadding);
	ImGui::ItemSize(size);
	ImGui::ItemAdd(ImRect(buttonStart, buttonStart + size), ImGui::GetID(label));


	ImGui::PopID();
	return changed | reset;
}



// [src] http://iquilezles.org/www/articles/minispline/minispline.htm
// key format (for dim == 1) is (t0,x0,t1,x1 ...)
// key format (for dim == 2) is (t0,x0,y0,t1,x1,y1 ...)
// key format (for dim == 3) is (t0,x0,y0,z0,t1,x1,y1,z1 ...)
template<int DIM>
void spline(const float* key, int num, float t, float* v)
{
    static float coefs[16] = {
        -1.0f, 2.0f,-1.0f, 0.0f,
         3.0f,-5.0f, 0.0f, 2.0f,
        -3.0f, 4.0f, 1.0f, 0.0f,
         1.0f,-1.0f, 0.0f, 0.0f
    };

    const int size = DIM + 1;

    // find key
    int k = 0;
    while (key[k * size] < t)
        k++;

    const float key0 = key[(k - 1) * size];
    const float key1 = key[k * size];

    // interpolant
    const float h = (t - key0) / (key1 - key0);

    // init result
    for (int i = 0; i < DIM; i++)
        v[i] = 0.0f;

    // add basis functions
    for (int i = 0; i < 4; ++i)
    {
        const float* co = &coefs[4 * i];
        const float b = 0.5f * (((co[0] * h + co[1]) * h + co[2]) * h + co[3]);

        const int kn = ImClamp(k + i - 2, 0, num - 1);
        for (int j = 0; j < DIM; j++)
            v[j] += b * key[kn * size + j + 1];
    }
}

float CurveValueSmooth(float p, int maxpoints, const ImVec2* points)
{
    if (maxpoints < 2 || points == 0)
        return 0;
    if (p < 0)
        return points[0].y;

    float* input = new float[maxpoints * 2];
    float output[4];

    for (int i = 0; i < maxpoints; ++i)
    {
        input[i * 2 + 0] = points[i].x;
        input[i * 2 + 1] = points[i].y;
    }

    spline<1>(input, maxpoints, p, output);

    delete[] input;
    return output[0];
}


static inline float ImRemap(float v, float a, float b, float c, float d)
{
    return (c + (d - c) * (v - a) / (b - a));
}

static inline ImVec2 ImRemap(const ImVec2& v, const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d)
{
    return ImVec2(ImRemap(v.x, a.x, b.x, c.x, d.x), ImRemap(v.y, a.y, b.y, c.y, d.y));
}

int CurveEditor(const char* label, const ImVec2& size, const int maxpoints, ImVec2* points, int* selection, const ImVec2& rangeMin, const ImVec2& rangeMax)
{
    int modified = 0;
    int i;
    if (maxpoints < 2 || points == nullptr)
        return 0;

    if (points[0].x <= CurveTerminator)
    {
        points[0] = ImVec2(rangeMin.x, rangeMax.y/2.0f);
        points[1] = ImVec2(rangeMax.x, rangeMax.y/2.0f);
        points[2].x = CurveTerminator;
    }

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGuiContext& g = *GImGui;

    const ImGuiID id = window->GetID(label);
    if (window->SkipItems)
        return 0;

    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, NULL))
        return 0;

    ImGui::PushID(label);

    int currentSelection = selection ? *selection : -1;

    const bool hovered = ImGui::ItemHoverable(bb, id);

    int pointCount = 0;
    while (pointCount < maxpoints && points[pointCount].x >= rangeMin.x)
        pointCount++;

    const ImGuiStyle& style = g.Style;
    ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg, 1), true, style.FrameRounding);

    const float ht = bb.Max.y - bb.Min.y;
    const float wd = bb.Max.x - bb.Min.x;

    int hoveredPoint = -1;

    const float pointRadiusInPixels = 5.0f;

    // Handle point selection
    if (hovered)
    {
        ImVec2 hoverPos = (g.IO.MousePos - bb.Min) / (bb.Max - bb.Min);
        hoverPos.y = 1.0f - hoverPos.y;

        ImVec2 pos = ImRemap(hoverPos, ImVec2(0, 0), ImVec2(1, 1), rangeMin, rangeMax);

        int left = 0;
        while (left < pointCount && points[left].x < pos.x)
            left++;
        if (left)
            left--;

        const ImVec2 hoverPosScreen = ImRemap(hoverPos, ImVec2(0, 0), ImVec2(1, 1), bb.Min, bb.Max);
        const ImVec2 p1s = ImRemap(points[left], rangeMin, rangeMax, bb.Min, bb.Max);
        const ImVec2 p2s = ImRemap(points[left + 1], rangeMin, rangeMax, bb.Min, bb.Max);

        const float p1d = ImSqrt(ImLengthSqr(p1s - hoverPosScreen));
        const float p2d = ImSqrt(ImLengthSqr(p2s - hoverPosScreen));

        if (p1d < pointRadiusInPixels)
            hoveredPoint = left;

        if (p2d < pointRadiusInPixels)
            hoveredPoint = left + 1;

        if (g.IO.MouseDown[0])
        {
            if (currentSelection == -1)
                currentSelection = hoveredPoint;
        }
        else
            currentSelection = -1;

        enum
        {
            action_none,
            action_add_point,
            action_delete_point
        };

        int action = action_none;

        if (currentSelection == -1)
        {
            if (g.IO.MouseDoubleClicked[0] || ImGui::IsMouseDragging(0))
                action = action_add_point;
        }
        else if(g.IO.MouseDoubleClicked[0])
            action = action_delete_point;

        if (action == action_add_point)
        {
            if (pointCount < maxpoints)
            {
                // select
                currentSelection = left + 1;

                ++pointCount;
                for (i = pointCount; i > left; --i)
                    points[i] = points[i - 1];

                points[left + 1] = pos;

                if (pointCount < maxpoints)
                    points[pointCount].x = CurveTerminator;
            }
        }
        else if (action == action_delete_point)
        {
            // delete point
            if (currentSelection > 0 && currentSelection < maxpoints - 1)
            {
                for (i = currentSelection; i < maxpoints - 1; ++i)
                    points[i] = points[i + 1];

                --pointCount;
                points[pointCount].x = CurveTerminator;
                currentSelection = -1;
            }
        }
    }

    // handle point dragging
    const bool draggingPoint = ImGui::IsMouseDragging(0) && currentSelection != -1;

    if (draggingPoint)
    {
        if (selection)
            ImGui::SetActiveID(id, window);

        ImGui::SetFocusID(id, window);
        ImGui::FocusWindow(window);

        modified = 1;

        ImVec2 pos = (g.IO.MousePos - bb.Min) / (bb.Max - bb.Min);

        // constrain Y to min/max
        pos.y = 1.0f - pos.y;
        pos = ImRemap(pos, ImVec2(0, 0), ImVec2(1, 1), rangeMin, rangeMax);

        // constrain X to the min left/ max right
        const float pointXRangeMin = (currentSelection > 0) ? points[currentSelection - 1].x : rangeMin.x;
        const float pointXRangeMax = (currentSelection + 1 < pointCount) ? points[currentSelection + 1].x : rangeMax.x;

        pos = ImClamp(pos, ImVec2(pointXRangeMin, rangeMin.y), ImVec2(pointXRangeMax, rangeMax.y));

        points[currentSelection] = pos;

        // snap X first/last to min/max
        if (points[0].x < points[pointCount - 1].x)
        {
            points[0].x = rangeMin.y;
            points[pointCount - 1].x = rangeMax.x;
        }
        else
        {
            points[0].x = rangeMax.x;
            points[pointCount - 1].x = rangeMin.y;
        }
    }

    if (!ImGui::IsMouseDragging(0) && ImGui::GetActiveID() == id && selection && *selection != -1 && currentSelection == -1)
    {
        ImGui::ClearActiveID();
    }

    const ImU32 gridColor1 = ImGui::GetColorU32(ImGuiCol_TextDisabled, 0.5f);
    const ImU32 gridColor2 = ImGui::GetColorU32(ImGuiCol_TextDisabled, 0.25f);

    ImDrawList* drawList = window->DrawList;

    for (i = 0; i < 10; i ++) {
        float r0, g0, b0;
		float r1, g1, b1;
        float r2, g2, b2;
		float r3, g3, b3;
        ImGui::ColorConvertHSVtoRGB((i/10.0f), .75f, .5f, r0, g0, b0); // top left
		ImGui::ColorConvertHSVtoRGB((i/10.0f), .75f, .5f, r1, g1, b1); // bot left
        ImGui::ColorConvertHSVtoRGB(((i+1)/10.0f), .75f, .5f, r2, g2, b2); // top right
		ImGui::ColorConvertHSVtoRGB(((i+1)/10.0f), .75f, .5f, r3, g3, b3); // bot right

        drawList->AddRectFilledMultiColor(ImVec2(bb.Min.x + (wd / 10) * (i), bb.Min.y), ImVec2(bb.Min.x + (wd / 10) * (i+1), bb.Max.y),
            ImColor(r0, g0, b0), ImColor(r2, g2, b2), ImColor(r3, g3, b3), ImColor(r1, g1, b1));
    }
    // bg grid
    drawList->AddLine(ImVec2(bb.Min.x, bb.Min.y + ht / 2), ImVec2(bb.Max.x, bb.Min.y + ht / 2), gridColor1, 3);

    drawList->AddLine(ImVec2(bb.Min.x, bb.Min.y + ht / 4), ImVec2(bb.Max.x, bb.Min.y + ht / 4), gridColor1);

    drawList->AddLine(ImVec2(bb.Min.x, bb.Min.y + ht / 4 * 3), ImVec2(bb.Max.x, bb.Min.y + ht / 4 * 3), gridColor1);

    for (i = 0; i < 9; i++)
    {
        drawList->AddLine(ImVec2(bb.Min.x + (wd / 10) * (i + 1), bb.Min.y), ImVec2(bb.Min.x + (wd / 10) * (i + 1), bb.Max.y), gridColor2);
    }

    // drawList->PushClipRect(bb.Min, bb.Max);

    // smooth curve
    enum
    {
        smoothness = 256
    }; // the higher the smoother
    for (i = 0; i <= (smoothness - 1); ++i)
    {
        float px = (i + 0) / float(smoothness);
        float qx = (i + 1) / float(smoothness);

        px = ImRemap(px, 0, 1, rangeMin.x, rangeMax.x);
        qx = ImRemap(qx, 0, 1, rangeMin.x, rangeMax.x);

        const float py = CurveValueSmooth(px, maxpoints, points);
        const float qy = CurveValueSmooth(qx, maxpoints, points);

        ImVec2 p = ImRemap(ImVec2(px, py), rangeMin, rangeMax, ImVec2(0,0), ImVec2(1,1));
        ImVec2 q = ImRemap(ImVec2(qx, qy), rangeMin, rangeMax, ImVec2(0,0), ImVec2(1,1));
        p.y = 1.0f - p.y;
        q.y = 1.0f - q.y;

        p = ImRemap(p, ImVec2(0,0), ImVec2(1,1), bb.Min, bb.Max);
        q = ImRemap(q, ImVec2(0,0), ImVec2(1,1), bb.Min, bb.Max);

        drawList->AddLine(p, q, ImGui::GetColorU32(ImGuiCol_PlotHistogram), 2.0f);
    }

    // // lines
    // for (i = 1; i < pointCount; i++)
    // {
    //     ImVec2 a = ImRemap(points[i - 1], rangeMin, rangeMax, ImVec2(0, 0), ImVec2(1, 1));
    //     ImVec2 b = ImRemap(points[i], rangeMin, rangeMax, ImVec2(0, 0), ImVec2(1, 1));

    //     a.y = 1.0f - a.y;
    //     b.y = 1.0f - b.y;

    //     a = ImRemap(a, ImVec2(0,0), ImVec2(1,1), bb.Min, bb.Max);
    //     b = ImRemap(b, ImVec2(0,0), ImVec2(1,1), bb.Min, bb.Max);

    //     drawList->AddLine(a, b, ImGui::GetColorU32(ImGuiCol_PlotLines, 0.5f));
    // }

    if (hovered || draggingPoint)
    {
        // control points
        for (i = 0; i < pointCount; i++)
        {
            ImVec2 p = ImRemap(points[i], rangeMin, rangeMax, ImVec2(0, 0), ImVec2(1, 1));
            p.y = 1.0f - p.y;
            p = ImRemap(p, ImVec2(0, 0), ImVec2(1, 1), bb.Min, bb.Max);

            if(hoveredPoint == i)
                drawList->AddCircleFilled(p, pointRadiusInPixels, ImColor(100, 255, 255));
            else
                drawList->AddCircleFilled(p, pointRadiusInPixels, ImColor(255, 255, 255));
        }
    }

    // drawList->PopClipRect();

    // // draw the text at mouse position
    // char buf[128];
    // const char* str = label;

    // if (hovered || draggingPoint)
    // {
    //     ImVec2 pos = (g.IO.MousePos - bb.Min) / (bb.Max - bb.Min);
    //     pos.y = 1.0f - pos.y;

    //     pos = ImLerp(rangeMin, rangeMax, pos);

    //     snprintf(buf, sizeof(buf), "%s (%.2f,%.2f)", label, pos.x, pos.y);
    //     str = buf;
    // }

    // ImGui::RenderTextClipped(ImVec2(bb.Min.x, bb.Min.y + style.FramePadding.y), bb.Max, str, NULL, NULL, ImVec2(0.5f, 0.5f));

    // buttons; @todo: mirror, smooth, tessellate
    // if (ImGui::BeginPopupContextItem(label))
    // {
    //     if (ImGui::Selectable("Reset"))
    //     {
    //         points[0] = rangeMin;
    //         points[1] = rangeMax;
    //         points[2].x = CurveTerminator;
    //     }
    //     if (ImGui::Selectable("Flip"))
    //     {
    //         for (i = 0; i < pointCount; ++i)
    //         {
    //             const float yVal = 1.0f - ImRemap(points[i].y, rangeMin.y, rangeMax.y, 0, 1);
    //             points[i].y = ImRemap(yVal, 0, 1, rangeMin.y, rangeMax.y);
    //         }
    //     }
    //     if (ImGui::Selectable("Mirror"))
    //     {
    //         for (int i = 0, j = pointCount - 1; i < j; i++, j--)
    //         {
    //             ImSwap(points[i], points[j]);
    //         }
    //         for (i = 0; i < pointCount; ++i)
    //         {
    //             const float xVal = 1.0f - ImRemap(points[i].x, rangeMin.x, rangeMax.x, 0, 1);
    //             points[i].x = ImRemap(xVal, 0, 1, rangeMin.x, rangeMax.x);
    //         }
    //     }
    

    //     ImGui::EndPopup();
    // }

    ImGui::PopID();

    if (selection)
    {
        *selection = currentSelection;
    }

    return modified;
}
