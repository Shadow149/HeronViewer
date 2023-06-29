#include "Graph.h"


void Graph::init()
{
}

void Graph::render()
{
	ImGui::Begin(name.c_str(), &visible);

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
			numNodes++;
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();

	for (int i = 0; i < numNodes; i++) {
		ImNodes::BeginNode(i);
		ImNodes::BeginInputAttribute(i * 2);
		ImGui::Text("in");
		ImNodes::EndInputAttribute();
		ImNodes::BeginOutputAttribute(i * 2 + 1);
		ImGui::Text("out");
		ImNodes::EndOutputAttribute();
		ImNodes::EndNode();
	}

	for (int i = 0; i < links.size(); ++i)
	{
		const std::pair<int, int> p = links[i];
		ImNodes::Link(i, p.first, p.second);
	}

	ImNodes::EndNodeEditor();

	int start_attr, end_attr;
	if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
	{
		links.push_back(std::make_pair(start_attr, end_attr));
	}

	ImGui::End();
}

void Graph::cleanup()
{
}
