#pragma once
#include "Module.h"
class Image;

class Vectorscope :
    public Module
{
public:
	Vectorscope(Image* i, std::string n, bool v = true) : Module(n, v) {
		img = i;
	};
	void init();
	void render();
	void cleanup();
private:
	Image* img;
};

