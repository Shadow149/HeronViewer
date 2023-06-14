#include "Editor.h"

void Editor::updateSharpenKernel() {
	float a[9], b[9], c[9], d[9], e[9];
	scalarMul(1.0f / 16.0f, blur_kernel33, a, 3);
	scalarMul(blur, a, b, 3);
	subMat(id33, b, c, 3);
	scalarMul(sharp, c, d, 3);
	addMat(id33, d, e, 3);
	normalMat(e, sharpen_kernel33, 3);
}

void Editor::toggleBwLabel() {
	if (bw_label == BW_LABEL)
		bw_label = COLOR_LABEL;
	else
		bw_label = BW_LABEL;
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

	if (sliderChanged |= ImGui::Button(bw ? COLOR_LABEL : BW_LABEL)) {
		bw = !bw;
		//toggleBwLabel();
	}

	sliderChanged |= ImGui::SliderFloat("White Balance", &wb, 1667, 25000);

	if (ImGui::BeginTabBar("MyTabBar"))
	{
		for (int n = 0; n < 4; n++)
			if (ImGui::BeginTabItem(tab_names[n]))
			{
				sliderChanged |= ImGui::SliderFloat("Exposure", &(exp[n]), -2, 2);
				sliderChanged |= ImGui::SliderFloat("Contrast", &(contrast[n]), -1, 1);
				sliderChanged |= ImGui::SliderFloat("High", &(high[n]), -1, 1);
				sliderChanged |= ImGui::SliderFloat("Mid", &(mid[n]), -1, 1);
				sliderChanged |= ImGui::SliderFloat("Whites", &(whites[n]), -1, 1);
				sliderChanged |= ImGui::SliderFloat("Low", &(low[n]), -1, 1);
				ImGui::EndTabItem();
			}
		ImGui::EndTabBar();
	}

	ImGui::Separator();

	sliderChanged |= ImGui::SliderFloat("Saturation", &sat, -1, 1);

	sliderChanged |= ImGui::SliderFloat("Sharpen: Blur", &blur, 0, 5);
	sliderChanged |= ImGui::SliderFloat("Sharpen: Sharp", &sharp, 0, 5);
	if (sharp != p_sharp || blur != p_blur) {
		updateSharpenKernel();
		p_sharp = sharp;
		p_blur = blur;
	}

	sliderChanged |= ImGui::SliderFloat("Noise", &high_thresh, 0, 5);
	sliderChanged |= ImGui::SliderFloat("Blue/Orange", &shad_thresh, -1, 1);
	sliderChanged |= ImGui::SliderFloat("Green/Purple", &high_incr, -1, 1);
	sliderChanged |= ImGui::SliderFloat("Blue", &shad_incr, -1, 1);
	sliderChanged |= ImGui::SliderFloat("Red", &shad_var, -1, 1);
	sliderChanged |= ImGui::SliderFloat("Variance mult", &var_mult, 0, 10);

	if (ImGui::Button("Reset")) {
		reset();
		Status::setStatus("Settings Reset!");
	}

	if (ImGui::Button("Save")) {
		updateConfigFile();
		Status::setStatus("Saving...");
	}

	if (img->rendering) {
		ImGui::Button("Exporting...");
		Status::setStatus("Exporting...");
	}
	else {
		if (ImGui::Button("Export")) {
			exportImage();
		}
	}

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
	iniWriting = true;
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
	else {
		Console::log("Config setter ini read...");
	}
	try {
		bw = std::stof(ini["sliders"]["bw"]);

	} catch (...) {
	}

	wb = std::stof(ini["sliders"]["White Balance"]);

	for (int n = 0; n < 4; n++) {
		exp[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_exp"]);
		contrast[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_contrast"]);
		high[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_high"]);
		mid[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_mid"]);
		whites[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_whites"]);
		low[n] = std::stof(ini["sliders"][std::string(tab_names[n]) + "_low"]);
	}

	sat = std::stof(ini["sliders"]["sat"]);
	blur = std::stof(ini["sliders"]["blur"]);
	sharp = std::stof(ini["sliders"]["sharp"]);
	high_thresh = std::stof(ini["sliders"]["high_thresh"]);
	shad_thresh = std::stof(ini["sliders"]["shad_thresh"]);
	high_incr = std::stof(ini["sliders"]["high_incr"]);
	shad_incr = std::stof(ini["sliders"]["shad_incr"]);
	shad_var = std::stof(ini["sliders"]["shad_var"]);
	var_mult = std::stof(ini["sliders"]["var_mult"]);
	
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
	bw_label = BW_LABEL;

	memset(low, 0, 4 * sizeof(float));
	memset(mid, 0, 4 * sizeof(float));
	memset(high, 0, 4 * sizeof(float));
	memset(exp, 0, 4 * sizeof(float));
	memset(contrast, 0, 4 * sizeof(float));
	memset(whites, 0, 4 * sizeof(float));

	sat = SAT_DEFAULT;
	wb = WB_DEFAULT;

	sharp = SHARP_DEFAULT;
	blur = BLUR_DEFAULT;

	p_sharp = SHARP_DEFAULT;
	p_blur = BLUR_DEFAULT;

	high_thresh = 0;
	shad_thresh = 0;
	high_incr = 0;
	shad_incr = 0;

	shad_var = 0;
	var_mult = 2;

	bw = false;

	setChanges();

}

void Editor::setChanges() {
	img->setChanges(low, whites, mid, high, exp, contrast, sharpen_kernel33, bw, high_thresh, shad_thresh, high_incr, shad_incr, shad_var, var_mult, sat, wb);
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

