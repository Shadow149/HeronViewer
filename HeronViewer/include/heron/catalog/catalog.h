#pragma once
#include <map>

#include "cat_item.h"


class catalog
{
public:
	static catalog* instance();
	bool add_image(cat_item item);
	int write_catalog() const;
	std::map<size_t, cat_item> get();
	cat_item* get_current_item();
	bool image_already_loaded(const cat_item& item) const;
	static size_t calc_item_hash(cat_item item);

private:
	catalog();
	int read_catalog();
private:
	static catalog* instance_;
	std::map<size_t, cat_item> catalog_map_;
	size_t current_item_key_;
	bool updated_ = false;

	const char* CATALOG_LOCATION = "HeronCatalog/HeronCatalog.hcatalog";
};
