#pragma once
#include <string>

class Status
{
public:
	static void set_status(const std::string& status);
	static std::string get_status();

private:
	static std::string status_text_;
};
