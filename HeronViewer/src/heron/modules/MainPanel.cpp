#include "MainPanel.h"


void MainPanel::init_panel()
{
}

void MainPanel::render_panel()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			
			if (ImGui::BeginMenu("Open Debug")) {
				if (ImGui::MenuItem("Landscape")) {
					dynamic_cast<Heron*>(heron_window_)->load_image("./images/landscape.png", "landscape.png");
				}
				if (ImGui::MenuItem("Colour Wheel")) {
					dynamic_cast<Heron*>(heron_window_)->load_image("./images/color-wheel.png", "color-wheel.png");
				}
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem(PreferencesDialog::instance()->name.c_str())) {
				PreferencesDialog::instance()->toggle_show();
			}
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Modules"))
		{
			const std::vector<Module*> modules = dynamic_cast<Heron*>(heron_window_)->get_modules();
			for (Module* m : modules) {
				if (m->show_module_option && ImGui::MenuItem(m->name.c_str(), nullptr, m->visible)) {
					m->toggle_show();
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Image")) {
			if (ImGui::MenuItem("Close Image")) {
				dynamic_cast<Heron*>(heron_window_)->unload_image();
			}
			ImGui::EndMenu();
		}

		const std::string status_text = Status::get_status() + " \t\t Version: " + HERON_VERSION;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(status_text.c_str()).x);
		// ImGui::SetCursorPosY(ImGui::CalcTextSize(Status::get_status().c_str()).y / 4.0f);
		ImGui::Text(status_text.c_str());
		
		ImGui::EndMenuBar();

	}
}

void MainPanel::cleanup_panel()
{
}
