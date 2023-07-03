#pragma once
#include <utility>
#include <vector>

#include "Module.h"
#include "imnodes.h"

class Graph :
    public Module
{
public:
	explicit Graph(std::string n) : Module(std::move(n), false) {}
	void init() override {};
	void render() override;
	void cleanup() override {};
private:
	int num_nodes_ = 1;
	std::vector<std::pair<int, int>> links_;
};

