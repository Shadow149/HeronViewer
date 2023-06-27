#include "Editor.h"

bool Spinner(const char* label, float radius, int thickness, const ImU32& color) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	// Render
	window->DrawList->PathClear();

	int num_segments = 30;
	int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

	const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
	const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

	const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

	for (int i = 0; i < num_segments; i++) {
		const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
		window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
			centre.y + ImSin(a + g.Time * 8) * radius));
	}

	window->DrawList->PathStroke(color, false, thickness);
}

template<typename T>
bool SliderFloatReset(T &data, T reset_value, const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0)
{
	const bool change = ImGui::SliderFloat(label, v, v_min, v_max, format, flags);
	ImGui::SameLine();
	if (ImGui::Button(("R##" + std::to_string(btn_id++)).c_str())) {
		data = reset_value;
		return true;
	}
	return change;
}


void Editor::updateSharpenKernel() {
	float a[9], b[9], c[9], d[9], e[9];
	scalarMul(1.0f / 16.0f, blur_kernel, a, 3);
	scalarMul(vals.blur, a, b, 3);
	subMat(id, b, c, 3);
	scalarMul(vals.sharp, c, d, 3);
	addMat(id, d, e, 3);
	normalMat(e, vals.sharp_kernel, 3);
}

void Editor::toggleBwLabel() {
	if (vals.bw_label == BW_LABEL)
		vals.bw_label = COLOR_LABEL;
	else
		vals.bw_label = BW_LABEL;
}

void Editor::init()
{
	updateSharpenKernel();
	setChanges();
}

void Editor::render()
{
	if (!visible) { return; }

	if (img->imageLoaded && !configSet) {
		configSet = true;
		setFromConfigFile();
	}

	ImGui::Begin(name.c_str());

	if (ImGui::Button("Reset")) {
		reset();
		Status::setStatus("Settings Reset!");
	}
	ImGui::SameLine();

	if (ImGui::Button("Save")) {
		updateConfigFile();
		Status::setStatus("Saving...");
	}
	if (img->rendering) {
		ImGui::Button("Exporting...");
		Status::setStatus("Exporting...");
		ImGui::SameLine();
		Spinner("EXPORTING...", ImGui::CalcTextSize("Exporting...").y/2, 3, ImGui::GetColorU32(ImVec4(255, 255, 255, 255)));
	}
	else {
		if (ImGui::Button("Export")) {
			exportImage();
		}
	}

	ImGui::Separator();


	if (sliderChanged |= ImGui::Button(vals.bw ? COLOR_LABEL : BW_LABEL)) {
		vals.bw = !vals.bw;
	}

	ImGui::Checkbox("Invert", &vals.inv);

	sliderChanged |= ImGui::SliderFloat("White Balance", &vals.wb, 1667, 25000);

	if (ImGui::BeginTabBar("MyTabBar"))
	{
		for (int n = 0; n < 4; n++)
			if (ImGui::BeginTabItem(tab_names[n]))
			{
				sliderChanged |= SliderFloatReset(vals.expo[n], 0.0f, "Exposure", &(vals.expo[n]), -2, 2);
				sliderChanged |= SliderFloatReset(vals.contrast[n], 0.0f, "Contrast", &(vals.contrast[n]), -1, 1);
				sliderChanged |= SliderFloatReset(vals.high[n], 0.0f, "High", &(vals.high[n]), -1, 1);
				sliderChanged |= SliderFloatReset(vals.mid[n], 0.0f, "Mid", &(vals.mid[n]), -1, 1);
				sliderChanged |= SliderFloatReset(vals.whites[n], 0.0f, "Whites", &(vals.whites[n]), -1, 1);
				sliderChanged |= SliderFloatReset(vals.low[n], 0.0f, "Low", &(vals.low[n]), -1, 1);

				ImGui::Separator();

				sliderChanged |= SliderFloatReset(vals.lift[n], 0.0f, "Lift", &(vals.lift[n]), -1, 1);
				sliderChanged |= SliderFloatReset(vals.gamma[n], 1.0f, "Gamma", &(vals.gamma[n]), 0, 2);
				sliderChanged |= SliderFloatReset(vals.gain[n], 1.0f, "Gain", &(vals.gain[n]), -1, 2);


				ImGui::EndTabItem();
			}
		ImGui::EndTabBar();
	}

	ImGui::Separator();

	sliderChanged |= drawColorSelector("High", ImGui::GetWindowWidth() / 3, &vals.high[1], &vals.high[2], &vals.high[3]); ImGui::SameLine();
	sliderChanged |= drawColorSelector("Mid", ImGui::GetWindowWidth() / 3, &vals.mid[1], &vals.mid[2], &vals.mid[3]); 
	sliderChanged |= drawColorSelector("Low", ImGui::GetWindowWidth() / 3, &vals.low[1], &vals.low[2], &vals.low[3]); ImGui::SameLine();

	sliderChanged |= drawColorSelector("Lift", ImGui::GetWindowWidth() / 3, &vals.lift[1], &vals.lift[2], &vals.lift[3]);
	sliderChanged |= drawColorSelector("Gamma", ImGui::GetWindowWidth() / 3, &vals.gamma[1], &vals.gamma[2], &vals.gamma[3], false); ImGui::SameLine();
	sliderChanged |= drawColorSelector("Gain", ImGui::GetWindowWidth() / 3, &vals.gain[1], &vals.gain[2], &vals.gain[3], false,  0);



	ImGui::Separator();

	sliderChanged |= SliderFloatReset(vals.sat, 0.0f, "Saturation", &vals.sat, -1, 1);

	sliderChanged |= SliderFloatReset(vals.blur, 0.0f, "Sharpen: Blur", &vals.blur, 0, 5);
	sliderChanged |= SliderFloatReset(vals.sharp, 0.0f, "Sharpen: Sharp", &vals.sharp, 0, 5);
	if (vals.sharp != vals.p_sharp || vals.blur != vals.p_blur) {
		updateSharpenKernel();
		vals.p_sharp = vals.sharp;
		vals.p_blur = vals.blur;
	}
	ImGui::Separator();

	ImGui::Checkbox("Noise?", &vals.noise_selected);
	if (vals.noise_selected) {
		sliderChanged |= SliderFloatReset(vals.noise, 0.0f, "Noise", &vals.noise, 0, 5);
	}

	ImGui::Separator();

	sliderChanged |= SliderFloatReset(vals.hues[0], 0.0f, "Red", &vals.hues[0], -1, 1);
	sliderChanged |= SliderFloatReset(vals.hues[1], 0.0f, "Orange", &vals.hues[1], -1, 1);
	sliderChanged |= SliderFloatReset(vals.hues[2], 0.0f, "Yellow", &vals.hues[2], -1, 1);
	sliderChanged |= SliderFloatReset(vals.hues[3], 0.0f, "Green", &vals.hues[3], -1, 1);
	sliderChanged |= SliderFloatReset(vals.hues[4], 0.0f, "Cyan", &vals.hues[4], -1, 1);
	sliderChanged |= SliderFloatReset(vals.hues[5], 0.0f, "Blue", &vals.hues[5], -1, 1);
	sliderChanged |= SliderFloatReset(vals.hues[6], 0.0f, "Purple", &vals.hues[6], -1, 1);
	sliderChanged |= SliderFloatReset(vals.hues[7], 0.0f, "Pink", &vals.hues[7], -1, 1);

	ImGui::Separator();

	sliderChanged |= SliderFloatReset(vals.yiq_y, 0.0f, "Blue/Orange", &vals.yiq_y, -1, 1);
	sliderChanged |= SliderFloatReset(vals.yiq_z, 0.0f, "Green/Purple", &vals.yiq_z, -1, 1);
	sliderChanged |= SliderFloatReset(vals.xyz_y, 0.0f, "Yellow/Blue", &vals.xyz_y, -1, 1);
	sliderChanged |= SliderFloatReset(vals.xyz_z, 0.0f, "Purple/Green", &vals.xyz_z, -1, 1);

	ImGui::Separator();

	sliderChanged |= SliderFloatReset(vals.scope_brightness, 2.0f, "Scope Brightness", &vals.scope_brightness, 0, 10);

	ImGui::End();

	if (sliderChanged) {
		setChanges();
		sliderChanged = false;
	}

	if (!iniWriting || !iniReading) {
		if (iniWriter.joinable()) {
			iniWriter.join();
			Console::log("iniWriter joined");

		}
	}

	btn_id = 0;

}

void Editor::exportImage() {
	//Console::log(stripExtension(fileName));
	std::experimental::filesystem::path p = stripExtension(fileName);
	std::experimental::filesystem::path dir = Preferences::instance()->EXPORT_DIR;
	dir /= p;
	exportDir = dir.u8string() + "-edit.png";
	img->exportImage(exportDir.c_str());
}

void Editor::writeIni() {
	Console::log("WARNING: Editor::writeIni not implemented!");

	/*iniWriting = true;
	mINI::INIFile file(stripExtension(filePath) + ".ini");
	mINI::INIStructure ini;
	bool readSuc = file.read(ini);
	if (!readSuc) {
		Console::log("Ini doesn't exist, creating one...");
	}
	else {
		Console::log("Ini read...");
	}

	ini["sliders"]["bw"] = std::to_string(bw);
	ini["sliders"]["White Balance"] = std::to_string(wb);

	for (int n = 0; n < 4; n++) {
		ini["sliders"][std::string(tab_names[n]) + "_exp"] = std::to_string(exp[n]);
		ini["sliders"][std::string(tab_names[n]) + "_contrast"] = std::to_string(contrast[n]);
		ini["sliders"][std::string(tab_names[n]) + "_high"] = std::to_string(high[n]);
		ini["sliders"][std::string(tab_names[n]) + "_mid"] = std::to_string(mid[n]);
		ini["sliders"][std::string(tab_names[n]) + "_whites"] = std::to_string(whites[n]);
		ini["sliders"][std::string(tab_names[n]) + "_low"] = std::to_string(low[n]);
	}

	ini["sliders"]["sat"] = std::to_string(sat);
	ini["sliders"]["blur"] = std::to_string(blur);
	ini["sliders"]["sharp"] = std::to_string(sharp);
	ini["sliders"]["high_thresh"] = std::to_string(high_thresh);
	ini["sliders"]["shad_thresh"] = std::to_string(shad_thresh);
	ini["sliders"]["high_incr"] = std::to_string(high_incr);
	ini["sliders"]["shad_incr"] = std::to_string(shad_incr);
	ini["sliders"]["shad_var"] = std::to_string(shad_var);
	ini["sliders"]["var_mult"] = std::to_string(var_mult);

	if (!readSuc) {
		file.generate(ini);
		Console::log("Ini generated");
	}
	else {
		file.write(ini);
		Console::log("Ini updated");
	}
	Status::setStatus("Saved!");
	iniWriting = false;*/
}

void Editor::readIni() {
	Console::log("WARNING: Editor::readIni not implemented!");
	//iniReading = true;
	//mINI::INIFile file(stripExtension(filePath) + ".ini");
	//mINI::INIStructure ini;
	//bool readSuc = file.read(ini);
	//if (!readSuc) {
	//	Console::log("Ini doesn't exist, no settings to import...");
	//	iniReading = false;
	//	reset();
	//	return;
	//}
	//else {
	//	Console::log("Config setter ini read...");
	//}
	//try {
	//	bw = std::stof(ini["sliders"]["bw"]);

	//} catch (...) {
	//}

	//wb = std::stof(ini["sliders"]["White Balance"]);

	//for (int n = 0; n < 4; n++) {
	//	exp[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_exp"]);
	//	contrast[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_contrast"]);
	//	high[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_high"]);
	//	mid[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_mid"]);
	//	whites[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_whites"]);
	//	low[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_low"]);
	//}

	//sat = std::stof(ini["sliders"]["sat"]);
	//blur = std::stof(ini["sliders"]["blur"]);
	//sharp = std::stof(ini["sliders"]["sharp"]);
	//high_thresh = std::stof(ini["sliders"]["high_thresh"]);
	//shad_thresh = std::stof(ini["sliders"]["shad_thresh"]);
	//high_incr = std::stof(ini["sliders"]["high_incr"]);
	//shad_incr = std::stof(ini["sliders"]["shad_incr"]);
	//shad_var = std::stof(ini["sliders"]["shad_var"]);
	//var_mult = std::stof(ini["sliders"]["var_mult"]);
	//
	//iniReading = false;
	//Status::setStatus("Read settings successfully!");
	//setChanges();
}


void Editor::updateConfigFile() {
	if (!imLoaded || iniReading || iniWriting)
		return;
	iniWriter = std::thread(&Editor::writeIni, this);
}

void Editor::setFromConfigFile() {
	if (iniWriting || iniReading)
		return;
	iniWriter = std::thread(&Editor::readIni, this);
}

void Editor::cleanup()
{
}

void Editor::reset()
{
	vals.bw_label = BW_LABEL;

	/// NEW

	vals.bw = false;
	vals.inv = false;

	std::fill_n(vals.low, 4,LOW_DEFAULT);
	std::fill_n(vals.mid, 4,MID_DEFAULT);
	std::fill_n(vals.high, 4,HIGH_DEFAULT);
	std::fill_n(vals.expo, 4,EXP_DEFAULT);
	std::fill_n(vals.contrast, 4,CONTRAST_DEFAULT);
	std::fill_n(vals.whites, 4,WHITE_DEFAULT);

	std::fill_n(vals.lift, 4, 0.0f);
	std::fill_n(vals.gamma, 4, 1.0f);
	std::fill_n(vals.gain, 4, 1.0f);

	std::fill_n(vals.hues, 8, 0.0f);

	vals.sat = SAT_DEFAULT;
	vals.wb = WB_DEFAULT;

	vals.noise_selected = false;
	vals.noise = 0;

	vals.sharp = SHARP_DEFAULT;
	vals.p_sharp = SHARP_DEFAULT; // Prev sharp
	vals.blur = BLUR_DEFAULT;
	vals.p_blur = BLUR_DEFAULT; // Prev blur


	vals.yiq_y = 0;
	vals.yiq_z = 0;
	vals.xyz_y = 0;
	vals.xyz_z = 0;

	vals.scope_brightness = 2;

	setChanges();

}

void Editor::setChanges() {
	img->setChanges(&vals);
}

void Editor::updateFile(std::string& fn, std::string& fp)
{
	configSet = false;
	fileName = fn;
	filePath = fp;
	//setFromConfigFile();
}

void Editor::loaded(bool l) {
	imLoaded = l;
}

