#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "Heron.h"
#pragma comment(lib, "dxgi")

int main()
{
	_CrtSetBreakAlloc(14962);
	{
		Heron heron(1600, 900);
		if (heron.init() == -1)
		{
			return -1;
		}
		heron.render();
	}

	_CrtDumpMemoryLeaks();
}
