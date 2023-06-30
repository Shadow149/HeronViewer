#include "MainPanel.h"


void MainPanel::init()
{
}

void MainPanel::render()
{
	ImGui::Begin("Main", &visible, ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiTabBarFlags_NoTooltip
	);

	ImGui::GetStyle().CellPadding = ImVec2(0, 0);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			
			if (ImGui::BeginMenu("Open Debug")) {
				if (ImGui::MenuItem("Landscape")) {
					((Heron*)hWindow)->loadImage("./images/landscape.png", "landscape.png");
				}
				if (ImGui::MenuItem("Colour Wheel")) {
					((Heron*)hWindow)->loadImage("./images/color-wheel.png", "color-wheel.png");
				}
				ImGui::EndMenu();
			}
			
			
			/*if (ImGui::MenuItem("Open")) {
				ImGuiFileDialog::Instance()->OpenDialog(((Heron*)hWindow)->getFileDialogKey(), "Choose File", "{.CR2,.png,.gif,.jpg,.jpeg}", "C:\\Users\\Alfred Roberts\\Pictures\\");
			}*/

			if (ImGui::MenuItem(Preferences::instance()->name.c_str())) {
				Preferences::instance()->toggleShow();
			}
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Modules"))
		{
			std::vector<Module*> modules = ((Heron*)hWindow)->getModules();
			for (Module* m : modules) {
				if (m->showModuleOption && ImGui::MenuItem(m->name.c_str(), 0, m->visible)) {
					m->toggleShow();
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Image")) {
			if (ImGui::MenuItem("Close Image")) {
				((Heron*)hWindow)->unloadImage();
			}
			ImGui::EndMenu();
		}
		
		ImGui::EndMenuBar();
	}
	
	ImGui::Text(Status::getStatus().c_str());

	ImGui::End();
}

void MainPanel::cleanup()
{
}
