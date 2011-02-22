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


	void CS::loadobj(std::string s){
		CClassDes* pc = NULL;
		pc = CCompiler::classDesTable.getClass((char*)(s.c_str()));
		if (pc == NULL){
			 	BOOL ret = c.Compile("test/test.cs");
				if (!ret){
						printf("==== compile failed ===\n");
				//		return "==== compiler error ===\n";
						return;
				}
				pc = CCompiler::classDesTable.getClass((char*)(s.c_str()));
				printf("-->pc=%x",pc);
		}
		vm.LoadObject(pc);
		return;
	};