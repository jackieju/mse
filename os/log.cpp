/*
 * log.cpp - implement logging functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#ifndef WIN32
	#include <errno.h>
	#include <syslog.h>
#else
	#include <windows.h>
#endif

#include "log.h"
//#include "../config.h"
#include "protected.h"
#include "thread.h"

/*
 * List of currently open log files.
 */
#define MAX_LOGFILES 8
static struct {
    FILE *file;
    int minimum_output_level;
    char filename[FILENAME_MAX + 1]; /* to allow re-open */
} logfiles[MAX_LOGFILES];
static int num_logfiles = 0;


/*
 * List of places that should be logged at debug-level.
 */
#define MAX_LOGGABLE_PLACES (10*1000)
static char *loggable_places[MAX_LOGGABLE_PLACES];
static int num_places = 0;


/*
 * Syslog support.
 */
static int sysloglevel;
static int dosyslog = 0;

/*
 * Debug switch support.
 */
static int 	log_g_i_need_debug = 1;

/*
 * Make sure stderr is included in the list.
 */
static void add_stderr(void) 
{
    int i;
    
    for (i = 0; i < num_logfiles; ++i)
	if (logfiles[i].file == stderr)
	    return;
    logfiles[num_logfiles].file = stderr;
    logfiles[num_logfiles].minimum_output_level = OS_DEBUG;
    ++num_logfiles;
}


void set_output_level(enum output_level level) 
{
    int i;
    
    add_stderr();
    for (i = 0; i < num_logfiles; ++i) {
	if (logfiles[i].file == stderr) {
	    logfiles[i].minimum_output_level = level;
	    break;
	}
    }
}

#ifndef WIN32
void set_syslog(const char *ident, int syslog_level) 
{
    if (ident == NULL)
	dosyslog = 0;
    else {
	dosyslog = 1;
	sysloglevel = syslog_level;
	openlog(ident, LOG_PID, LOG_DAEMON);
    }
}
#endif

void reopen_log_files(void) 
{
	int i;
	
    for (i = 0; i < num_logfiles; ++i) {
	if (logfiles[i].file != stderr) {
	    fclose(logfiles[i].file);
	    logfiles[i].file = fopen(logfiles[i].filename, "a");
	    if (logfiles[i].file == NULL) {
		error(errno, "Couldn't re-open logfile `%s'.",
		      logfiles[i].filename);
	    }
	}
    }		
}


void close_all_logfiles(void) 
{
    while (num_logfiles > 0) {
	--num_logfiles;
	if (logfiles[num_logfiles].file != stderr)
	    fclose(logfiles[num_logfiles].file);
	logfiles[num_logfiles].file = NULL;
    }
}


void open_logfile(char *filename, int level) 
{
    FILE *f;
    
    add_stderr();
    if (num_logfiles == MAX_LOGFILES) {
	error(0, "Too many log files already open, not adding `%s'", 
	      filename);
	return;
    }
    
    if (strlen(filename) > FILENAME_MAX) {
	error(0, "Log filename too long: `%s'.", filename);
	return;
    }
    
    f = fopen(filename, "a");
    if (f == NULL) {
	error(errno, "Couldn't open logfile `%s'.", filename);
	return;
    }
    
    logfiles[num_logfiles].file = f;
    logfiles[num_logfiles].minimum_output_level = level;
    strcpy(logfiles[num_logfiles].filename, filename);
    ++num_logfiles;
    info(0, "Added logfile `%s' with level `%d'.", filename, level);
}


#define FORMAT_SIZE (10*1024)
static void format(char *buf, int level, const char *place, int e, 
		   const char *fmt)
{
    static char *tab[] = {
	"DEBUG: ",
	"INFO: ",
	"WARNING: ",
	"ERROR: ",
	"PANIC: ",
	"LOG: "
    };
    static int tab_size = sizeof(tab) / sizeof(tab[0]);
    time_t t;
    struct tm tm;
    char *p, prefix[1024];
    
    p = prefix;
    time(&t);
#if LOG_TIMESTAMP_LOCALTIME
    tm = OS_localtime(t);
#else
    tm = OS_gmtime(t);
#endif
    sprintf(p, "%04d-%02d-%02d %02d:%02d:%02d ",
    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
    tm.tm_hour, tm.tm_min, tm.tm_sec);
    
    p = strchr(p, '\0');
#ifndef WIN32
    sprintf(p, "[%ld] ", OSThreadSelf());
#else
	sprintf(p, "[%ld] ", GetCurrentThreadId());
#endif
    
    p = strchr(p, '\0');
    if (level < 0 || level >= tab_size)
	sprintf(p, "UNKNOWN: ");
    else
	sprintf(p, "%s", tab[level]);
    
    p = strchr(p, '\0');
    if (place != NULL && *place != '\0')
	sprintf(p, "%s: ", place);
    
    if (strlen(prefix) + strlen(fmt) > FORMAT_SIZE / 2) {
	sprintf(buf, "%s <OUTPUT message too long>\n", prefix);
	return;
    }
    
    if (e == 0)
		sprintf(buf, "%s%s\n", prefix, fmt);
    else
		sprintf(buf, "%s%s\n%sSystem error %d: %s\n",
	prefix, fmt, prefix, e, strerror(e));
}


static void output(FILE *f, char *buf, va_list args) 
{
    vfprintf(f, buf, args);
    fflush(f);
}


#ifndef WIN32
static void OS_syslog(char *format, va_list args, int level)
{
    char buf[4096]; /* Trying to syslog more than 4K could be bad */
    int translog;
    
    if (level >= sysloglevel && dosyslog) {
	vsnprintf(buf, sizeof(buf), format, args);
	/* XXX vsnprint not 100% portable */

	switch(level) {
	case OS_DEBUG:
	    translog = LOG_DEBUG;
	    break;
	case OS_INFO:
	    translog = LOG_INFO;
	    break;
	case OS_WARNING:
	    translog = LOG_WARNING;
	    break;
	case OS_ERROR:
	    translog = LOG_ERR;
	    break;
	case OS_PANIC:
	    translog = LOG_ALERT;
	    break;
	default:
	    translog = LOG_INFO;
	    break;
	}
	syslog(translog,buf);
    }
}
#endif

/*
 * Almost all of the message printing functions are identical, except for
 * the output level they use. This macro contains the identical parts of
 * the functions so that the code needs to exist only once. It's a bit
 * more awkward to edit, but that can't be helped. The "do {} while (0)"
 * construct is a gimmick to be more like a function call in all syntactic
 * situation.
 */

#define FUNCTION_GUTS(level, place) \
	do { \
	    int i; \
	    char buf[FORMAT_SIZE]; \
	    va_list args; \
	    \
	    add_stderr(); \
	    format(buf, level, place, e, fmt); \
	    for (i = 0; i < num_logfiles; ++i) { \
		if (level >= logfiles[i].minimum_output_level) { \
		    va_start(args, fmt); \
		    output(logfiles[i].file, buf, args); \
		    va_end(args); \
		} \
/*		if (dosyslog) { */\
/*		    va_start(args, fmt); */\
/*		    OS_syslog(buf,args,level); */\
/*		    va_end(args); */\
/*		} */\
	    } \
	} while (0)


void panic(int e, const char *fmt, ...) 
{
    FUNCTION_GUTS(OS_PANIC, "");
    exit(EXIT_FAILURE);
}


void error(int e, const char *fmt, ...) 
{
    FUNCTION_GUTS(OS_ERROR, "");
}


void warning(int e, const char *fmt, ...) 
{
    FUNCTION_GUTS(OS_WARNING, "");
}


void info(int e, const char *fmt, ...) 
{
   FUNCTION_GUTS(OS_INFO, "");
}


static int place_matches(const char *place, const char *pat) 
{
    size_t len;
    
    len = strlen(pat);
    if (pat[len-1] == '*')
#ifndef WIN32
		return (strncasecmp(place, pat, len - 1) == 0);
#else
		return (strnicmp(place, pat, len - 1) == 0);
#endif

#ifndef WIN32
    return (strcasecmp(place, pat) == 0);
#else
	return (stricmp(place, pat) == 0);
#endif
}


static int place_should_be_logged(const char *place) 
{
    int i;
    
    if (num_places == 0)
	return 1;
    for (i = 0; i < num_places; ++i) {
	if (*loggable_places[i] != '-' && 
	    place_matches(place, loggable_places[i]))
		return 1;
    }
    return 0;
}


static int place_is_not_logged(const char *place) 
{
    int i;
    
    if (num_places == 0)
	return 0;
    for (i = 0; i < num_places; ++i) {
	if (*loggable_places[i] == '-' &&
	    place_matches(place, loggable_places[i]+1))
		return 1;
    }
    return 0;
}


void debug(const char *place, int e, const char *fmt, ...) 
{
	// If we don't need output debug information.
	if (log_g_i_need_debug == 0)
		return;

    if (place_should_be_logged(place) && place_is_not_logged(place) == 0) {
	FUNCTION_GUTS(OS_DEBUG, "");
	/*
	 * Note: giving `place' to FUNCTION_GUTS makes log lines
    	 * too long and hard to follow. We'll rely on an external
    	 * list of what places are used instead of reading them
    	 * from the log file.
	 */
    }
}


void set_debug_places(const char *places) 
{
    char *p = NULL;
#if 0
    p = strtok(OS_strdup(places), " ,");
    num_places = 0;
    while (p != NULL && num_places < MAX_LOGGABLE_PLACES) {
	loggable_places[num_places++] = p;
	p = strtok(NULL, " ,");
    }
#endif
}

/*
 * Switch on or off the debug function.
 *
 * Added by Zhang Yuan, for normal service. 2001-3-1.
 */
void disable_debug_info(void)
{
	log_g_i_need_debug = 0;
	return;
}

void enable_debug_info(void)
{
	log_g_i_need_debug = 1;
	return;
}
