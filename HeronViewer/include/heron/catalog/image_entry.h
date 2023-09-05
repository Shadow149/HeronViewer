#pragma once
#include "cat_item.h"

class image_entry {
public:
	image_entry(cat_item item);
	void set_loaded();
	void unload();
	bool is_loaded() const;

	void preview_outdated();
	bool preview_updated();
	bool preview_needs_updating() const;
public:
	cat_item data;
private:
	bool _is_loaded;
};
