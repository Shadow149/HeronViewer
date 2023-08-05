#include "catalog.h"

#include <limits>

#include "Console.h"
#include "serialise.h"
#include "Status.h"
#include "Utils.h"

catalog* catalog::instance_;

// Returns 0 as success, -1 for fail
s_error_t serialize_catalog(const std::map<std::size_t, cat_item>& data, const char* location)
{
	auto* buffer = static_cast<char*>(malloc(sizeof(cat_item) * data.size()));

	std::ofstream fd;
	fd.open(location, std::ios::binary);
	if (fd.fail()) {
		printf("Error opening settings file for writing\n");
		free(buffer);
		return -1;
	}
	printf("Writing to file: %s\n", location);

	int i = 0;
	for (auto& item : data) {
		printf("writing an item to catalog\n");
		memcpy(buffer + (i++ * sizeof(cat_item)), &item.second, sizeof(cat_item));
	}
	fd.write(buffer, sizeof(cat_item) * data.size());

	if (fd.fail()) {
		printf("Error writing serialised file\n");
		free(buffer);
		return -1;
	}
	printf("Serialised file written\n");
	free(buffer);
	return 0;
}

s_error_t read_serialize_catalog(std::map<std::size_t, cat_item>& data, const char* location)
{

	std::ifstream fd;
	fd.open(location, std::ios::binary);
	if (fd.fail()) {
		printf("Error opening serialised file for reading\n");
		return -1;
	}
	printf("Reading hconf file: %s\n", location);


	fd.ignore( std::numeric_limits<std::streamsize>::max() );
	std::streamsize pos = fd.gcount();
	fd.clear();   //  Since ignore will have set eof.
	int length = pos;
	std::vector<char> buffer(length);

    fd.seekg(0, std::ios::beg);
    fd.read(buffer.data(), length);

	for (int i = 0; i < length / sizeof(cat_item); i ++) {
		printf("Getting catalog item\n");
		char item_buffer[sizeof(cat_item)];
		memcpy(item_buffer, buffer.data() + i * sizeof(cat_item), sizeof(cat_item));
		const cat_item item = *reinterpret_cast<cat_item*>(item_buffer);
		std::size_t hash = catalog::calc_item_hash(item);
		data[hash] = item;
	}
	printf("Serialised file read\n");
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

std::size_t catalog::calc_item_hash(const cat_item item)
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

std::map<std::size_t, cat_item> catalog::get()
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

