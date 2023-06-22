#pragma once
#include <utility>
#include <vector>

#include "Module.h"
#include "imnodes.h"

class Graph :
    public Module
{
public:
	Graph(std::string n) : Module(std::move(n), false) {}
	void init();
	void render();
	void cleanup();
private:
	int numNodes = 1;
	std::vector<std::pair<int, int>> links;
};

