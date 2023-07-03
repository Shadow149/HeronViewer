#include "FileDialog.h"
#include "../Heron.h"

std::string escaped(const std::string& input)
{
	std::string output;
	output.reserve(input.size());
	for (const char c : input)
	{
		switch (c)
		{
		case '\a': output += "\\a";
			break;
		case '\b': output += "\\b";
			break;
		case '\f': output += "\\f";
			break;
		case '\n': output += "\\n";
			break;
		case '\r': output += "\\r";
			break;
		case '\t': output += "\\t";
			break;
		case '\v': output += "\\v";
			break;
		case '\\': output += "\\\\";
			break;
		default: output += c;
			break;
		}
	}

	return output;
}

void FileDialog::set_style()
{
	ImGui::GetStyle().WindowMinSize = ImVec2(400, 200);
}


void FileDialog::render()
{
	if (ImGuiFileDialog::Instance()->Display(name))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			const std::string file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();
			const std::string file_path = ImGuiFileDialog::Instance()->GetCurrentPath();
			const std::string file_name = ImGuiFileDialog::Instance()->GetCurrentFileName();
			escape_ = escaped(file_path_name);
			Console::log("Opening: " + escape_);
			heron_window_->set_image_path(file_path);
			heron_window_->load_image(escape_, file_name);
		}
		ImGuiFileDialog::Instance()->Close();
	}
}
