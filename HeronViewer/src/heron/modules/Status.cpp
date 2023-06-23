#include "Status.h"

std::string Status::status_text;

void Status::setStatus(std::string status)
{
	status_text = status;
}

std::string Status::getStatus()
{
	return status_text;
}
