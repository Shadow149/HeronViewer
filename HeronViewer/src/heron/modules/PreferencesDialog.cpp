#include "PreferencesDialog.h"

PreferencesDialog* PreferencesDialog::pref_;

PreferencesDialog* PreferencesDialog::instance()
{
	if (!pref_)
		pref_ = new PreferencesDialog();
	return pref_;
}

void PreferencesDialog::init()
{
	if (!reading_ini_ && !writing_ini_)
		pref_thread_ = std::thread(&PreferencesDialog::read_settings, this);
}

void PreferencesDialog::render()
{
	if (!writing_ini_ && !reading_ini_)
	{
		if (pref_thread_.joinable())
		{
			pref_thread_.join();
			Console::log("iniWriter joined");
		}
	}

	if (!visible) { return; }

	if (ImGui::Begin(name.c_str()))
	{
		ImGui::InputText("Export Directory", &export_dir_str_);

		if (ImGui::Button("Save"))
		{
			save_settings();
		}
	}
	ImGui::End();
}

void PreferencesDialog::save_settings()
{
	EXPORT_DIR = export_dir_str_;

	if (!reading_ini_ && !writing_ini_)
		pref_thread_ = std::thread(&PreferencesDialog::write_settings, this);
}

void PreferencesDialog::read_settings()
{
	reading_ini_ = true;
	const mINI::INIFile file(PREFERENCES_FILE);
	mINI::INIStructure ini;
	const bool read_suc = file.read(ini);

	if (!read_suc)
	{
		Console::log("No preferences ini, will save current settings...");
		write_settings();
		return;
	}
	Console::log("PreferencesDialog ini read...");

	EXPORT_DIR = ini["preferences"]["EXPORT_DIR"];

	write_to_buffers();

	Status::set_status("PreferencesDialog Read");
	reading_ini_ = false;
}

void PreferencesDialog::write_to_buffers()
{
	export_dir_str_ = EXPORT_DIR.u8string();
}


void PreferencesDialog::write_settings()
{
	writing_ini_ = true;
	const mINI::INIFile file(PREFERENCES_FILE);
	mINI::INIStructure ini;
	const bool read_suc = file.read(ini);

	if (!read_suc)
	{
		Console::log("No preferences ini, will create one...");
	}
	else
	{
		Console::log("PreferencesDialog ini read...");
	}

	ini["preferences"]["EXPORT_DIR"] = EXPORT_DIR.u8string();

	if (!read_suc)
	{
		file.generate(ini);
		Console::log("Ini generated");
	}
	else
	{
		file.write(ini);
		Console::log("Ini updated");
	}
	Status::set_status("PreferencesDialog Saved!");
	writing_ini_ = false;
}
