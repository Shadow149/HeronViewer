#include "Histogram.h"
#include "Image.h"

void Histogram::init()
{
	int width = img->getWidth();
	int height = img->getHeight();
	unsigned char* data = img->getData();

	if (!data) {
		return;
	}
	for (int n = 0; n < 4; n++) {
		for (int i = 0; i < 256; i++) {
			hist[n][i] = 0;
		}
	}
	//for (int n = 1; n < 4; n++) {
	//	for (int i = n-1; i < width * height * sizeof(char); i += sizeof(char)) {
	//		int val = 0.2126 * data[i] + 0.7152 * data[i + 1] + 0.0722 * data[i + 2];
	//		hist[n][val] += 1;
	//		//Console::log(std::to_string(n) + " - " + std::to_string(data[i]));
	//		//hist[n][data[i]] += 1;
	//	}
	//}
	for (int i = 0; i < 256; i++) {
		hist_x[i] = i;
	}
}

void Histogram::render()
{
	if (!visible) { return; }

	if (img->getChanged()) {
		//for (int n = 0; n < 4; n++) {
		//	for (int i = 0; i < 256; i++) {
		//		hist[n][i] = 0;
		//	}
		//}
		//for (int n = 0; n < 4; n++) {
			//for (int i = 0; i < 256; i++) {
			//	float t = (float)i / 255.0f;
			//	//t = img->calcCurve(t + 0.00001, n);
			//	//img->calcCurveHist(t + 0.001, n, &r, &g, &b);
			//	float l = img->calcCurve(t, 0);
			//	float r = img->calcCurve(t, 1);
			//	float g = img->calcCurve(t, 2);
			//	float b = img->calcCurve(t, 3);

			//	r = clamp(r, 0.01f, 0.99f);
			//	g = clamp(g, 0.01f, 0.99f);
			//	b = clamp(b, 0.01f, 0.99f);
			//	l = clamp(l, 0.01f, 0.99f);


			//	int r_index = floor(r * 255.0f);
			//	int g_index = floor(g * 255.0f);
			//	int b_index = floor(b * 255.0f);
			//	int l_index = floor(l * 255.0f);

			//	hist[0][l_index] ++;
			//	hist[1][r_index] ++;
			//	hist[2][g_index] ++;
			//	hist[3][b_index] ++;
			//}
		//}

		int width = img->getWidth();
		int height = img->getHeight();
		if (height == 1 || !img->histogram_loaded) return;
		unsigned char* data = img->getData();
		for (int n = 0; n < 4; n++) {
			for (int i = 0; i < 256; i++) {
				hist[n][i] = 0;
			}
		}
		//for (int n = 1; n < 4; n++) {
		//	for (int i = n; i < width * height * 3; i += 3) {
		//		//int val = 0.2126 * data[i] + 0.7152 * data[i + 1] + 0.0722 * data[i + 2];
		//		hist[3-n][data[i]] ++;
		//		//Console::log(std::to_string(n) + " - " + std::to_string(data[i]));
		//		//hist[n][data[i]] += 1;
		//	}
		//}
		for(int i = 0; i < 256; i ++)
		{
			hist[0][i] = img->histogram[i];
		}
	}

	ImGui::Begin(name.c_str());
	ImVec2 size = ImGui::GetWindowSize();
	ImPlot::GetStyle().PlotMinSize = ImVec2(0, 0);
	ImPlot::GetStyle().PlotDefaultSize = ImVec2(-1, size.y - 30);
	ImPlot::GetStyle().AntiAliasedLines = true;
	//if (ImPlot::BeginPlot("Hist plot", ImVec2(-1,-1), plotFlags)) {
	if (ImPlot::BeginPlot("Hist plot", ImVec2(-1,-1), plotFlags)) {
		//ImPlot::SetupAxes("x", "y", flags, flags);
		ImPlot::SetupAxes("x", "y", flags, flags);
		for (int n = 3; n >= 1; n--) {
			ImPlot::SetNextLineStyle(channel_colors[n]);
			ImPlot::PlotLine(tab_names[n], hist_x, hist[n], 256);
		}
		ImPlot::PlotLine(tab_names[0], hist_x, hist[0], 256);
		ImPlot::EndPlot();
	}
	ImGui::End();
}

void Histogram::cleanup()
{
}