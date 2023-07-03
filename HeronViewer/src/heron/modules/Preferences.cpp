#include "Preferences.h"

Preferences* Preferences::pref_;

Preferences* Preferences::instance()
{
	if (!pref_)
		pref_ = new Preferences();
	return pref_;
}

void Preferences::init()
{
	if (!reading_ini_ && !writing_ini_)
		pref_thread_ = std::thread(&Preferences::read_settings, this);
}

void Preferences::render()
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

	if (ImGui::Begin(name.c_str(), &visible))
	{
		ImGui::InputText("Export Directory", &export_dir_str_);

		if (ImGui::Button("Save"))
		{
			save_settings();
		}
	}
	ImGui::End();
}

void Preferences::save_settings()
{
	EXPORT_DIR = export_dir_str_;

	if (!reading_ini_ && !writing_ini_)
		pref_thread_ = std::thread(&Preferences::write_settings, this);
}

void Preferences::read_settings()
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
	Console::log("Preferences ini read...");

	EXPORT_DIR = ini["preferences"]["EXPORT_DIR"];

	write_to_buffers();

	Status::set_status("Preferences Read");
	reading_ini_ = false;
}

void Preferences::write_to_buffers()
{
	export_dir_str_ = EXPORT_DIR.u8string();
}


void Preferences::write_settings()
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
		Console::log("Preferences ini read...");
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
	Status::set_status("Preferences Saved!");
	writing_ini_ = false;
}
