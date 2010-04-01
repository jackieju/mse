#include "clib.h"
#include "stdio.h"
#include "cocoR/cp.hpp"
#include "cocoR/CR_ERROR.hpp"
#include "compiler.h"
#include "findfile.h"
#include "mem.h"
#include "VirtualMachine.h"
#include "os/osutils.h"


CCompiler c;
CVirtualMachine vm;
CPubFuncTable g_PubFuncTable;

///////////////////////////////////////
// function for external lib function
///////////////////////////////////////




int main(int num, char** args){


	//c.setOutput("compile_err.txt");


	BOOL ret = c.Compile("test\\test.c");
	//ret = c.Compile("D:\\studio\\projects\\webmud\\mud\\MudOS\\MudLib\\xkx2001\\d\\city\\dongmen.c");
	printf("ret=%d\n", ret);

	
	g_PubFuncTable.LoadLib("baselib.dll", "baselib.int");

	vm.LoadObject(CCompiler::classTable.getClass("test\\test"));
	//int index = 0;
	//CFunction* pfn = CCompiler::classTable.getClass("test")->getFuncTable()->GetFunction("create", &index);
	//vm.LoadFunction(pfn);
	vm.Run();
	//std::vector<std::string>* files = findfile("*", "D:\\studio\\projects\\webmud\\mud\\MudOS\\MudLib\\xkx2001\\d\\baituo");

	//for (int i =0; i< files->size(); i++){
	//	printf("%s\r\n", ((*files)[i]).c_str());
	//	c.Compile((char*) ((*files)[i]).c_str());
	//}
	//Delete(files);
	//getchar();
}
