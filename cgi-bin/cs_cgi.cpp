#include "stdio.h"
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

#include "clib.h"
#include "stdio.h"
#include "cocoR/cp.hpp"
#include "cocoR/CR_ERROR.hpp"
#include "compiler.h"
#include "findfile.h"
#include "mem.h"
#include "VirtualMachine.h"
#include "os/osutils.h"
#include "Configure.h"
#include "cscript.h"

CConfigure  conf;
 CCompiler c;
 CVirtualMachine vm;
 CPubFuncTable g_PubFuncTable;
CS* cs;
unsigned int htoi(char s[])
{
    unsigned int val = 0;
    int x = 0;

    if(s[x] == '0' && (s[x+1]=='x' || s[x+1]=='X')) x+=2;

    while(s[x]!='\0')
    {
       if(val > UINT_MAX) return 0;
       else if(s[x] >= '0' && s[x] <='9')
       {
          val = val * 16 + s[x] - '0';
       }
       else if(s[x]>='A' && s[x] <='F')
       {
          val = val * 16 + s[x] - 'A' + 10;
       }
       else if(s[x]>='a' && s[x] <='f')
       {
          val = val * 16 + s[x] - 'a' + 10;
       }
       else return 0;

       x++;
    }
    return val;
}

void init_cs(){
	cs = new CS();
	conf.set("debug","yes");
	conf.set("classpath", "/Users/juweihua/studio/projects/WebMudFramework/ScriptEngine/mse/lib;/Users/juweihua/studio/projects/WebMudFramework/ScriptEngine/mse");
	cs->setConf(conf);
}

string exec_cmd(string user, string cmd, std::map<string, string> p){
	string r = "hello";
	// load user
	
	// get cmd
	
	cLOG("11");
	// send cmd to user
	// user::onCommand(command name, command param)
#if 0
	// test
 	BOOL ret = c.Compile("test/test.cs");
	// if (!ret){
	// 	printf("==== compiler error ===\n");
	// 	return "==== compiler error ===\n";
	// }
		
//	CCompiler::classDesTable.dump();
	CClassDes* pc = CCompiler::classDesTable.getClass("test/test");
//	printf("this=%x,==>ps=%x", &CCompiler::classDesTable, pc);
	vm.LoadObject(pc);
#endif
	cs->loadobj("test/test");
	
	cLOG("111");
	return r;
}

int main()
{

	cLOG("1");

//	debug("a=%s, b=%s", "aa", "bb");
//return 0;
    int i,n;
//printf ("Content type: text/plain\n\n");
printf ("Content type: text/html\n\n");

n=0;
    char qs[2000] ="";
    char* pqs = qs;
    if(getenv("CONTENT-LENGTH")){
    n=atoi(getenv("CONTENT-LENGTH"));
    printf("content-length:%d\n",n);
    for (i=0; i<n;i++){
        //int is-eq=0;
        char c=getchar();
//printf("%c", c);
        switch (c){
            case '&':
                *pqs='\n';
                pqs++;
                break;
            case '+':
                *pqs=' ';
                pqs++;
                break;
            case '%':{
                char s[3] = "";
                s[0]=getchar();
                s[1]=getchar();
                s[2]=0;
                *pqs=htoi(s);
                i+=2;
                pqs++;
                }
                break;
//case '=':
//c=':';
//is-eq=1;
//break;
            default:
            *pqs=c;
            pqs++;
        };
    }
    }
printf("%s\n",getenv("QUERY_STRING"));
printf("qs=%s\n",qs);

init_cs();
map<string, string> m;
// object->command(param), return
exec_cmd("test", "cmd", m);

	cLOG("2");

fflush(stdout);
SAFEDELETE(cs);
return 0;
}
