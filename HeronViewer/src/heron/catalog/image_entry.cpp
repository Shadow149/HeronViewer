#include "image_entry.h"

image_entry::image_entry(cat_item item) : 
	data(item), 
	_is_loaded(false) {}

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

void image_entry::preview_outdated() {
	data.hprev_needs_updating = true;
}

bool image_entry::preview_updated() {
	bool changed = data.hprev_needs_updating;
	data.hprev_needs_updating = false;
	return changed;
}

bool image_entry::preview_needs_updating() const {
	return data.hprev_needs_updating;
}
