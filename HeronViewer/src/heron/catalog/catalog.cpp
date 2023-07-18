#include "catalog.h"

#include "Console.h"
#include "serialise.h"
#include "Status.h"
#include "Utils.h"

catalog* catalog::instance_;


// Returns 0 as success, -1 for fail
s_error_t serialize_catalog(const std::map<size_t, cat_item>& data, const char* location)
{
	auto* buffer = static_cast<char*>(malloc(sizeof(cat_item) * data.size()));

	int fd;
	const errno_t err = _sopen_s(&fd, location, _O_WRONLY | _O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (fd < 0) {
		Console::log("Error opening settings file for writing");
		free(buffer);
		return -1;
	}
	Console::log("Writing to file: %s", location);

	int i = 0;
	for (auto& item : data) {
		memcpy(buffer + (i++ * sizeof(cat_item)), &item.second, sizeof(cat_item));
	}
	const int w_err = _write(fd, buffer, sizeof(cat_item) * data.size());

	if (w_err < 0) {
		Console::log("Error writing serialised file");
		free(buffer);
		return -1;
	}
	Console::log("Serialised file written");
	free(buffer);
	return 0;
}

s_error_t read_serialize_catalog(std::map<size_t, cat_item>& data, const char* location)
{

	int fd;
	errno_t err = _sopen_s(&fd, location, O_RDONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (fd < 0) {
		Console::log("Error opening serialised file for reading");
		return -1;
	}
	Console::log("Reading hconf file: %s", location);

	char buffer[60000]; // TODO could be too small...

	const int bytes = _read(fd, buffer, sizeof(buffer));
	for (int i = 0; i < bytes / sizeof(cat_item); i ++) {
		char item_buffer[sizeof(cat_item)];
		memcpy(item_buffer, buffer + i * sizeof(cat_item), sizeof(cat_item));
		const cat_item item = *reinterpret_cast<cat_item*>(item_buffer);
		size_t hash = catalog::calc_item_hash(item);
		data[hash] = item;
	}
	Console::log("Serialised file read");
	return 0;
}



catalog* catalog::instance()
{
	if (!instance_)
		instance_ = new catalog();
	return instance_;
}

catalog::catalog()
{
	read_catalog(); // TODO put this on a thread
}

size_t catalog::calc_item_hash(const cat_item item)
{
	return std::hash<std::string>{}(std::string(item.file_location));
}

bool catalog::add_image(const cat_item item)
{
	current_item_key_ = calc_item_hash(item);
	if (catalog_map_.count(calc_item_hash(item))) {
		Console::log("Item in catalog, no need to import...");
		return false;
	}
	updated_ = true;
	catalog_map_[calc_item_hash(item)] = item;
	return true;
}

int catalog::write_catalog() const
{
	if (!updated_) return 0;
	// TODO put this on a thread
	if (serialize_catalog(catalog_map_, CATALOG_LOCATION) < 0)
	{
		Console::log("Failed to write .hcatalog file");
		return -1;
	}

	Status::set_status("Saved catalog!");
	return 0;
}

std::map<size_t, cat_item> catalog::get()
{
	return catalog_map_;
}

cat_item* catalog::get_current_item()
{
	return &catalog_map_[current_item_key_];
}

bool catalog::image_already_loaded(const cat_item& item) const
{
	return calc_item_hash(item) == current_item_key_;
}

int catalog::read_catalog()
{

	if (read_serialize_catalog(catalog_map_, CATALOG_LOCATION) < 0)
	{
		Console::log("Failed to read .hcatalog file at");
		Status::set_status("Read catalog unsuccessfully :(");
		return -1;
	}

	Status::set_status("Read catalog successfully!");
	return 0;
}

