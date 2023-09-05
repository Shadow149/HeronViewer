#pragma once
#include <map>

#include "image_entry.h"

#define IMAGE_NOT_LOADED -1

class catalog
{
public:
	static catalog* instance();
	std::map<std::size_t, image_entry> get();


    bool add_image(const cat_item item);
	bool remove_image(const std::size_t hash);
	bool remove_image(const image_entry item);
	static std::size_t calc_item_hash(const cat_item item);
	static std::size_t calc_item_hash(const image_entry item);

	int write_catalog() const;

	image_entry* get_current_item();
	cat_item* get_current_item_data();
	bool image_already_exists(const image_entry& item) const;
	bool is_image_loaded() const { return loaded_image_ != IMAGE_NOT_LOADED; }
	void unload_image();
	void load_image(const std::size_t hash);

	void needs_updating() {updated_ = true;}

	image_entry* get_item(const std::size_t key);
	void hconf_to_clipboard(const image_entry* item);
	void paste_hconf_clipboard(image_entry* item);

private:
	catalog();
	int read_catalog();

private:
	static catalog* instance_;
	std::map<std::size_t, image_entry> catalog_map_;
	bool updated_ = false;
	std::size_t loaded_image_ = -1;

	const char* hconf_loc_clipboard_;

	const char* CATALOG_LOCATION = "HeronCatalog/HeronCatalog.hcatalog";
};
