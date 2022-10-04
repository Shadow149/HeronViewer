#pragma once
#include <string>
class Status
{
private:
	static std::string status_text;
public:
	static void setStatus(std::string status);
	static std::string getStatus();

};

