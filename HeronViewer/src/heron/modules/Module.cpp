#include "Module.h"

void Module::setStyle()
{
	ImGui::GetStyle().WindowMinSize = ImVec2(100, 100);
}

void Module::updateMouseInWindow()
{
	ImVec2 size = ImGui::GetWindowSize();
	ImVec2 mouse_pos = ImGui::GetMousePos();
	ImVec2 win_pos = ImGui::GetWindowPos();
	if (mouse_pos.x < (win_pos.x + size.x) && mouse_pos.y < (win_pos.y + size.y) &&
		mouse_pos.x > (win_pos.x) && mouse_pos.y > (win_pos.y) && ImGui::IsWindowFocused()) {
		mouseInWindow = true;
	}
	else {
		mouseInWindow = false;
	}
}
