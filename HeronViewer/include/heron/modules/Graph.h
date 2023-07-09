#pragma once
#include <map>
#include <utility>
#include <vector>

#include "Module.h"
#include "imnodes.h"

class link
{
public:
	link(const int from, const int to) : from(from), to(to), id_(link_id_count_++) {}
	int get_id() const { return id_; }
public:
	int from;
	int to;
private:
	int id_;
	static int link_id_count_;
};

class node
{
public:
	virtual ~node() = default;
	explicit node(const ImVec2 pos) :
		start_set_(false),
		ss_pos_(pos),
		id_(id_count_++),
		in_attr_id_(attr_count_++),
		out_attr_id_(attr_count_++),
		in_link_id_(-1)
	{
	}

	virtual void render();
	void set_grid_pos(ImVec2 pos);
	int get_id() const { return id_; }
	int get_in_attr_id() const { return in_attr_id_; }
	int get_in_node_id() const { return in_node_id_; }

	int set_out_link(const int in_attr)
	{
		const auto n_link = new link(out_attr_id_, in_attr);
		out_links_[n_link->get_id()] = n_link;
		return n_link->get_id();
	}
	void remove_out_link(const int id) { out_links_.erase(id); }

	void add_in(const int id, const int node_id)
	{
		in_link_id_ = id;
		in_node_id_ = node_id;
	}
	int remove_in()
	{
		const int tmp = in_link_id_;
		in_link_id_ = -1;
		in_node_id_ = -1;
		return tmp;
	}
	bool has_in_link() const { return in_link_id_ != -1; }

protected:
	void render_links() const;

protected:
	bool start_set_;
	ImVec2 ss_pos_;
	int id_{};
	int in_attr_id_;
	int out_attr_id_;

	std::map<int, link*> out_links_;
	int in_link_id_;
	int in_node_id_;

	static int id_count_;
	static int attr_count_;
};

class start_node final : public node
{
public:
	start_node() : node({ 0,0 }) {}
	void render() override;
};

class end_node final : public node
{
public:
	end_node() : node({ 0,0 }) {}
	void render() override;
};


class Graph :
    public Module
{
public:
	explicit Graph(std::string n, const bool v = false) : Module(std::move(n), v),
		start_pos_set_(false) {}
	void init() override;
	void set_init_pos();
	void render() override;
	void cleanup() override {}
private:
	start_node start_node_;
	node* init_node_;
	end_node end_node_;
	std::map<int, node*> nodes_;
	std::vector<std::pair<int, int>> links_;

	bool start_pos_set_;
};

