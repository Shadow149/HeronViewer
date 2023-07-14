#pragma once

class renderable
{
public:
	virtual void init() = 0;
	virtual void render() = 0;
	virtual void cleanup() = 0;
};