#include "Module.h"

void Module::set_style()
{
	ImGui::GetStyle().WindowMinSize = ImVec2(100, 100);
}

void Module::update_mouse_in_window()
{
	const ImVec2 size = ImGui::GetWindowSize();
	const ImVec2 mouse_pos = ImGui::GetMousePos();
	const ImVec2 win_pos = ImGui::GetWindowPos();
	if (mouse_pos.x < (win_pos.x + size.x) && mouse_pos.y < (win_pos.y + size.y) &&
		mouse_pos.x > (win_pos.x) && mouse_pos.y > (win_pos.y) && ImGui::IsWindowFocused())
	{
		mouse_in_window_ = true;
	}
	else
	{
		mouse_in_window_ = false;
	}
}
