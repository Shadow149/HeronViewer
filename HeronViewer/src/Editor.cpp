#include "Editor.h"
#include <algorithm>
#include <imnodes.h>

#include "Curve.h"

glm::vec3 rgb2hsv(glm::vec3 c)
{
	glm::vec4 K = glm::vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
	glm::vec4 p = glm::mix(glm::vec4(c.b, c.g, K.w, K.z), glm::vec4(c.g, c.b, K.x, K.y), glm::step(c.b, c.g));
	glm::vec4 q = glm::mix(glm::vec4(p.x, p.y, p.w, c.r), glm::vec4(c.r, p.y, p.z, p.x), glm::step(p.x, c.r));

	float d = q.x - glm::min(q.w, q.y);
	float e = 1.0e-10;
	return {abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x};
}

glm::vec3 hsv2rgb(glm::vec3 c)
{
	glm::vec4 K = glm::vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	glm::vec3 p = abs(glm::fract(glm::vec3(c.x, c.x, c.x) + glm::vec3(K.x, K.y, K.z)) * 6.0f - glm::vec3(K.w, K.w, K.w));
	return c.z * glm::mix(glm::vec3(K.x, K.x, K.x), clamp(p - glm::vec3(K.x, K.x, K.x), 0.0f, 1.0f), c.y);
}


long double TAU = 6.2831855;
// color editor for 3 or 4 component colors
bool drawColorSelector(const char* label, float height, float* r, float* g, float* b) {
	ImGui::PushID(label);

	ImVec2 buttonStart = ImGui::GetCursorScreenPos();


	//ImGui::Image((void*)g_wheelTexture, ImVec2(height, height), ImVec2(0, 0), ImVec2(1, 1));

	ImGui::SetCursorScreenPos(buttonStart);
	ImGui::InvisibleButton(label, ImVec2(height, height)); ImGui::SameLine();

	glm::vec3 rgb = glm::vec3(glm::max(0.f, *r), glm::max(0.f, *g), glm::max(0.f, *b));
	glm::vec3 hsv = rgb2hsv(rgb);

	float h = hsv.r;
	float s = hsv.g;
	float v = hsv.b;

	glm::vec2 onCircle = glm::vec2(cos(h * TAU), sin(h * TAU)) * s;

	glm::vec2 center = glm::vec2(buttonStart.x, buttonStart.y) + glm::vec2(height, height) * 0.5f + onCircle * height * 0.5f;

	bool changed = false;
	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
		float speed = 0.3f;
		if (ImGui::GetIO().KeyShift) {
			speed *= 1.0f;
		}
		const ImVec2 delta = ImGui::GetCursorScreenPos();
		onCircle += glm::vec2(glm::vec2(delta.x - buttonStart.x, delta.y - buttonStart.y));
		s = glm::min(1.0f, glm::length(onCircle));
		if (s == 0.0f) {
			h = 0.0f;
		}
		else {
			h = atan2f(onCircle.y, onCircle.x) / TAU;
			if (h < 0) {
				h += 1.0f;
			}
		}
		//center = (glm::vec2(delta.x, delta.y));
		//ImGui::ResetMouseDragDelta();
		ImGui::GetWindowDrawList()->AddCircle(ImVec2(center.x - ImGui::GetCursorScreenPos().x + ImGui::GetMousePos().x + height * 0.5f,
			center.y - ImGui::GetCursorScreenPos().y + ImGui::GetMousePos().y - height * 0.5f)
			, 3.0f, ImColor(255, 255, 255));
		changed = true;
	}
	//printf("%f, %f\n", center.x - ImGui::GetMousePos().x, center.y - ImGui::GetMousePos().y);
	

	glm::vec4 c = glm::vec4(hsv2rgb(glm::vec3(h, s, 0.5f)), 1.0f);
	ImVec4 im_c = ImVec4(c.x, c.y, c.z, c.w);
	ImGui::PushStyleColor(ImGuiCol_FrameBg, im_c);
	changed |= ImGui::VSliderFloat("##v", ImVec2(10, height), &v, 0.0f, 10.0f, "");
	ImGui::PopStyleColor();


	ImGui::SameLine();

	if (changed) {
		rgb = (hsv2rgb(glm::vec3(h, s, v)));
		*r = rgb.r;
		*g = rgb.g;
		*b = rgb.b;
	}

	ImGui::PopID();
	return changed;
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
	scalarMul(1.0f / 16.0f, blur_kernel33, a, 3);
	scalarMul(vals.blur, a, b, 3);
	subMat(id33, b, c, 3);
	scalarMul(vals.sharp, c, d, 3);
	addMat(id33, d, e, 3);
	normalMat(e, sharpen_kernel33, 3);
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
	ImGui::SameLine();
	if (img->rendering) {
		ImGui::Button("Exporting...");
		Status::setStatus("Exporting...");
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

				sliderChanged |= SliderFloatReset(vals.lift[n], 0.0f, "lift", &(vals.lift[n]), -1, 1);
				sliderChanged |= SliderFloatReset(vals.gamma[n], 1.0f, "gamma", &(vals.gamma[n]), 0, 2);
				sliderChanged |= SliderFloatReset(vals.gain[n], 1.0f, "gain", &(vals.gain[n]), -1, 2);


				ImGui::EndTabItem();
			}
		ImGui::EndTabBar();
	}

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


	//float r, g, b;
	//drawColorSelector("Foo", 200, &r, &g, &b);

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

