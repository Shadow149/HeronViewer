#pragma once

#pragma pack(push, 1) // exact fit - no padding
struct SliderValues
{
	bool show_low_res;

	const char* bw_label;
	float sharp_kernel[9];


	float low[4];
	float mid[4];
	float high[4];
	float expo[4];
	float contrast[4];
	float whites[4];

	float lift[4];
	float gamma[4];
	float gain[4];
	float offset[4];

	ImVec2 hue_hue[10];
	ImVec2 hue_sat[10];
	ImVec2 hue_lum[10];

	ImVec2 sat_hue[10];
	ImVec2 sat_sat[10];
	ImVec2 sat_lum[10];

	ImVec2 lum_hue[10];
	ImVec2 lum_sat[10];
	ImVec2 lum_lum[10];

	bool bw;
	bool inv;

	float sat;
	float wb;
	float tint;

	bool noise_selected;
	float noise;

	float dn_sigma;
	float dn_ksigma;
	float dn_thresh;

	float sharp;
	float p_sharp;
	float blur;
	float p_blur;

	float yiq_y;
	float yiq_z;
	float xyz_y;
	float xyz_z;

	float sat_ref;

	float scope_brightness;
};
#pragma pack(pop) // back to whatever the previous packing mode was