#include "Console.h"

std::string Console::log_str;
int Console::line_pos;

void Console::init()
{
	font = ImGui::GetIO().Fonts->AddFontFromFileTTF("SourceCodePro-Regular.ttf", 14);
}

void Console::render()
{
	if (!visible) { return; }
	ImGui::SetNextWindowBgAlpha(0.50f); // Transparent background
	ImGui::Begin(name.c_str(), &visible);
	ImGui::PushFont(font);
	ImGui::TextWrapped(Console::log_str.c_str());
	ImGui::PopFont();
	ImGui::End();
}

void Console::cleanup()
{
}

void Console::log(std::string log)
{
	Console::log_str += "> " + log + "\n";
	if (line_pos > MAX_LINES) {
		Console::log_str = "";
		line_pos = 0;
	}
}

