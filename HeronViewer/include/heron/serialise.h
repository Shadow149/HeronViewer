#pragma once
#include <corecrt_io.h>
#include <cstdlib>
#include <fcntl.h>

#include "Console.h"
#include "Utils.h"

typedef int s_error_t;

// Returns 0 as success, -1 for fail
inline s_error_t s_prev_write(GLfloat* buffer, const char* location, const unsigned size)
{
	int fd;
	const errno_t err = _sopen_s(&fd, location, _O_WRONLY | O_BINARY | _O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (fd < 0) {
		Console::log("Error opening settings file for writing");
		return -1;
	}
	//Console::log("Writing to file: %s", location);
	const int w_err = _write(fd, buffer, size);

	if (w_err < 0) {
		Console::log("Error writing serialised file");
		return -1;
	}
	//Console::log("Serialised file written");
	return 0;
}

// Returns 0 as success, -1 for fail
inline s_error_t s_prev_read(GLfloat* buffer, const char* location, const unsigned size)
{
	int fd;
	errno_t err = _sopen_s(&fd, location, O_RDONLY | O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (fd < 0) {
		Console::log("Error opening serialised file for reading");
		return -1;
	}
	//Console::log("Reading hprev file: %s", location);
	_read(fd, buffer, size);
	//Console::log("Serialised file read");
	return 0;
}



// Returns 0 as success, -1 for fail
template<typename T>
inline s_error_t s_write(T& data, const char* location, const unsigned size)
{
	auto* buffer = (char*)malloc(size);

	int fd;
	const errno_t err = _sopen_s(&fd, location, _O_WRONLY | _O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (fd < 0) {
		Console::log("Error opening settings file for writing");
		free(buffer);
		return -1;
	}
	Console::log("Writing to file: %s", location);
	memcpy(buffer, &data, size);
	const int w_err = _write(fd, buffer, size);

	if (w_err < 0) {
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

	int fd;
	errno_t err = _sopen_s(&fd, location, O_RDONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (fd < 0) {
		Console::log("Error opening serialised file for reading");
		return -1;
	}
	Console::log("Reading hconf file: %s", location);
	char buffer[sizeof(T)];
	_read(fd, buffer, sizeof(buffer));
	Console::log("Serialised file read");
	data = *reinterpret_cast<T*>(buffer);
	return 0;
}
