#include <string>
#include <time.h>
#include "log.h"


    
using namespace JUJU;
		
#ifdef WIN32
stdext::hash_map<char*, JUJU::CLog*> JUJU::CLog::log_list;
#else
__gnu_cxx::hash_map<char*, JUJU::CLog*> JUJU::CLog::log_list;

#endif

bool CLog::bUseCache = false;
CLog CLog::pInst;// = new CLog;
char CLog::logFileName[256] = "";
	
void JUJU::debug(char* fmt, ...){

	 char msg[1001] = "";
	 va_list v;
	 va_start(v, fmt);
	 vsnprintf(msg, 1000, fmt, v);
	 va_end(v);
	JUJU::CLog::Log(msg, __FILE__, __LINE__, 100, "DBG", "");
}
	void JUJU::_log(std::string  msg, char* file, long line, int l, char* type, char* filename){
        _log((char*)msg.c_str(), file, line, l, type, filename);        
    }
  
	void JUJU::_log(char*  msg, char* file, long line, int l, char* type, char* filename){
        if (msg == NULL)
            msg = "";
        char *p, prefix[1024];
        char fileName[256] = ""; // generated full filename
        p = prefix;
        
        // get current time
        time_t t;
        struct tm *ptm;
        time(&t);
#ifdef LOG_TIMESTAMP_GMTTIME
        ptm = localtime(&t);
#else
        ptm = gmtime(&t);
#endif
        
        // generate actual file name
        snprintf(fileName, 255, "%s%04d%02d%02d.log",filename, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
        sprintf(p, "%04d-%02d-%02d %02d:%02d:%02d ",
            ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
        
        FILE* f = fopen(fileName, "a+");
        if (f == NULL)
            return;
        
        
        fprintf(f, "%s [%s %d]\t%s\t(%s:%d)\r\n", p, type, l, msg, file, line);
        
        fflush(f);
  
        fclose(f);
        
    }

	void JUJU::CLog::_log(char *msg, char* file, long line, int l, char *type){
		if (msg == NULL)
            msg = "";
        char *p, prefix[1024];
        char fileName[256] = ""; // generated full filename
        p = prefix;
        
        // get current time
        time_t t;
        struct tm *ptm;
        time(&t);
#if LOG_TIMESTAMP_GMTTIME
        tm = localtime(t);
#else
        ptm = gmtime(&t);
#endif
        
		// generate time string
        sprintf(p, "%04d-%02d-%02d %02d:%02d:%02d ",
            ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
		
		int thread_id = 0;
#ifdef WIN32
		thread_id = GetCurrentThreadId();
#else
#endif
		char content[1024] = "";
		snprintf(content, 1000, "%s [%s %d]%d\t%s\t(%s:%d)\r\n", p, type, l, thread_id, msg, file, line);

#ifdef ENABLE_LOG_CACHE
		if (bUseCache)
			cache.append(content);
		else
#endif
			real_log(content);
		
		
	};


    
#ifdef ENABLE_LOG_CACHE
void JUJU::log_thread(void* p){
	
	cLOG("log thread started.");
	thread* thd = (thread*)p;
	thd->stop_event = -1;
	int count = 0;
	thd->heart_beat = 0;
	while (thd->stop_event< 0){
		thd->heart_beat = thd->heart_beat ^0xff;
		if (count > 3600*24)
		{
			cLOG("I am working");
			count = 0;
		}
		((CLog*)(thd->param))->flush();
		sleep(1000);
		count ++;

	}
cLOG("log thread quit.");
	thd->stop_event = 2;
	
}
#endif

	JUJU::CLog::~CLog(){
#ifdef ENABLE_LOG_CACHE
		if (pth->stop_event < 0 ){
			pth->stop();	
			unsigned short h = pth->heart_beat;
			int count = 0;
			while(pth->stop_event != 2){
				if (h == pth->heart_beat && count > 100)
					break;
				count ++;
				JUJU::sleep(10);
			}
			JUJU::sleep(100);
		}
#endif
	};
