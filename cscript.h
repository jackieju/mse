#ifndef __CS_H__
#define __CS_H__

class CS{
public:
	
	CCompiler c;
	CVirtualMachine vm;
	CPubFuncTable g_PubFuncTable;
	
public:
	CS(){
			CConfigure conf;
			conf.set("debug","yes");
			conf.set("classpath", ".");
			c.setConf(conf);
		};
	~CS(){};
	
	void setConf(CConfigure& conf){
		c.setConf(conf);
	};
	
	void loadobj(std::string s){
		CClassDes* pc = NULL;
		pc = CCompiler::classDesTable.getClass((char*)(s.c_str()));
		if (pc == NULL){
			 	BOOL ret = c.Compile("test/test.cs");
				if (!ret){
						printf("==== compiler error ===\n");
				//		return "==== compiler error ===\n";
						return;
				}
				pc = CCompiler::classDesTable.getClass((char*)(s.c_str()));
				printf("-->pc=%x",pc);
		}
		vm.LoadObject(pc);
		return;
	};

};
#endif
