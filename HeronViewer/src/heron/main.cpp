#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>

#ifdef _WIN32
#include <crtdbg.h>
#endif

#include "Heron.h"
#pragma comment(lib, "dxgi")

int main()
{
	//_CrtSetBreakAlloc(14962);
	{
		Heron heron(1600, 900);
		if (heron.init() == -1)
		{
			return -1;
		}
		heron.render();
	}
#ifdef _WIN32
	_CrtDumpMemoryLeaks();
#endif
}
