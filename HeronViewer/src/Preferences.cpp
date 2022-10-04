#include "Preferences.h"

Preferences* Preferences::pref;

Preferences* Preferences::instance() {
	if (!pref)
		pref = new Preferences();
	return pref;
}

void Preferences::init()
{
	if (!readingIni && !writingIni)
		prefThread = std::thread(&Preferences::readSettings, this);
}

void Preferences::render()
{
	if (!writingIni && !readingIni) {
		if (prefThread.joinable()) {
			prefThread.join();
			Console::log("iniWriter joined");
		}
	}

    if (!visible) { return; }

    if (ImGui::Begin(name.c_str(), &visible))
    {
        ImGui::InputText("Export Directory", &exportDirStr);

        if (ImGui::Button("Save")) {
            saveSettings();
        }
    }
    ImGui::End();
}

void Preferences::cleanup()
{
}

void Preferences::saveSettings() {
    EXPORT_DIR = exportDirStr;

	if (!readingIni && !writingIni)
		prefThread = std::thread(&Preferences::writeSettings, this);
}

void Preferences::readSettings() {
	readingIni = true;
	mINI::INIFile file(PREFERENCES_FILE);
	mINI::INIStructure ini;
	bool readSuc = file.read(ini);

	if (!readSuc) {
		Console::log("No preferences ini, will save current settings...");
		writeSettings();
		return;
	}
	else {
		Console::log("Preferences ini read...");
	}

	EXPORT_DIR = ini["preferences"]["EXPORT_DIR"];

	writeToBuffers();

	Status::setStatus("Preferences Read");
	readingIni = false;
}

void Preferences::writeToBuffers() {
	exportDirStr = EXPORT_DIR.u8string();
}


void Preferences::writeSettings() {
	writingIni = true;
	mINI::INIFile file(PREFERENCES_FILE);
	mINI::INIStructure ini;
	bool readSuc = file.read(ini);

	if (!readSuc) {
		Console::log("No preferences ini, will create one...");
	}
	else {
		Console::log("Preferences ini read...");
	}

	ini["preferences"]["EXPORT_DIR"] = EXPORT_DIR.u8string();

	if (!readSuc) {
		file.generate(ini);
		Console::log("Ini generated");
	}
	else {
		file.write(ini);
		Console::log("Ini updated");
	}
	Status::setStatus("Preferences Saved!");
	writingIni = false;
}
