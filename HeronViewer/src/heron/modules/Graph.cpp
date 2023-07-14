#include "Graph.h"

int node::id_count_ = 0;
int node::attr_count_ = 0;
int link::link_id_count_ = 0;

void node::render()
{
	ImNodes::BeginNode(id_);
	if (!start_set_) {
		ImNodes::SetNodeScreenSpacePos(id_, ss_pos_);
		start_set_ = true;
	}
	ImNodes::BeginNodeTitleBar();
	ImGui::Text(("Node " + std::to_string(id_)).c_str());
	ImNodes::EndNodeTitleBar();
	ImNodes::BeginInputAttribute(in_attr_id_);
	ImGui::Text(("in " + std::to_string(in_attr_id_)).c_str());
	ImNodes::EndInputAttribute();
	ImNodes::BeginOutputAttribute(out_attr_id_);
	ImGui::Text(("out " + std::to_string(out_attr_id_)).c_str());
	ImNodes::EndOutputAttribute();
	ImNodes::EndNode();
	render_links();
}

void node::set_grid_pos(const ImVec2 pos)
{
	ImNodes::SetNodeGridSpacePos(id_, pos);
	start_set_ = true;
}

void node::render_links() const
{
	for (const auto& n_link : out_links_) {
		ImNodes::Link(n_link.second->get_id(), out_attr_id_, n_link.second->to);
	}
}

void start_node::render()
{
	ImNodes::BeginNode(id_);
	ImNodes::BeginNodeTitleBar();
	ImGui::Text(("Input Image " + std::to_string(id_)).c_str());
	ImNodes::EndNodeTitleBar();
	ImNodes::BeginOutputAttribute(out_attr_id_);
	ImGui::Text(("out " + std::to_string(out_attr_id_)).c_str());
	ImNodes::EndOutputAttribute();
	ImNodes::EndNode();
	render_links();
}

void end_node::render()
{
	ImNodes::BeginNode(id_);
	ImNodes::BeginNodeTitleBar();
	ImGui::Text(("Output " + std::to_string(id_)).c_str());
	ImNodes::EndNodeTitleBar();
	ImNodes::BeginInputAttribute(in_attr_id_);
	ImGui::Text(("in " + std::to_string(in_attr_id_)).c_str());
	ImNodes::EndInputAttribute();
	ImNodes::EndNode();
	render_links();
}

void Graph::init()
{
	nodes_[start_node_.get_id()] = &start_node_;
	init_node_ = new node({ 0,0 });
	nodes_[init_node_->get_id()] = init_node_;
	nodes_[end_node_.get_id()] = &end_node_;

	int link_id = start_node_.set_out_link(init_node_->get_in_attr_id());
	init_node_->add_in(link_id, start_node_.get_id());
	link_id = init_node_->set_out_link(end_node_.get_in_attr_id());
	end_node_.add_in(link_id, init_node_->get_id());
}

void Graph::set_init_pos()
{
	start_node_.set_grid_pos({ ImGui::GetWindowHeight() / 2.0f - 200, ImGui::GetWindowHeight() / 2.0f });
	init_node_->set_grid_pos({ ImGui::GetWindowHeight() / 2.0f, ImGui::GetWindowHeight() / 2.0f });
	end_node_.set_grid_pos({ ImGui::GetWindowHeight() / 2.0f + 200, ImGui::GetWindowHeight() / 2.0f });
}

void Graph::render()
{
	ImGui::Begin(name.c_str());

	if (!start_pos_set_) {
		set_init_pos();
		start_pos_set_ = true;
	}

	ImNodes::BeginNodeEditor();

	const bool open_popup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
		ImNodes::IsEditorHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
	if (!ImGui::IsAnyItemHovered() && open_popup)
	{
		ImGui::OpenPopup("add node");
	}

	if (ImGui::BeginPopup("add node"))
	{
		if (ImGui::MenuItem("add"))
		{
			auto const n = new node(ImGui::GetMousePos());
			nodes_[n->get_id()] = n;
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
	

	for (const auto& node : nodes_)
	{
		node.second->render();
	}

	ImNodes::EndNodeEditor();

	int start_id, end_id, start_attr, end_attr;
	if (ImNodes::IsLinkCreated(&start_id, &start_attr, 
		&end_id, &end_attr))
	{
		node* end = nodes_[end_id];
		if (end->has_in_link())
			nodes_[end->get_in_node_id()]->remove_out_link(end->remove_in());
		const int link_id = nodes_[start_id]->set_out_link(end->get_in_attr_id());
		end->add_in(link_id, start_id);
		printf("Start %d to End %d -- Link created: %d -> %d\n",start_id, end_id, start_attr, end_attr);
	}

	ImGui::End();
}
