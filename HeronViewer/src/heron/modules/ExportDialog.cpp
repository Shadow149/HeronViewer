#include "ExportDialog.h"

#include "PreferencesDialog.h"
#include "Utils.h"
#include "Widgets.h"

ExportDialog* ExportDialog::export_;

ExportDialog* ExportDialog::instance()
{
	if (!export_)
		export_ = new ExportDialog();
	return export_;
}

void ExportDialog::init()
{
}

void ExportDialog::export_image(const std::string file_name, const export_data export_data)
{
	std::experimental::filesystem::path dir = PreferencesDialog::instance()->EXPORT_DIR;
	dir /= file_name;
	export_dir_ = dir.u8string();
	p_img_->export_image(export_dir_.c_str(), export_data);
}

void ExportDialog::set_file_name(const std::string& file_name)
{
	file_name_jpg_ = strip_extension(file_name) + "-edit.jpg";
	file_name_png_ = strip_extension(file_name) + "-edit.png";
}

bool ExportDialog::export_button(const char* label) const
{
	if (p_img_->is_exporting())
	{
		ImGui::Button("Exporting...");
		Status::set_status("Exporting...");
		ImGui::SameLine();
		spinner("EXPORTING...", ImGui::CalcTextSize("Exporting...").y / 2, 3,
		        ImGui::GetColorU32(ImVec4(255, 255, 255, 255)));
	}
	else
	{
		if (ImGui::Button(label)) return true;
		return false;
	}
	return false;
}

void ExportDialog::render()
{
	if (ImGui::Begin(name.c_str()))
	{
		if (ImGui::BeginTabBar("ExportTab"))
		{
			if (ImGui::BeginTabItem("JPG")) {
				ImGui::InputText("JPG Export Name", &file_name_jpg_);
				ImGui::SliderInt("Quality##000", &jpg_quality_, 0, 100);
				if (export_button("Export to JPG")) export_image(file_name_jpg_, { EXPORT_JPEG, jpg_quality_});
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("PNG")) {
				ImGui::InputText("PNG Export Name", &file_name_png_);
				if (export_button("Export to PNG")) export_image(file_name_png_, { EXPORT_PNG, 0 });
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

void ExportDialog::cleanup()
{}


