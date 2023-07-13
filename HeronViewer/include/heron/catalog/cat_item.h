#pragma once
#include "glad/glad.h"

#pragma pack(push, 1) // exact fit - no padding
struct cat_item
{
	char file_name[255];
	char file_location[255]; // including file name (full path)

	char hprev_location[255]; // preview image location (.hprev)
	GLsizei hprev_width;
	GLsizei hprev_height;

	char hconf_location[255]; // image config locatoin (.hconf)

	// Scope caches
	//char vectorscope[255 * 255];
	//char waveform[512 * 255];
	//unsigned hist[255];
};
#pragma pack(pop) // back to whatever the previous packing mode was
