#include "Console.h"

char Console::log_str_[MAX_LOG_SIZE];
int Console::line_pos_;

void Console::init()
{
	font_ = ImGui::GetIO().Fonts->AddFontFromFileTTF("./fonts/SourceCodePro-Regular.ttf", 14);
}

void Console::render()
{
	ImGui::SetNextWindowBgAlpha(0.50f); // Transparent background
	ImGui::Begin(name.c_str());
	ImGui::PushFont(font_);
	ImGui::TextWrapped(log_str_);
	ImGui::PopFont();
	ImGui::End();
}

void Console::cleanup()
{
}

void Console::log(const char* format, ...)
{
	char buffer[BUFFER_MAX];
	va_list list;
	va_start(list, format);
	if (vsprintf_s(buffer, BUFFER_MAX, format, list) < 0)
	{
		printf("Failed to log correctly\n");
		return;
	}

	strcat_s(log_str_, "> ");
	strcat_s(log_str_, buffer);
	strcat_s(log_str_, "\n");

	if (line_pos_ > MAX_LINES)
	{
		memset(log_str_, 0, MAX_LOG_SIZE);
		line_pos_ = 0;
	}
}
