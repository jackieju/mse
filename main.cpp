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

CConfigure conf;
CCompiler c;
CVirtualMachine vm;
CPubFuncTable g_PubFuncTable;

///////////////////////////////////////
// function for external lib function
///////////////////////////////////////




int main(int num, char** args){


	//c.setOutput("compile_err.txt");

	conf.set("debug","yes");
	conf.set("classpath", "../");

	c.setConf(conf);
	BOOL ret = c.Compile("test/test.c");
	//ret = c.Compile("D:\\studio\\projects\\webmud\\mud\\MudOS\\MudLib\\xkx2001\\d\\city\\dongmen.c");
	printf("compile ret=%d\n", ret);

#ifdef WIN32

	g_PubFuncTable.LoadLib("baselib.dll", "baselib.int");
#else
	g_PubFuncTable.LoadLib("libBaselib.so", "../baselib.int");
#endif
	CCompiler::classTable.getClass("test\\test");
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
}
