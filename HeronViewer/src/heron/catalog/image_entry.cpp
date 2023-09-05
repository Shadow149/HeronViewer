#include "image_entry.h"

image_entry::image_entry(cat_item item) : data(item), _is_loaded(false) {}

void image_entry::set_loaded()
{
	_is_loaded = true; 
}

void image_entry::unload()
{
	_is_loaded = false;
}

bool image_entry::is_loaded() const
{
    return _is_loaded;
}
