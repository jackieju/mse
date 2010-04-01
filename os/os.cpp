
#include <stdlib.h>

// common include
#include "os.h"

// local include
#include "protected.h"
#include "version.h"

static int iInitilizated = 0;

void OSLibInit(void)
{

	if (iInitilizated ++)
		return;

	// random function initilization
	srand(rand());

	OSThreadInit();
	OSSocketInit();
	OSlib_protected_init();

	// init the directory of executable file
	OSInitCurrentDirectory();

	return;
}

void OSLibShutdown(void)
{

	if (--iInitilizated)
		return;

	OSThreadShutdown();
	OSSocketShutdown();
	OSlib_protected_shutdown();

	return;
}

char *OSGetVersion(void)
{
	static char lszVersion[50];

	snprintf(lszVersion, 50, "%s-%s", szVersion, BUILD_NUMBER);

	return lszVersion;
}