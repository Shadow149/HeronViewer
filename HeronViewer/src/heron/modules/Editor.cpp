#include "Editor.h"

#include "ExportDialog.h"
#include "Heron.h"

#include <fcntl.h>

#include "catalog.h"
#include "serialise.h"

template <typename T>
bool slider_float_reset(T& data, T reset_value, const char* label, float* v, const float v_min, const float v_max,
                        const char* format = "%.2f", const ImGuiSliderFlags flags = 0,
                        const ImU32 left_colour = ImColor(50, 50, 50),
                        const ImU32 right_colour = ImColor(220, 220, 220))
{
	const bool change = ImGui::SliderFloat((std::string(label) + "##" + std::to_string(btn_id++)).c_str(), v, v_min, v_max, format, flags, left_colour, right_colour);
	ImGui::SameLine();
	if (ImGui::Button(("R##" + std::to_string(btn_id++)).c_str()))
	{
		data = reset_value;
		return true;
	}
	return change;
}


void Editor::update_sharpen_kernel()
{
	float a[9], b[9], c[9], d[9], e[9];
	scalar_mul(1.0f / 16.0f, blur_kernel_, a, 3);
	scalar_mul(vals_.blur, a, b, 3);
	sub_mat(id_, b, c, 3);
	scalar_mul(vals_.sharp, c, d, 3);
	add_mat(id_, d, e, 3);
	normal_mat(e, vals_.sharp_kernel, 3);
}

void Editor::toggle_bw_label()
{
	if (vals_.bw_label == BW_LABEL)
		vals_.bw_label = COLOR_LABEL;
	else
		vals_.bw_label = BW_LABEL;
}

void Editor::init()
{
	update_sharpen_kernel();
	set_changes();
}

void Editor::render()
{
	//if (img_->is_loaded() && !config_set_)
	//{
	//	config_set_ = true;
	//	set_from_config_file();
	//}


	ImGui::Begin(name.c_str());
	update_mouse_in_window();


	if (ImGui::IsWindowFocused() && mouse_in_window_ && ImGui::IsMouseDown(0) && new_values_set_ && slider_changed_)
	{
		prev_vals_ = vals_;
		new_values_set_ = false;
		vals_.show_low_res = true;
	}
	slider_changed_ = false;


	if (ImGui::Button("Reset Settings"))
	{
		reset();
		Status::set_status("Settings Reset!");
	}
	ImGui::SameLine();
	if (ImGui::Button("Export"))
	{
		ExportDialog::instance()->set_file_name(std::string(catalog::instance()->get_current_item()->file_name));
		ExportDialog::instance()->toggle_show();
	}

	ImGui::Separator();


	if (ImGui::Button(vals_.bw ? COLOR_LABEL : BW_LABEL))
	{
		vals_.bw = !vals_.bw;
		slider_changed_ |= true;
	}
	ImGui::SameLine();
	slider_changed_ |= ImGui::Checkbox("Invert", &vals_.inv);

	slider_changed_ |= slider_float_reset(vals_.wb, 7200.0f, "White Balance", &vals_.wb, 1667, 25000, "%.0f", 0,
	                                      ImColor(242, 126, 36), ImColor(36, 173, 242));

	slider_changed_ |= slider_float_reset(vals_.tint, 1.0f, "Tint", &(vals_.tint), 0.0f, 2.0f, "%.2f", 0,
	                                      ImColor(0, 255, 0), ImColor(255, 0, 255));


	if (ImGui::BeginTabBar("MyTabBar"))
	{
		for (int n = 0; n < 4; n++)
			if (ImGui::BeginTabItem(tab_names_[n]))
			{
				slider_changed_ |= slider_float_reset(vals_.expo[n], 0.0f, "Exposure", &(vals_.expo[n]), -2, 2);
				slider_changed_ |= slider_float_reset(vals_.contrast[n], 0.0f, "Contrast", &(vals_.contrast[n]), -1, 1);
				slider_changed_ |= slider_float_reset(vals_.high[n], 0.0f, "High", &(vals_.high[n]), -1, 1);
				slider_changed_ |= slider_float_reset(vals_.mid[n], 0.0f, "Mid", &(vals_.mid[n]), -1, 1);
				slider_changed_ |= slider_float_reset(vals_.whites[n], 0.0f, "Whites", &(vals_.whites[n]), -1, 1);
				slider_changed_ |= slider_float_reset(vals_.low[n], 0.0f, "Low", &(vals_.low[n]), -1, 1);

				ImGui::Separator();

				slider_changed_ |= slider_float_reset(vals_.lift[n], 0.0f, "Lift", &(vals_.lift[n]), -1, 1);
				slider_changed_ |= slider_float_reset(vals_.gamma[n], 1.0f, "Gamma", &(vals_.gamma[n]), 0, 2);
				slider_changed_ |= slider_float_reset(vals_.gain[n], 1.0f, "Gain", &(vals_.gain[n]), -1, 2);
				slider_changed_ |= slider_float_reset(vals_.offset[n], 1.0f, "Offset", &(vals_.offset[n]), -1, 2);


				ImGui::EndTabItem();
			}
		ImGui::EndTabBar();
	}

	ImGui::Separator();

	slider_changed_ |= slider_float_reset(vals_.sat_ref, 1.0f, "Saturation Refine", &(vals_.sat_ref), 0, 1);

	ImGui::Separator();

	slider_changed_ |= draw_color_selector("High", ImGui::GetWindowWidth() / 3, &vals_.high[1], &vals_.high[2],
	                                     &vals_.high[3]);
	ImGui::SameLine();
	slider_changed_ |= draw_color_selector("Mid", ImGui::GetWindowWidth() / 3, &vals_.mid[1], &vals_.mid[2],
	                                     &vals_.mid[3]);
	slider_changed_ |= draw_color_selector("Low", ImGui::GetWindowWidth() / 3, &vals_.low[1], &vals_.low[2],
	                                     &vals_.low[3]);
	ImGui::SameLine();

	slider_changed_ |= draw_color_selector("Lift", ImGui::GetWindowWidth() / 3, &vals_.lift[1], &vals_.lift[2],
	                                     &vals_.lift[3]);
	slider_changed_ |= draw_color_selector("Gamma", ImGui::GetWindowWidth() / 3, &vals_.gamma[1], &vals_.gamma[2],
	                                     &vals_.gamma[3], false);
	ImGui::SameLine();
	slider_changed_ |= draw_color_selector("Gain", ImGui::GetWindowWidth() / 3, &vals_.gain[1], &vals_.gain[2],
	                                     &vals_.gain[3], false, 0);

	slider_changed_ |= draw_color_selector("Offset", ImGui::GetWindowWidth() / 3, &vals_.offset[1], &vals_.offset[2],
	                                     &vals_.offset[3], false, 0);

	ImGui::Separator();

	slider_changed_ |= slider_float_reset(vals_.sat, 0.0f, "Saturation", &vals_.sat, -1, 1);

	slider_changed_ |= slider_float_reset(vals_.blur, 1.0f, "Sharpen: Blur", &vals_.blur, 0, 5);
	slider_changed_ |= slider_float_reset(vals_.sharp, 0.5f, "Sharpen: Sharp", &vals_.sharp, 0, 5);
	if (vals_.sharp != vals_.p_sharp || vals_.blur != vals_.p_blur)
	{
		update_sharpen_kernel();
		vals_.p_sharp = vals_.sharp;
		vals_.p_blur = vals_.blur;
	}
	ImGui::Separator();

	ImGui::Checkbox("Noise", &vals_.noise_selected);
	if (vals_.noise_selected)
	{
		slider_changed_ |= slider_float_reset(vals_.noise, 0.0f, "Noise", &vals_.noise, 0, 5);
	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("HSL Editing"))
	{
		if (ImGui::CollapsingHeader("Red"))
		{
			slider_changed_ |= slider_float_reset(vals_.hues[0], 0.0f, "Red Hue", &vals_.hues[0], -1, 1, "%.2f", 0,
			                                      ImColor(224, 27, 89), ImColor(219, 106, 35));
			slider_changed_ |= slider_float_reset(vals_.sats[0], 0.0f, "Red Saturation", &vals_.sats[0], -1, 1, "%.2f",
			                                      0,
			                                      ImColor(255, 255, 255), ImColor(227, 48, 48));
			slider_changed_ |= slider_float_reset(vals_.lums[0], 0.0f, "Red Value", &vals_.lums[0], -1, 1);
		}
		ImGui::Separator();


		if (ImGui::CollapsingHeader("Orange"))
		{
			slider_changed_ |= slider_float_reset(vals_.hues[1], 0.0f, "Orange Hue", &vals_.hues[1], -1, 1, "%.2f", 0,
			                                      ImColor(224, 55, 25), ImColor(224, 171, 25));
			slider_changed_ |= slider_float_reset(vals_.sats[1], 0.0f, "Orange Saturation", &vals_.sats[1], -1, 1,
			                                      "%.2f", 0,
			                                      ImColor(255, 255, 255), ImColor(219, 106, 35));
			slider_changed_ |= slider_float_reset(vals_.lums[1], 0.0f, "Orange Value", &vals_.lums[1], -1, 1);
		}
		ImGui::Separator();


		if (ImGui::CollapsingHeader("Yellow"))
		{
			slider_changed_ |= slider_float_reset(vals_.hues[2], 0.0f, "Yellow Hue", &vals_.hues[2], -1, 1, "%.2f", 0,
			                                      ImColor(227, 140, 34), ImColor(192, 227, 34));
			slider_changed_ |= slider_float_reset(vals_.sats[2], 0.0f, "Yellow Saturation", &vals_.sats[2], -1, 1,
			                                      "%.2f", 0,
			                                      ImColor(255, 255, 255), ImColor(227, 221, 34));
			slider_changed_ |= slider_float_reset(vals_.lums[2], 0.0f, "Yellow Value", &vals_.lums[2], -1, 1);
		}
		ImGui::Separator();


		if (ImGui::CollapsingHeader("Green"))
		{
			slider_changed_ |= slider_float_reset(vals_.hues[3], 0.0f, "Green Hue", &vals_.hues[3], -1, 1, "%.2f", 0,
			                                      ImColor(227, 221, 34), ImColor(34, 227, 118));
			slider_changed_ |= slider_float_reset(vals_.sats[3], 0.0f, "Green Saturation", &vals_.sats[3], -1, 1,
			                                      "%.2f", 0,
			                                      ImColor(255, 255, 255), ImColor(76, 227, 34));
			slider_changed_ |= slider_float_reset(vals_.lums[3], 0.0f, "Green Value", &vals_.lums[3], -1, 1);
		}
		ImGui::Separator();


		if (ImGui::CollapsingHeader("Cyan"))
		{
			slider_changed_ |= slider_float_reset(vals_.hues[4], 0.0f, "Cyan Hue", &vals_.hues[4], -1, 1, "%.2f", 0,
			                                      ImColor(76, 227, 34), ImColor(34, 182, 227));
			slider_changed_ |= slider_float_reset(vals_.sats[4], 0.0f, "Cyan Saturation", &vals_.sats[4], -1, 1, "%.2f",
			                                      0,
			                                      ImColor(255, 255, 255), ImColor(34, 227, 217));
			slider_changed_ |= slider_float_reset(vals_.lums[4], 0.0f, "Cyan Value", &vals_.lums[4], -1, 1);
		}
		ImGui::Separator();


		if (ImGui::CollapsingHeader("Blue"))
		{
			slider_changed_ |= slider_float_reset(vals_.hues[5], 0.0f, "Blue Hue", &vals_.hues[5], -1, 1, "%.2f", 0,
			                                      ImColor(34, 166, 227), ImColor(114, 34, 227));
			slider_changed_ |= slider_float_reset(vals_.sats[5], 0.0f, "Blue Saturation", &vals_.sats[5], -1, 1, "%.2f",
			                                      0,
			                                      ImColor(255, 255, 255), ImColor(34, 40, 227));
			slider_changed_ |= slider_float_reset(vals_.lums[5], 0.0f, "Blue Value", &vals_.lums[5], -1, 1);
		}
		ImGui::Separator();


		if (ImGui::CollapsingHeader("Purple"))
		{
			slider_changed_ |= slider_float_reset(vals_.hues[6], 0.0f, "Purple Hue", &vals_.hues[6], -1, 1, "%.2f", 0,
			                                      ImColor(73, 34, 227), ImColor(185, 34, 227));
			slider_changed_ |= slider_float_reset(vals_.sats[6], 0.0f, "Purple Saturation", &vals_.sats[6], -1, 1,
			                                      "%.2f", 0,
			                                      ImColor(255, 255, 255), ImColor(147, 34, 227));
			slider_changed_ |= slider_float_reset(vals_.lums[6], 0.0f, "Purple Value", &vals_.lums[6], -1, 1);
		}
		ImGui::Separator();


		if (ImGui::CollapsingHeader("Pink"))
		{
			slider_changed_ |= slider_float_reset(vals_.hues[7], 0.0f, "Pink Hue", &vals_.hues[7], -1, 1, "%.2f", 0,
			                                      ImColor(156, 34, 227), ImColor(227, 34, 105));
			slider_changed_ |= slider_float_reset(vals_.sats[7], 0.0f, "Pink Saturation", &vals_.sats[7], -1, 1, "%.2f",
			                                      0,
			                                      ImColor(255, 255, 255), ImColor(227, 34, 150));
			slider_changed_ |= slider_float_reset(vals_.lums[7], 0.0f, "Pink Value", &vals_.lums[7], -1, 1);
		}
	}


	ImGui::Separator();

	slider_changed_ |= slider_float_reset(vals_.yiq_y, 0.0f, "YIQ: Blue/Orange", &vals_.yiq_y, -1, 1, "%.2f", 0,
	                                      ImColor(34, 40, 227), ImColor(219, 106, 35));
	slider_changed_ |= slider_float_reset(vals_.yiq_z, 0.0f, "YIQ: Green/Purple", &vals_.yiq_z, -1, 1, "%.2f", 0,
	                                      ImColor(76, 227, 34), ImColor(147, 34, 227));
	slider_changed_ |= slider_float_reset(vals_.xyz_y, 0.0f, "XYZ: Purple/Green", &vals_.xyz_y, -1, 1, "%.2f", 0,
	                                      ImColor(147, 34, 227), ImColor(76, 227, 34));
	slider_changed_ |= slider_float_reset(vals_.xyz_z, 0.0f, "XYZ: Orange/Blue", &vals_.xyz_z, -1, 1, "%.2f", 0,
	                                      ImColor(219, 106, 35), ImColor(34, 40, 227));

	ImGui::Separator();

	slider_changed_ |= slider_float_reset(vals_.scope_brightness, 2.0f, "Scope Brightness", &vals_.scope_brightness, 0,
	                                      10);

	if (ImGui::IsWindowFocused() && ImGui::IsMouseReleased(0) && !new_values_set_)
	{
		new_values_set_ = true;
		vals_.show_low_res = false;
	}

	ImGui::End();

	if (slider_changed_)
	{
		set_changes();
	}

	if (!ini_writing_ || !ini_reading_)
	{
		if (ini_writer_.joinable())
		{
			ini_writer_.join();
			Console::log("iniWriter joined");
		}
	}

	btn_id = 0;
}

void Editor::write_ini()
{
	ini_writing_ = true;


	if (s_write(vals_, catalog::instance()->get_current_item()->hconf_location, sizeof(SliderValues)) < 0)
	{
		Console::log("Failed to write .hconf file to %s", catalog::instance()->get_current_item()->hconf_location);
		ini_writing_ = false;
		return;
	}

	Status::set_status("Saved!");
	Console::log("Written to .hconf file %s", catalog::instance()->get_current_item()->hconf_location);

	ini_writing_ = false;
}

void Editor::read_ini()
{
	ini_reading_ = true;

	SliderValues n_vals{};

	if (s_read<SliderValues>(n_vals, catalog::instance()->get_current_item()->hconf_location) < 0)
	{
		Console::log("Failed to read .hconf file at %s", catalog::instance()->get_current_item()->hconf_location);
		Status::set_status("Read settings unsuccessfully :(");
		ini_reading_ = false;
		return;
	}

	vals_ = n_vals;

	ini_reading_ = false;
	Status::set_status("Read settings successfully!");
	prev_vals_ = vals_;
	set_changes();
}


void Editor::update_config_file()
{
	if (!im_loaded_ || ini_reading_ || ini_writing_)
		return;
	ini_writer_ = std::thread(&Editor::write_ini, this);
}

void Editor::undo()
{
	vals_ = prev_vals_;
	set_changes();
}

void Editor::set_from_config_file()
{
	if (ini_writing_ || ini_reading_)
		return;
	ini_writer_ = std::thread(&Editor::read_ini, this);
}


void Editor::reset()
{
	vals_.bw_label = BW_LABEL;

	vals_.show_low_res = false;

	vals_.bw = false;
	vals_.inv = false;

	std::fill_n(vals_.low, 4,LOW_DEFAULT);
	std::fill_n(vals_.mid, 4,MID_DEFAULT);
	std::fill_n(vals_.high, 4,HIGH_DEFAULT);
	std::fill_n(vals_.expo, 4,EXP_DEFAULT);
	std::fill_n(vals_.contrast, 4,CONTRAST_DEFAULT);
	std::fill_n(vals_.whites, 4,WHITE_DEFAULT);

	std::fill_n(vals_.lift, 4, 0.0f);
	std::fill_n(vals_.gamma, 4, 1.0f);
	std::fill_n(vals_.gain, 4, 1.0f);
	std::fill_n(vals_.offset, 4, 1.0f);

	std::fill_n(vals_.hues, 8, 0.0f);
	std::fill_n(vals_.sats, 8, 0.0f);
	std::fill_n(vals_.lums, 8, 0.0f);

	vals_.sat_ref = 1;

	vals_.sat = SAT_DEFAULT;
	vals_.wb = WB_DEFAULT;
	vals_.tint = 1.0f;

	vals_.noise_selected = false;
	vals_.noise = 0;

	vals_.sharp = SHARP_DEFAULT;
	vals_.p_sharp = SHARP_DEFAULT; // Prev sharp
	vals_.blur = BLUR_DEFAULT;
	vals_.p_blur = BLUR_DEFAULT; // Prev blur


	vals_.yiq_y = 0;
	vals_.yiq_z = 0;
	vals_.xyz_y = 0;
	vals_.xyz_z = 0;

	vals_.scope_brightness = 2;

	set_changes();
}
