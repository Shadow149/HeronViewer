#pragma once
#include "Module.h"
class Image;

class Vectorscope :
	public Module
{
public:
	Vectorscope(Image* i, const std::string n, const bool v = true) : Module(n, v)
	{
		img_ = i;
	}

	void init() override {}
	void render() override;
	void cleanup() override {}

private:
	Image* img_;
};
