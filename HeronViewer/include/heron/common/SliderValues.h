#pragma once
struct SliderValues
{
	const char* bw_label;
	bool bw;
	bool inv;

	float low[4];
	float mid[4];
	float high[4];
	float expo[4];
	float contrast[4];
	float whites[4];

	float lift[4];
	float gamma[4];
	float gain[4];

	float sat;
	float wb;

	bool noise_selected;
	float noise;

	float sharp_kernel[9];
	float sharp;
	float p_sharp;
	float blur;
	float p_blur;

	float hues[8];
	float sats[8];
	float lums[8];

	float yiq_y;
	float yiq_z;
	float xyz_y;
	float xyz_z;

	float scope_brightness;
};