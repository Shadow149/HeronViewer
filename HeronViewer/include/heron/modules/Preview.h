#pragma once
#include "Module.h"

class Image;

class Preview :
	public Module
{
public:
	Preview(Image* i, const std::string n, const bool v = true) : Module(n, v)
	{
		p_img_ = i;
	}

	void init() override {}
	void render() override;
	void cleanup() override {}

private:
	Image* p_img_;
};

