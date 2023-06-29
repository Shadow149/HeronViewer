#pragma once
#include "Module.h"
class Image;

class Waveform :
    public Module
{
public:
	Waveform(Image* i, std::string n, bool v = true) : Module(n, v) {
		img = i;
	};
	void init();
	void render();
	void cleanup();
private:
	Image* img;
};

