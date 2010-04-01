#include "findfile.h"
#ifdef WIN32
#include <io.h>
#endif
#include "macros.h"
#include "mem.h"


#ifndef WIN32
/*
In linux there are no __finddata_t, no findfirst and findnext,so
we use scandir to search files
Ruifeng 2002/10/29
*/
char* pFor="*.*";
extern "C" int select_file(dirent* mydir)
{
	return IsWanted( (char *)(mydir[0].d_name), pFor);
}
#endif

std::vector<std::string>* findfile(char* pattern, char* dir){
	std::vector<std::string>* r = new std::vector<std::string>;
	long findhandle = -1;
#ifndef WIN32
	struct dirent **namelist;
	char szScriptName[256];
	int n;
	pFor = pValue;
	n=scandir(g_szScriptPath ,&namelist, select_file,alphasort);
	if (n<0)
		perror("scandir");
	else
	{
		while(n--)
		{
			memset(szScriptName, 0, 256);
			snprintf(szScriptName, 255, "%s/%s", g_szScriptPath, namelist[n]->d_name); 
			free(namelist[n]);
			printf("compilering %s\n",szScriptName);
			if (compiler.Compile(szScriptName))
			{
				snprintf(szMsg, 300,"SE:: compile \"%s\" succeeded", szScriptName);
				g_pLogFunc(szMsg, __FILE__, __LINE__ , 1);
				printf(szMsg);
				printf("\n");
			}
			else
			{
				snprintf(szMsg, 300,"SE:: compile \"%s\" failed, error msg from compiler:\"%s\"",
					szScriptName, compiler.GetErrMsg());
				g_pLogFunc(szMsg, __FILE__, __LINE__ , 1);
				printf(szMsg);
				printf("\n");

				lRet = -1;
				goto out;
			}
		}
		free(namelist);
	}
#else
	char szScriptFileMask[_MAX_PATH];
	sprintf(szScriptFileMask, "%s%s%s", dir, PATH_SEPARATOR_S, pattern);
	_finddata_t finddata;
	findhandle = _findfirst(szScriptFileMask, &finddata);
	if (findhandle != -1)
	{
		int ret;
		ret = 0;
		while (ret != -1)
		{

			if (finddata.name[0] != '.')
			{
				if (finddata.attrib == _A_SUBDIR){
					char sub_dir[_MAX_PATH] = "";
					snprintf(sub_dir, _MAX_PATH -1, "%s\\%s", dir, finddata.name);
					std::vector<std::string>* rr = findfile("*", sub_dir);
					for (int i = 0; i< rr->size(); i++){
						r->push_back(((*rr)[i]).c_str());
					}
					Delete(rr);
				}
				else{
					char szScriptFile[256] = "";
					sprintf(szScriptFile, "%s\\%s", dir, finddata.name);
					r->push_back(szScriptFile);
				}
			}
			memset(&finddata, 0, sizeof(_finddata_t));
			ret = _findnext(findhandle, &finddata);
		}
	}
#endif
	return r;
}	