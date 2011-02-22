#ifndef JUJU_LOG_H
#define JUJU_LOG_H
#include "options.h"
#include <string>
#ifdef WIN32
#include <hash_map>
#else
#include <ext/hash_map>
#endif
#include "macros.h"
#ifdef ENABLE_LOG_CACHE
#include "thread.h"
#endif

#include <stdio.h>
  #include   <time.h>   
  #include   <stdlib.h>  

#define LOG_DEFAULT_LVL 100

#if !defined(_WIN32)
using   namespace   __gnu_cxx;
#endif
//static std::hash_map<char*, CLog*> log_list;


namespace JUJU{
void _log(std::string  msg, char* file, long line, int l, char* type, char* filename="");
void _log(char*  msg, char* file, long li, int l, char* type,char* filename="");


#ifdef ENABLE_LOG_CACHE
void log_thread(void* p);
class thread{

public:
	unsigned char heart_beat;
	thread(void* param){
		this->param = param;
		thread_create_real(log_thread, (void*)this, "log_daemon");
		stop_event = 0;
	}
	
	~thread(){

	}

	void stop(){
		stop_event = 1;
	}

	int stop_event;

	void* param;

};
#endif
/*struct   str_hash  
  {  
                  size_t   operator()(const  std::string&   str)   const  
                  {  
                                  return   __stl_hash_string(str.c_str());  
                  }  
  }; */  
class CLog{



	
public:
#ifdef WIN32
	static stdext::hash_map<char*, CLog*> log_list;
#else
	static hash_map<char*, CLog*> log_list;
#endif

	void _log(char *msg,char* file, long line, int l, char *type);

	static void Log(char* msg, char* file, long line, int l, char* type, char* category = ""){
		if (category == NULL || strlen(category) == 0)
			getDefInst()->_log(msg, file, line, l, type);
		else{
			getInst(category)->_log(msg, file, line, l, type);
		}
	}


	
	static CLog* getInst(char* cat){
			if (cat == NULL || strlen(cat) == 0)
				return getDefInst();
		CLog* p = log_list[cat];
		if (p == NULL){
			p = new CLog(cat);
			log_list[cat] = p;
		}
		return p;

	}
	static CLog* getDefInst(){
		//if (pInst == NULL)
//		{
//			pInst = new CLog();
//			pInst.Init();
//		}
		return &pInst;
	}

	void Init(){

	}

	~CLog();
	static void setFile(char* file){
		strncpy(logFileName, file, 255);
	}
	static void enableStdOut(bool t){
		bStdOut = t;
	}
	static std::string genFileName(char* cat=""){
		if (strlen(logFileName) == 0)
			return "";
			
 		char fileName[256] = ""; // generated full filename
      
        // get current time
        time_t t;
        struct tm *ptm;
        time(&t);
#if LOG_TIMESTAMP_GMTTIME
        tm = localtime(t);
#else
        ptm = gmtime(&t);
#endif
        
        // generate actual file name
        snprintf(fileName, 255, "%s%s%04d%02d%02d.log",logFileName, cat, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);

		return std::string(fileName);

	}

	void real_log(char *s){
		std::string fileName = genFileName(this->category);
		FILE* f = NULL;
		if (strlen(fileName.c_str())>0){
	 		f = fopen(fileName.c_str(), "a+");
			fprintf(f, "%s", s);
			fflush(f);
        	fclose(f);
		}
        //if (f == NULL)
          //  return;
        
        if (bStdOut && strlen(logFileName)>0)
			fprintf(stdout, "%s", s);
	}
	void flush(){
       
		if (cache.length() == 0)
			return;

		real_log((char *)cache.c_str());
		
		cache = "";
	}

	void writeFirstLog(){
		char msg[300] = "";
		sprintf(msg, "log started, category=%s", this->category);

		this->_log(msg, __FILE__, __LINE__, 0, "EVENT");

		flush();
	}

	static void setCache(bool useCache){
		bUseCache = useCache;
	}
protected:
	CLog(char* n ){
		strcpy(category, n);
		writeFirstLog();
#ifdef ENABLE_LOG_CACHE
		pth = new thread(this);
#endif
		//bUseCache = true;
		level = LOG_DEFAULT_LVL;
	};
	
	CLog(){
		strcpy(category, "");
		writeFirstLog();
#ifdef ENABLE_LOG_CACHE
		pth = new thread(this);
#endif
		//bUseCache = true;
		level = LOG_DEFAULT_LVL;
	};

private:
	static CLog pInst;
	static char logFileName[256];
	char category[256];
	std::string cache;
#ifdef ENABLE_LOG_CACHE
	thread* pth;
#endif
	static bool bUseCache;
	static bool bStdOut;
	int level;


};

void debug(char* fmt, ...);

}

// simple log, no cache
#define cLOG(m) JUJU::_log(m, __FILE__, __LINE__, 10, "LOG")

// cached log
#define LOG0(m) JUJU::CLog::Log(m, __FILE__, __LINE__, 10, "LOG")
#define ERR(m) JUJU::CLog::Log(m, __FILE__, __LINE__, 10, "ERR")
#define ERR2(m, c) JUJU::CLog::Log(m, __FILE__, __LINE__, 10, "ERR", c)
#define LOG(m, category)  JUJU::CLog::Log(m, __FILE__, __LINE__, 10, "LOG", category)
#define nLOG(m, n)  JUJU::CLog::Log(m, __FILE__, __LINE__, n, "LOG", "")
#define nTRACE(m, n) JUJU::CLog::Log(m, __FILE__, __LINE__, n, "TRC", "")
//#ifdef TRACE
//#undef TRACE
//#endif
//#define TRACE(m)  JUJU::CLog::Log(m, __FILE__, __LINE__, 10, "TRC")
#define TRACE1(m, category)  JUJU::CLog::Log(m, __FILE__, __LINE__, 10, "TRC", category)
using namespace JUJU;
#endif
