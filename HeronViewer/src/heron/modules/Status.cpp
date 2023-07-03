#include "Status.h"

std::string Status::status_text_;

void Status::set_status(const std::string& status)
{
	status_text_ = status;
}

std::string Status::get_status()
{
	return status_text_;
}
