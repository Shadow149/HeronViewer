#include "Heron.h"
#pragma comment(lib, "dxgi")

int main() {
	Heron heron(1600, 900);
	if ( (heron.init()) == -1 ) {
		return -1;
	}
	heron.render();
}