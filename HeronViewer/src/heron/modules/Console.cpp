#include "Console.h"

std::string Console::log_str;
int Console::line_pos;

void Console::init()
{
	font_ = ImGui::GetIO().Fonts->AddFontFromFileTTF("./fonts/SourceCodePro-Regular.ttf", 14);
}

void Console::render()
{
	ImGui::SetNextWindowBgAlpha(0.50f); // Transparent background
	ImGui::Begin(name.c_str(), &visible);
	ImGui::PushFont(font_);
	ImGui::TextWrapped(log_str.c_str());
	ImGui::PopFont();
	ImGui::End();
}

void Console::cleanup()
{
}

void Console::log(const std::string& log)
{
	log_str += "> " + log + "\n";
	if (line_pos > MAX_LINES)
	{
		log_str = "";
		line_pos = 0;
	}
}
