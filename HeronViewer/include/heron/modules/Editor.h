#pragma once
#include "Module.h"
#include "Image.h"
#include "Utils.h"
#include "Status.h"
#include "mini/ini.h"
#include "PreferencesDialog.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#include "SliderValues.h"

#include <algorithm>
#include <imnodes.h>

#include "Curve.h"
#include <imgui_internal.h>

#include "Widgets.h"

// TODO make these not suck
#define LOW_DEFAULT 0.0f
#define MID_DEFAULT 0.0f
#define HIGH_DEFAULT 0.0f
#define EXP_DEFAULT 0.0f
#define WHITE_DEFAULT 0.0f
#define CONTRAST_DEFAULT 0.0f

#define SAT_DEFAULT 0.0f
#define WB_DEFAULT 7200.0f

#define SHARP_DEFAULT 0.5f
#define BLUR_DEFAULT 1.0f

#define BW_LABEL "to Black and White"
#define COLOR_LABEL "to Color"

static int btn_id = 0;

class Editor : public Module
{
public:
	Editor(Image* i, const std::string& n, const bool v = true) : Module(n, v)
	{
		img_ = i;
		for(int i = 0; i < 9; i++) selection_idx_[i] = -1;
		reset();
	}

	void init() override;
	void render() override;
	void cleanup() override {};
	void reset();

	void set_changes()
	{
		img_->set_changes(&vals_);
	}

	void update_file()
	{
		config_set_ = false;
	}

	void update_config_file();
	void undo();
	void write_ini();
	void set_from_config_file();

private:
	void update_sharpen_kernel();
	void toggle_bw_label();

	void read_ini();

private:
	Image* img_;
	std::thread ini_writer_;

	std::string export_dir_;


	float id_[9] = {0, 0, 0, 0, 1, 0, 0, 0, 0};
	float blur_kernel_[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};

	bool slider_changed_ = false;

	SliderValues vals_ = SliderValues();
	SliderValues prev_vals_ = SliderValues();
	int selection_idx_[9];

	bool new_values_set_ = true;

	bool ini_writing_ = false;
	bool ini_reading_ = false;
	bool config_set_ = false;
};
