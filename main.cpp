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

CConfigure conf;
CCompiler c;
CVirtualMachine vm;
CPubFuncTable g_PubFuncTable;

///////////////////////////////////////
// function for external lib function
///////////////////////////////////////


void applyOption(char* p, char* v){
	
}

int main(int num, char** args){

#if 0

	//c.setOutput("compile_err.txt");

	conf.set("debug","yes");
	conf.set("classpath", "../");

	c.setConf(conf);
	BOOL ret = c.Compile("test/test.cs");
	//ret = c.Compile("D:\\studio\\projects\\webmud\\mud\\MudOS\\MudLib\\xkx2001\\d\\city\\dongmen.c");
	printf("compile ret=%d\n", ret);

#ifdef WIN32
//	g_PubFuncTable.LoadLib("baselib.dll", "baselib.int");
#else
//	g_PubFuncTable.LoadLib("libbaselib.so", "../baselib.int");
#endif
	CCompiler::classDesTable.getClass("test\\test");
//	vm.LoadObject(CCompiler::classTable.getClass("test\\test"));
	//int index = 0;
	//CFunction* pfn = CCompiler::classTable.getClass("test")->getFuncTable()->GetFunction("create", &index);
	//vm.LoadFunction(pfn);
//	vm.Run();
	//std::vector<std::string>* files = findfile("*", "D:\\studio\\projects\\webmud\\mud\\MudOS\\MudLib\\xkx2001\\d\\baituo");

	//for (int i =0; i< files->size(); i++){
	//	printf("%s\r\n", ((*files)[i]).c_str());
	//	c.Compile((char*) ((*files)[i]).c_str());
	//}
	//Delete(files);
	//getchar();
#endif
	if (num == 1){
	
		printf("New C Script 0.0.1 Deveopled by jackie.ju@gmail.com\n");
		printf("New C Script is C-style programming script language.\n");
		printf("Usage: mse {options} (class_name)\n");
		printf("www.cs-soft.com\n");
		return 0;
	}
	
	bool bOption = false;
	char* target = NULL;
	for (int i = 1; i < num; i++){
		if (args[i][0]=='-'){
			applyOption(args[i], args[i+1]);
			i++;
		}else{
			target = args[i];
		}
			
	} 
	
CConfigure  conf;
CS* cs;
	cs = new CS();
    cs->setOutput(stdout);
    conf.set("debug","yes");
    conf.set("classpath", "/Users/juweihua/studio/projects/WebMudFramework/ScriptEngine/mse/lib;/Users/juweihua/studio/projects/WebMudFramework/ScriptEngine/mse");
    cs->setConf(conf);
//	cs->loadobj("test/test");
	cs->loadobj(target);
}
