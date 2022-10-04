#include "MainPanel.h"


void MainPanel::init()
{
}

void MainPanel::render()
{
	ImGui::Begin("Main", &visible, ImGuiWindowFlags_MenuBar |
		//ImGuiWindowFlags_NoMove |
		//ImGuiWindowFlags_NoResize |
		//ImGuiWindowFlags_NoScrollbar |
		//ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoTitleBar
	);

	ImGui::GetStyle().CellPadding = ImVec2(0, 0);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			/*
			if (ImGui::BeginMenu("Open Recent")) {
				if (ImGui::MenuItem("U75A1167.CR2")) {
					((Heron*)hWindow)->loadImage("C:\\Users\\Alfred Roberts\\Pictures\\U75A1167.CR2");
				}
				if (ImGui::MenuItem("U75A1242.CR2")) {
					((Heron*)hWindow)->loadImage("C:\\Users\\Alfred Roberts\\Pictures\\U75A1242.CR2");
				}
				if (ImGui::MenuItem("U75A1065.CR2")) {
					((Heron*)hWindow)->loadImage("C:\\Users\\Alfred Roberts\\Pictures\\U75A1065.CR2");
				}
				if (ImGui::MenuItem("U75A1083.CR2")) {
					((Heron*)hWindow)->loadImage("C:\\Users\\Alfred Roberts\\Pictures\\U75A1083.CR2");
				}
				if (ImGui::MenuItem("U75A1192.CR2")) {
					((Heron*)hWindow)->loadImage("C:\\Users\\Alfred Roberts\\Pictures\\U75A1192.CR2");
				}
				if (ImGui::MenuItem("U75A1256.CR2")) {
					((Heron*)hWindow)->loadImage("C:\\Users\\Alfred Roberts\\Pictures\\U75A1256.CR2");
				}
				if (ImGui::MenuItem("U75A1295.CR2")) {
					((Heron*)hWindow)->loadImage("C:\\Users\\Alfred Roberts\\Pictures\\U75A1295.CR2");
				}
				if (ImGui::MenuItem("U75A0996.CR2")) {
					((Heron*)hWindow)->loadImage("C:\\Users\\Alfred Roberts\\Pictures\\U75A0996.CR2");
				}
				if (ImGui::MenuItem("U75A1051.CR2")) {
					((Heron*)hWindow)->loadImage("C:\\Users\\Alfred Roberts\\Pictures\\U75A1051.CR2");
				}
				if (ImGui::MenuItem("Isaac-Autumn-storm-Landscape-catagory-scaled.jpg")) {
					((Heron*)hWindow)->loadImage("C:\\Users\\Alfred Roberts\\Pictures\\Isaac-Autumn-storm-Landscape-catagory-scaled.jpg");
				}
				if (ImGui::MenuItem("Isaac-wizard-of-the-woods-Wildlife-catagory-scaled.jpg")) {
					((Heron*)hWindow)->loadImage("C:\\Users\\Alfred Roberts\\Pictures\\Isaac-wizard-of-the-woods-Wildlife-catagory-scaled.jpg");
				}
				ImGui::EndMenu();
			}
			*/
			
			if (ImGui::MenuItem("Open")) {
				ImGuiFileDialog::Instance()->OpenDialog(((Heron*)hWindow)->getFileDialogKey(), "Choose File", "{.CR2,.png,.gif,.jpg,.jpeg}", "C:\\Users\\Alfred Roberts\\Pictures\\");
			}

			//if (ImGui::MenuItem("Save", "Ctrl+S")) {}
			//if (ImGui::MenuItem("Close", "Ctrl+W")) {}
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
