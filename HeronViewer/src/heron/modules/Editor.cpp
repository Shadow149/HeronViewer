#include "Editor.h"

#include "Heron.h"

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

	sliderChanged |= SliderFloatReset(vals.sat_ref, 1.0f, "Saturation Refine", &(vals.sat_ref), 0, 1);

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

	if (ImGui::CollapsingHeader("HSL Editing")) {


		if (ImGui::CollapsingHeader("Red")) {
			sliderChanged |= SliderFloatReset(vals.hues[0], 0.0f, "Red Hue", &vals.hues[0], -1, 1);
			sliderChanged |= SliderFloatReset(vals.sats[0], 0.0f, "Red Saturation", &vals.sats[0], -1, 1);
			sliderChanged |= SliderFloatReset(vals.lums[0], 0.0f, "Red Value", &vals.lums[0], -1, 1);
		}
		ImGui::Separator();


		if (ImGui::CollapsingHeader("Orange")) {
			sliderChanged |= SliderFloatReset(vals.hues[1], 0.0f, "Orange Hue", &vals.hues[1], -1, 1);
			sliderChanged |= SliderFloatReset(vals.sats[1], 0.0f, "Orange Saturation", &vals.sats[1], -1, 1);
			sliderChanged |= SliderFloatReset(vals.lums[1], 0.0f, "Orange Value", &vals.lums[1], -1, 1);
		}
		ImGui::Separator();


		if (ImGui::CollapsingHeader("Yellow")) {
			sliderChanged |= SliderFloatReset(vals.hues[2], 0.0f, "Yellow Hue", &vals.hues[2], -1, 1);
			sliderChanged |= SliderFloatReset(vals.sats[2], 0.0f, "Yellow Saturation", &vals.sats[2], -1, 1);
			sliderChanged |= SliderFloatReset(vals.lums[2], 0.0f, "Yellow Value", &vals.lums[2], -1, 1);
		}
		ImGui::Separator();


		if (ImGui::CollapsingHeader("Green")) {
			sliderChanged |= SliderFloatReset(vals.hues[3], 0.0f, "Green Hue", &vals.hues[3], -1, 1);
			sliderChanged |= SliderFloatReset(vals.sats[3], 0.0f, "Green Saturation", &vals.sats[3], -1, 1);
			sliderChanged |= SliderFloatReset(vals.lums[3], 0.0f, "Green Value", &vals.lums[3], -1, 1);
		}
		ImGui::Separator();


		if (ImGui::CollapsingHeader("Cyan")) {
			sliderChanged |= SliderFloatReset(vals.hues[4], 0.0f, "Cyan Hue", &vals.hues[4], -1, 1);
			sliderChanged |= SliderFloatReset(vals.sats[4], 0.0f, "Cyan Saturation", &vals.sats[4], -1, 1);
			sliderChanged |= SliderFloatReset(vals.lums[4], 0.0f, "Cyan Value", &vals.lums[4], -1, 1);
		}
		ImGui::Separator();


		if (ImGui::CollapsingHeader("Blue")) {
			sliderChanged |= SliderFloatReset(vals.hues[5], 0.0f, "Blue Hue", &vals.hues[5], -1, 1);
			sliderChanged |= SliderFloatReset(vals.sats[5], 0.0f, "Blue Saturation", &vals.sats[5], -1, 1);
			sliderChanged |= SliderFloatReset(vals.lums[5], 0.0f, "Blue Value", &vals.lums[5], -1, 1);
		}
		ImGui::Separator();


		if (ImGui::CollapsingHeader("Purple")) {
			sliderChanged |= SliderFloatReset(vals.hues[6], 0.0f, "Purple Hue", &vals.hues[6], -1, 1);
			sliderChanged |= SliderFloatReset(vals.sats[6], 0.0f, "Purple Saturation", &vals.sats[6], -1, 1);
			sliderChanged |= SliderFloatReset(vals.lums[6], 0.0f, "Purple Value", &vals.lums[6], -1, 1);
		}
		ImGui::Separator();


		if (ImGui::CollapsingHeader("Pink")) {
			sliderChanged |= SliderFloatReset(vals.hues[7], 0.0f, "Pink Hue", &vals.hues[7], -1, 1);
			sliderChanged |= SliderFloatReset(vals.sats[7], 0.0f, "Pink Saturation", &vals.sats[7], -1, 1);
			sliderChanged |= SliderFloatReset(vals.lums[7], 0.0f, "Pink Value", &vals.lums[7], -1, 1);
		}
	}


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
	exportDir = dir.u8string() + "-edit.jpg";
	img->exportImage(exportDir.c_str());
}

void Editor::writeIni() {
	iniWriting = true;
	mINI::INIFile file(stripExtension(filePath) + ".ini");
	mINI::INIStructure ini;
	bool readSuc = file.read(ini);
	if (!readSuc) {
		Console::log("Ini doesn't exist, creating one...");
	}

	Console::log("Writing ini to " + stripExtension(filePath) + ".ini");

	ini["version"]["number"] = std::to_string(HERON_VERSION);

	for (int n = 0; n < 4; n++) {
		ini["sliders"][std::string(tab_names[n]) + "_expo"] = std::to_string(vals.expo[n]);
		ini["sliders"][std::string(tab_names[n]) + "_contrast"] = std::to_string(vals.contrast[n]);
		ini["sliders"][std::string(tab_names[n]) + "_high"] = std::to_string(vals.high[n]);
		ini["sliders"][std::string(tab_names[n]) + "_mid"] = std::to_string(vals.mid[n]);
		ini["sliders"][std::string(tab_names[n]) + "_whites"] = std::to_string(vals.whites[n]);
		ini["sliders"][std::string(tab_names[n]) + "_low"] = std::to_string(vals.low[n]);
		ini["sliders"][std::string(tab_names[n]) + "_lift"] = std::to_string(vals.lift[n]);
		ini["sliders"][std::string(tab_names[n]) + "_gain"] = std::to_string(vals.gain[n]);
		ini["sliders"][std::string(tab_names[n]) + "_gamma"] = std::to_string(vals.gamma[n]);
	}

	for (int n = 0; n < 8; n++) {
		ini["sliders"]["hues_" + std::to_string(n)] = std::to_string(vals.hues[n]);
		ini["sliders"]["sats_" + std::to_string(n)] = std::to_string(vals.sats[n]);
		ini["sliders"]["lums_" + std::to_string(n)] = std::to_string(vals.lums[n]);
	}


	ini["sliders"]["bw"] = std::to_string(vals.bw);
	ini["sliders"]["inv"] = std::to_string(vals.inv);

	ini["sliders"]["sat"] = std::to_string(vals.sat);
	ini["sliders"]["wb"] = std::to_string(vals.wb);

	ini["sliders"]["noise_selected"] = std::to_string(vals.noise_selected);
	ini["sliders"]["noise"] = std::to_string(vals.noise);

	ini["sliders"]["sharp"] = std::to_string(vals.sharp);
	ini["sliders"]["p_sharp"] = std::to_string(vals.p_sharp);
	ini["sliders"]["blur"] = std::to_string(vals.blur);
	ini["sliders"]["p_blur"] = std::to_string(vals.p_blur);

	ini["sliders"]["yiq_y"] = std::to_string(vals.yiq_y);
	ini["sliders"]["yiq_z"] = std::to_string(vals.yiq_z);
	ini["sliders"]["xyz_y"] = std::to_string(vals.xyz_y);
	ini["sliders"]["xyz_z"] = std::to_string(vals.xyz_z);

	ini["sliders"]["sat_ref"] = std::to_string(vals.sat_ref);

	if (!readSuc) {
		file.generate(ini);
		Console::log("Ini generated");
	}
	else {
		file.write(ini);
		Console::log("Ini updated");
	}
	Status::setStatus("Saved!");
	iniWriting = false;
}

void Editor::readIni() {
	iniReading = true;
	mINI::INIFile file(stripExtension(filePath) + ".ini");
	mINI::INIStructure ini;
	bool readSuc = file.read(ini);
	if (!readSuc) {
		Console::log("Ini doesn't exist, no settings to import...");
		iniReading = false;
		reset();
		return;
	}

	Console::log("Reading ini " + stripExtension(filePath) + ".ini");

	float ini_ver;

	try {

		ini_ver = std::stof(ini["version"]["number"]);

	} catch (...)
	{
		Console::log("ERROR: Old version of Ini with no version number... Cannot read ini");
		return;
	}

	if (ini_ver != HERON_VERSION)
	{
		Console::log("ERROR: Ini version doesn't match current version of Heron! Delete the file's ini to read!");
		return;
	}


	for (int n = 0; n < 4; n++) {
		vals.expo[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_expo"]);
		vals.contrast[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_contrast"]);
		vals.high[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_high"]);
		vals.mid[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_mid"]);
		vals.whites[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_whites"]);
		vals.low[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_low"]);

		vals.lift[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_lift"]);
		vals.gamma[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_gamma"]);
		vals.gain[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_gain"]);

	}

	for (int n = 0; n < 8; n++) {
		vals.hues[n] = std::stof(ini["sliders"]["hues_" + std::to_string(n)]);
		vals.lums[n] = std::stof(ini["sliders"]["lums_" + std::to_string(n)]);
		vals.sats[n] = std::stof(ini["sliders"]["sats_" + std::to_string(n)]);
	}

	vals.bw = std::stof(ini["sliders"]["bw"]);
	vals.inv = std::stof(ini["sliders"]["inv"]);

	vals.sat = std::stof(ini["sliders"]["sat"]);
	vals.wb = std::stof(ini["sliders"]["wb"]);

	vals.noise_selected = std::stof(ini["sliders"]["noise_selected"]);
	vals.noise = std::stof(ini["sliders"]["noise"]);

	vals.sharp = std::stof(ini["sliders"]["sharp"]);
	vals.p_sharp = std::stof(ini["sliders"]["p_sharp"]);
	vals.blur = std::stof(ini["sliders"]["blur"]);
	vals.p_blur = std::stof(ini["sliders"]["p_blur"]);

	vals.yiq_y = std::stof(ini["sliders"]["yiq_y"]);
	vals.yiq_z = std::stof(ini["sliders"]["yiq_z"]);
	vals.xyz_y = std::stof(ini["sliders"]["xyz_y"]);
	vals.xyz_z = std::stof(ini["sliders"]["xyz_z"]);

	vals.sat_ref = std::stof(ini["sliders"]["sat_ref"]);
	
	iniReading = false;
	Status::setStatus("Read settings successfully!");
	setChanges();
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
	std::fill_n(vals.sats, 8, 0.0f);
	std::fill_n(vals.lums, 8, 0.0f);

	vals.sat_ref = 1;

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

