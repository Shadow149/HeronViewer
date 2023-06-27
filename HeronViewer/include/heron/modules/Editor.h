#pragma once
#include "Module.h"
#include "Image.h"
#include "Utils.h"
#include "Status.h"
#include "mini/ini.h"
#include "Preferences.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#include "SliderValues.h"

#include <algorithm>
#include <imnodes.h>

#include "Curve.h"
#include <imgui_internal.h>

#include "Widgets.h"


#define LOW_DEFAULT 0.0f
#define MID_DEFAULT 0.0f
#define HIGH_DEFAULT 0.0f
#define EXP_DEFAULT 0.0f
#define WHITE_DEFAULT 0.0f
#define CONTRAST_DEFAULT 0.0f

#define SAT_DEFAULT 0.0f
#define WB_DEFAULT 7200.0f

#define SHARP_DEFAULT 0.2f
#define BLUR_DEFAULT 1.0f

#define BW_LABEL "to Black and White"
#define COLOR_LABEL "to Color"

static int btn_id = 0;

class Editor : public Module
{
public:
	Editor(Image* i, std::string n, bool v = true) : Module(n, v) {
		img = i;
		reset();
	};
	void init();
	void render();
	void cleanup();
	void reset();

	void setChanges();
	void updateFile(std::string &fn, std::string &fp);
	void loaded(bool l);

	void updateConfigFile();

private:
	Image* img;
	std::thread iniWriter;

	std::string fileName, filePath;
	std::string exportDir;


	float id[9] = { 0, 0, 0, 0, 1, 0, 0, 0, 0 };
	float blur_kernel[9] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };

	bool sliderChanged = false;

	SliderValues vals = SliderValues();

	bool iniWriting = false;
	bool iniReading = false;
	bool imLoaded = false;
	bool configSet = false;

	void updateSharpenKernel();
	void toggleBwLabel();
	void exportImage();

	void setFromConfigFile();
	void writeIni();
	void readIni();
};

