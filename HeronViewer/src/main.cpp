#include "Heron.h"

int main() {
	Heron heron(1600, 900);
	if ( !(heron.init()) ) {
		return -1;
	}
	heron.render();
}