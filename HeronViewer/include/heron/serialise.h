#pragma once

#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <fstream>

#include "Console.h"
#include "Utils.h"

typedef int s_error_t;

// Returns 0 as success, -1 for fail
inline s_error_t s_prev_write(GLfloat* buffer, const char* location, const unsigned size)
{
	std::ofstream fd;
	fd.open(location, std::ios::binary);
	if (fd.fail()) {
		Console::log("Error opening settings file for writing");
		return -1;
	}
	//Console::log("Writing to file: %s", location);
	fd.write((char*)buffer, size);

	if (fd.fail()) {
		Console::log("Error writing serialised file");
		return -1;
	}
	//Console::log("Serialised file written");
	return 0;
}

// Returns 0 as success, -1 for fail
inline s_error_t s_prev_read(GLfloat* buffer, const char* location, const unsigned size)
{
	std::ifstream fd;
	fd.open(location, std::ios::binary);
	if (fd.fail()) {
		Console::log("Error opening serialised file for reading");
		return -1;
	}
	//Console::log("Reading hprev file: %s", location);
	fd.read((char*)buffer, size);

	if (fd.fail()) {
		Console::log("Error reading serialised file");
		return -1;
	}
	//Console::log("Serialised file read");
	return 0;
}



// Returns 0 as success, -1 for fail
template<typename T>
inline s_error_t s_write(T& data, const char* location, const unsigned size)
{
	auto* buffer = (char*)malloc(size);

	std::ofstream fd;
	fd.open(location, std::ios::binary);
	if (fd.fail()) {
		Console::log("Error opening settings file for writing");
		free(buffer);
		return -1;
	}
	Console::log("Writing to file: %s", location);
	memcpy(buffer, &data, size);
	fd.write(buffer, size);

	if (fd.fail()) {
		Console::log("Error writing serialised file");
		free(buffer);
		return -1;
	}
	Console::log("Serialised file written");
	free(buffer);
	return 0;
}


// Returns nullptr as error
template<typename T>
inline s_error_t s_read(T& data, const char* location)
{

	std::ifstream fd;
	fd.open(location, std::ios::binary);
	if (fd.fail()) {
		Console::log("Error opening serialised file for reading");
		return -1;
	}
	Console::log("Reading hconf file: %s", location);
	char buffer[sizeof(T)];
	fd.read(buffer, sizeof(buffer));
	Console::log("Serialised file read");
	data = *reinterpret_cast<T*>(buffer);
	return 0;
}
