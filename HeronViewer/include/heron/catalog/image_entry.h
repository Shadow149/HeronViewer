#pragma once
#include "cat_item.h"

class image_entry {
public:
	image_entry(cat_item item);
	void set_loaded();
	void unload();
	bool is_loaded() const;
public:
	cat_item data;
private:
	bool _is_loaded;
};
