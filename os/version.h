
#ifndef __VERSION_H__
#define __VERSION_H__

#ifdef WIN32
// internal variables
SERVICE_STATUS          ssStatus;       // current status of the service
#endif

//SERVICE_STATUS_HANDLE   sshStatusHandle;
OS_UL                   dwErr = 0;
BOOL                    bDebug = FALSE;
static char*	szVersion = "1.0.0.B";
static char*	szVersionInfo = 
		"Service Name: TestServer\n"
		"Version:      %s-%s\n"
		"Build No.:	   %d\n"
		"Memo:         This is test server for oslib demostratation.\n"
		"Build Date:   2001-8-24\n"
		"Copyright:    NS Ltd. \n";
static char*	szErr[256] = {0};

#define BUILD_NUMBER	0

// Name of the executable
#define SZAPPNAME            "TestServer"

// Internal name of the service
#define SZSERVICENAME        "TestServer"

// Displayed name of the service
#define SZSERVICEDISPLAYNAME "NS Test Server"

// List of service dependencies - "dep1\0dep2\0\0"
#define SZDEPENDENCIES       ""

#endif
