//: ssLogLevel.cpp

#include "osmacros.h"
#include "LogLevel.h"

#define LOGLEVEL_NAME_LEN	20
#define OS_LOG_MAX_LOGLEVEL	101

// global log level string table
static char OSLogLevel[OS_LOG_MAX_LOGLEVEL][LOGLEVEL_NAME_LEN] =
{
"LVL_000","FATAL  ","LVL_002","LVL_003","LVL_004","SYSTEM ","LVL_006","LVL_007","LVL_008","LVL_009",
"ERROR  ","LVL_011","LVL_012","LVL_013","LVL_014","LVL_015","LVL_016","LVL_017","LVL_018","LVL_019",
"LVL_020","LVL_021","LVL_022","LVL_023","LVL_024","LVL_025","LVL_026","LVL_027","LVL_028","LVL_029",
"LVL_030","LVL_031","LVL_032","LVL_033","LVL_034","LVL_035","LVL_036","LVL_037","LVL_038","LVL_039",
"LVL_040","LVL_041","LVL_042","LVL_043","LVL_044","LVL_045","LVL_046","LVL_047","LVL_048","LVL_049",
"WARNING","LVL_051","LVL_052","LVL_053","LVL_054","LVL_055","LVL_056","LVL_057","LVL_058","LVL_059",
"LVL_060","LVL_061","LVL_062","LVL_063","LVL_064","LVL_065","LVL_066","LVL_067","LVL_068","LVL_069",
"LVL_070","LVL_071","LVL_072","LVL_073","LVL_074","LVL_075","LVL_076","LVL_077","LVL_078","LVL_079",
"LVL_080","LVL_081","LVL_082","LVL_083","LVL_084","LVL_085","LVL_086","LVL_087","LVL_088","LVL_089",
"LVL_090","LVL_091","LVL_092","LVL_093","LVL_094","LVL_095","LVL_096","LVL_097","LVL_098","LVL_099",
"DEBUG  "
};

char *OSLOG_GetLogLevelName(int nLogLevel)
{
	if (nLogLevel < 0 || nLogLevel > OS_LOG_MAX_LOGLEVEL - 1)
	{
		return OSLogLevel[OS_LOG_DEBUG];
	}

	return OSLogLevel[nLogLevel];
}


void OSLOG_SetLogLevelName(int nLogLevel, char * sLogLevelName)
{
	if (nLogLevel < 0 || nLogLevel > OS_LOG_MAX_LOGLEVEL - 1)
	{
		return;
	}

	switch(nLogLevel)
	{
	case OS_LOG_FATAL:
	case OS_LOG_SYSTEM:
	case OS_LOG_ERROR:
	case OS_LOG_WARNING:
	case OS_LOG_DEBUG:
		return;
	}

	if (strlen(sLogLevelName) < LOGLEVEL_NAME_LEN)
	{
		strcpy(OSLogLevel[nLogLevel], sLogLevelName);
	}
}