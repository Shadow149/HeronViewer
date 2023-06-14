#pragma once
#include "Module.h"
#include "Image.h"
#include "Utils.h"
#include "Status.h"
#include "mini/ini.h"
#include "Preferences.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>


#define LOW_DEFAULT 0.0f
#define MID_DEFAULT 0.0f
#define HIGH_DEFAULT 0.0f
#define EXP_DEFAULT 0.0f
#define WHITE_DEFAULT 0.0f
#define CONTRAST_DEFAULT 0.0f

#define SAT_DEFAULT 0.0f
#define WB_DEFAULT 6200.0f

#define SHARP_DEFAULT 0.0f
#define BLUR_DEFAULT 1.0f

#define BW_LABEL "to Black and White"
#define COLOR_LABEL "to Color"

class Editor : public Module
{
public:
	Editor(Image* i, std::string n, bool v = true) : Module(n, v) {
		img = i;
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

	float sharpen_kernel[25];
	float sharpen_kernel33[9];

	float id[25] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	float blur_kernel[25] = { 1, 4, 6, 4, 1, 4, 16, 25, 26, 4, 6, 24, 36, 24, 6, 4, 16, 25, 26, 4, 1, 4, 6, 4, 1 };

	float id33[9] = { 0, 0, 0, 0, 1, 0, 0, 0, 0 };
	float blur_kernel33[9] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };

	bool sliderChanged = false;

	/////////////////// slider values //////////////////////////////

	const char* bw_label = BW_LABEL;

	float low[4] = { LOW_DEFAULT, LOW_DEFAULT, LOW_DEFAULT, LOW_DEFAULT };
	float mid[4] = { MID_DEFAULT, MID_DEFAULT, MID_DEFAULT, MID_DEFAULT };
	float high[4] = { HIGH_DEFAULT, HIGH_DEFAULT, HIGH_DEFAULT, HIGH_DEFAULT };
	float exp[4] = { EXP_DEFAULT, EXP_DEFAULT, EXP_DEFAULT, EXP_DEFAULT };
	float contrast[4] = { CONTRAST_DEFAULT, CONTRAST_DEFAULT, CONTRAST_DEFAULT, CONTRAST_DEFAULT };
	float whites[4] = { WHITE_DEFAULT, WHITE_DEFAULT, WHITE_DEFAULT, WHITE_DEFAULT };

	float sat = SAT_DEFAULT;
	float wb = WB_DEFAULT;

	float sharp = SHARP_DEFAULT;
	float blur = BLUR_DEFAULT;
	
	float p_sharp = SHARP_DEFAULT;
	float p_blur = BLUR_DEFAULT;

	float high_thresh = 0;
	float shad_thresh = 0;
	float high_incr = 0;
	float shad_incr = 0;

	float shad_var = 0;
	float var_mult = 2;

	bool bw = false;

	////////////////////////////////////////////

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

