//****************************************************************
//   CPLUS2\PARSER_C.FRM
//   Coco/R C++ Support Frames.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   Jun 12, 1996  Version 1.06
//      Many fixes and suggestions thanks to
//      Pat Terry <p.terry@.ru.ac.za>
//   Oct 11, 1997  Version 1.07
//      Eliminated redundant ";" in some places
//****************************************************************

#include "clib.h"
#include "cc.hpp"

/////////////////////////////////
// add by jackie juju
#include "clib.h"
#include <string.h>
#include <vector>
#include <string>
#include "datatype.h"
#include "LoopTree.h"
#include "opcode.h"
#include "os/CSS_LOCKEX.h"
#include "ScriptFuncTable.h"
#include "log.h"
#include "PubFuncTable.h"
#include "ClassDes.h"
#include "compiler.h"
#include "utility.h"

#define CAST Cast(op1, type1, dt1, op2, type2, dt2);


// add by jackie juju
/////////////////////////////////



#include "cp.hpp"
#include "cs.hpp"

unsigned short int cParser::SymSet[][MAXSYM] = {
  /*EOF_Sym */
  {0x1,0x0,0x0,0x0,0x0,0x0},
  
  {0x0}
};
BOOL cParser::init(CClassDesTable *table, CPubFuncTable* pft, CConfigure* c){
	m_classTable = table;
	m_PubFuncTable = pft;
	m_conf = c;
	// show options
	showOptions();
	return TRUE;
}

void cParser::destroy(){

}
void cParser::Get()
{ do {
    Sym = Scanner->Get();//aa
    Scanner->NextSym.SetSym(Sym);
    if (Sym <= MAXT) Error->ErrorDist ++;
    else {
      if (Sym == PreProcessorSym) { /*83*/
      	
      	    // process #include                                 
      	char str[256];
      	Scanner->GetName(&Scanner->NextSym, str, 255);
      	// get directive
      	char* p = strchr(str, ' ');
      	char* directive = NULL;
      	char* content = NULL;
      	if ( p != NULL ){
      	    *p=0; 
      	    directive = str + sizeof(char);
      	    // proce include 
      	    if (strcmp(str, "include") == 0){
      	            // get content
      	            p += sizeof(char);
      	            while ( (*p == ' ' || *p == '\t' ) && *p != '\0' ){
      	                    p += sizeof(char);              
      	            }
      	            if ( *p != '\0' )
      	                    content = p;
      	    }
      	
      	}
      	    
      	    ;
      } else
      /* Empty Stmt */ ;
      Scanner->NextSym = Scanner->CurrSym;
    }
  } while (Sym > MAXT);
}

void cParser::ExpectWeak(int n, int follow)
{ if (Sym == n) Get();
  else {
    GenError(n);
    while (!(In(SymSet[follow], Sym) || In(SymSet[0], Sym))) Get();
  }
}

int cParser::WeakSeparator(int n, int syFol, int repFol)
{ unsigned short int s[MAXSYM];
  int i;

  if (Sym == n) { Get(); return 1; }
  if (In(SymSet[repFol], Sym)) return 0;

  for (i = 0; i < MAXSYM; i++)
    s[i] = SymSet[0][i] | SymSet[syFol][i] | SymSet[repFol][i];
  GenError(n);
  while (!In(s, Sym)) Get();
  return In(SymSet[syFol], Sym);
}

void cParser::C()
{
	
	
	m_pCurClassDes = new CClassDes(this);
	std::string className = JUJU::getFileName(curFileName);
	m_pCurClassDes->SetName((char*)className.c_str());
	m_classTable->addClass(m_pCurClassDes);
	
	while (Sym == useSym ||
	       Sym == loadSym) {
		if (Sym == useSym) {
			Import();
		} else if (Sym == loadSym) {
			LoadLib();
		} else GenError(84);
	}
	if (Sym == inheritSym) {
		Inheritance();
	}
	while (Sym == identifierSym ||
	       Sym == classSym ||
	       Sym >= staticSym && Sym <= stringSym) {
		Definition();
	}
	Expect(EOF_Sym);
	
	std::string name = m_pCurClassDes->GetFullName();
	name += ".class";
	//FILE* file = fopen(name.c_str(), "w");
	//std::string *s = m_pCurClassDes->output();
	//fprintf(file, "%s", s->c_str());
	//SAFEDELETE(s);
	//fclose(file);
	
}

void cParser::Import()
{
	Expect(useSym);
	char szName[_MAX_PATH]= "";
	ClassFullName(szName);
	
	    
	    CClassDes* cd = this->m_classTable->getClass(szName);
	    if (cd == NULL){
	            strcat(szName, SCRIPT_EXT);
	                                            CCompiler cc;
	                             std::string s = JUJU::getFilePath(c->getCurSrcFile())+szName;
	                                    printf("current file: %s\n", c->getCurSrcFile());
	                                    printf("crrent file path: %s\n", JUJU::getFilePath(c->getCurSrcFile()).c_str());
	                                    printf("use %s\n", szName);
	                                                            if (m_conf)     
	                                                                    cc.setConf(*m_conf);
	                                                            
	                                                            cc.getClassPath().insert(cc.getClassPath().begin(), JUJU::getFilePath(c->getCurSrcFile()));
	                                cc.Compile(szName);
	    };
	Expect(SemicolonSym);
}

void cParser::LoadLib()
{
	Expect(loadSym);
	Expect(identifierSym);
	
	        // load external library 
	        std::string lib = GetCurrSym();
	    #ifdef          WIN32
	        std::string libfile = lib+".dll";
	    #else
	            std::string libfile = "lib"+lib+".so";
	    #endif
	        std::string libintfile = lib+".int";
	#ifdef WIN32
	            m_PubFuncTable->LoadLib((char*)libfile.c_str(), (char*)libintfile.c_str());
	#else
	        char* c1 = (char*)(libfile).c_str();
	        char* c2 = (char*)(libintfile).c_str();
	    m_PubFuncTable->LoadLib(c1, c2);
	#endif
	/* for the further if VM run in another process than compiler run
	    int address = m_pMainFunction->AddStaticData(lib.size()+1, (BYTE*)lib.c_str());
	    ADDCOMMAND1(__loadlib, DS, address);
	*/;
	Expect(SemicolonSym);
}

void cParser::Inheritance()
{
	Expect(inheritSym);
	Expect(identifierSym);
	
	
	// TODO get full name in current imported classes, should not need to provide full name here
	    char* szName = GetCurrSym();
	    CClassDes* cd = this->m_classTable->getClass(szName);
	    if (cd == NULL){
	            std::string s = JUJU::getFilePath(c->getCurSrcFile())+szName;
	            strcpy(szName, s.c_str());
	            cd = this->m_classTable->getClass(szName);
	            if (cd == NULL){
	                            strcat(szName, ".c");
	                            CCompiler cc;                         
	                cc.Compile(szName);
	        }
	    }
	    this->m_pCurClassDes->setParent(szName);
	Expect(SemicolonSym);
}

void cParser::Definition()
{
	if (Sym == identifierSym ||
	    Sym >= staticSym && Sym <= stringSym) {
		if (Sym >= staticSym && Sym <= functionSym) {
			StorageClass();
		}
		PTYPEDES type = new TYPEDES;
		if (Sym >= varSym && Sym <= stringSym) {
			Type(type);
		}
		Expect(identifierSym);
		
		      char szName[MAX_IDENTIFIER_LENGTH];
		      memset(szName, 0, MAX_IDENTIFIER_LENGTH);
		      Scanner->GetName(&Scanner->CurrSym, szName, MAX_IDENTIFIER_LENGTH-1);//得到名称;
		if (Sym == LparenSym) {
			FunctionDefinition();
			this->m_pCurClassDes->AddMember(szName, *type);
		} else if (Sym == SemicolonSym ||
		           Sym == EqualSym) {
			if (Sym == EqualSym) {
				Get();
				this->m_pCurClassDes->AddMember(szName, *type);
				Expression();
				/*TODO*/;
				Expect(SemicolonSym);
			} else if (Sym == SemicolonSym) {
				Get();
				this->m_pCurClassDes->AddMember(szName, *type);
			} else GenError(85);
		} else GenError(86);
	} else if (Sym == classSym) {
		ClassDef();
	} else GenError(87);
}

void cParser::ClassFullName(char* szName)
{
	Expect(identifierSym);
	strcpy(szName, GetCurrSym());
	while (Sym == ColonColonSym) {
		Get();
		strcat(szName, PATH_SEPARATOR_S);
		Expect(identifierSym);
		strcat(szName, GetCurrSym());
	}
}

void cParser::StorageClass()
{
	if (Sym == staticSym) {
		Get();
	} else if (Sym == mySym) {
		Get();
	} else if (Sym == functionSym) {
		Get();
	} else GenError(88);
}

void cParser::Type(PTYPEDES type)
{
	switch (Sym) {
		case varSym: 
		case mixedSym:  
			if (Sym == varSym) {
				Get();
			} else if (Sym == mixedSym) {
				Get();
			} else GenError(89);
			
			              /*      #ifdef __SUPPORT_OBJ
			                      char szName[MAX_IDENTIFIER_LENGTH];
			                      memset(szName, 0, MAX_IDENTIFIER_LENGTH);
			                      Scanner->GetName(&Scanner->CurrSym, szName, MAX_IDENTIFIER_LENGTH-1);//得到名称                     
			                      type->type = dtGeneral;
			                      type->objID = GetObjIDByName(szName); 
			                      if (type->objID == 0)
			                              GenError(120);
			                      #else
			                      // TODO
			                              GenError(120);
			                      #endif
			*/
			      type->type = dtGeneral; type->refLevel = 0;
			break;
		case shortSym:  
			Get();
			if (Sym == intSym) {
				Get();
			}
			                         type->type = dtShort; type->refLevel = 0;      
			break;
		case longSym:  
			Get();
			if (Sym == intSym ||
			    Sym == floatSym) {
				if (Sym == intSym) {
					Get();
				} else if (Sym == floatSym) {
					Get();
				} else GenError(90);
			}
			                         type->type = dtLong; type->refLevel = 0;
			break;
		case unsignedSym:  
			Get();
			if (Sym >= intSym && Sym <= longSym ||
			    Sym == charSym) {
				if (Sym == charSym) {
					Get();
				} else if (Sym == intSym) {
					Get();
				} else if (Sym == longSym) {
					Get();
				} else GenError(91);
			}
			break;
		case charSym:  
			Get();
			                        type->type = dtChar;                    type->refLevel = 0;
			break;
		case intSym:  
			Get();
			                         type->type = dtInt; type->refLevel = 0;
			break;
		case floatSym:  
			Get();
			                        type->type = dtFloat;                   type->refLevel = 0;
			break;
		case doubleSym:  
			Get();
			break;
		case voidSym:  
			Get();
			break;
		case stringSym:  
			Get();
			type->type =  dtStr;            type->refLevel = 0;
			break;
		default :GenError(92); break;
	}
}

void cParser::FunctionDefinition()
{
	
	    this->m_pMainFunction = new CFunction;
	    //for test
	    long t = sizeof(CFunction);
	    m_pMainFunction->m_SymbolTable.m_pParser = this;
	    this->m_LoopTree->m_pFunc = m_pMainFunction;
	    
	    char szName[MAX_IDENTIFIER_LENGTH];
	    memset(szName, 0, MAX_IDENTIFIER_LENGTH);
	    Scanner->GetName(&Scanner->CurrSym, szName, MAX_IDENTIFIER_LENGTH-1);//得到名称
	
	    if (strlen(szName) > 20)
	            GenError(105);
	    else
	            strcpy(m_pMainFunction->m_szName, szName);
	            
	            
	FunctionHeader();
	FunctionBody();
	
	    if (!this->Error->Errors)
	    {
	               char path[_MAX_PATH];
	                            if (strlen(m_szByteCodeFilePath) == 0 )
	                                    sprintf(path, "%s.%s", this->m_pCurClassDes->GetFullName(),szName);
	                            else
	                                    sprintf(path, "%s%s%s.%s", m_szByteCodeFilePath, PATH_SEPARATOR_S, this->m_pCurClassDes->GetFullName(),szName);
	                m_pMainFunction->Output(path);
	            
	    }else{
	    SAFEDELETE( m_pMainFunction);   
	     }
	     m_pMainFunction = NULL;
}

void cParser::Expression()
{
	Conditional();
	while (Sym == EqualSym ||
	       Sym >= StarEqualSym && Sym <= GreaterGreaterEqualSym) {
		AssignmentOperator();
		Expression();
		
		    if (!doAssign()) 
		                    continue;
		
	}
}

void cParser::ClassDef()
{
	Expect(classSym);
	Expect(identifierSym);
	
	      // get name
	      char *szName = new char[MAX_IDENTIFIER_LENGTH];
	      memset(szName, 0, MAX_IDENTIFIER_LENGTH);
	      Scanner->GetName(&Scanner->CurrSym, szName, MAX_IDENTIFIER_LENGTH-1);//得到名称;
	ClassBody();
	Expect(SemicolonSym);
}

void cParser::ClassBody()
{
	
	// 利用CFunction来存放类成员
	CFunction* pSaved = this->m_pMainFunction;      // save current function
	CFunction function;
	function.m_SymbolTable.m_pParser = this;
	this->m_pMainFunction = &function;
	
	
	Expect(LbraceSym);
	while (Sym == identifierSym ||
	       Sym == classSym ||
	       Sym >= staticSym && Sym <= stringSym) {
		Definition();
	}
	Expect(RbraceSym);
	
	/*
	//添加class
	if (!Error->Errors)
	{
	      CObjDes* pClass = new CObjDes(this);
	      pClass->SetName(szName);
	      if (!pClass)
	      {
	              REPORT_MEM_ERROR("Allcotion memory failed")
	      }
	      else
	      {
	              for (int i = 0; i < function.m_SymbolTable.m_nSymbolCount; i++)
	              {
	                      if (!pClass->AddMember(function.m_SymbolTable.tableEntry[i].szName, 
	                              function.m_SymbolTable.tableEntry[i].type))
	                      {
	                              if (pClass)
	                              {
	                                      delete pClass;
	                                      pClass = NULL;
	                              }
	                              GenError(118);                                  
	                              break;
	                      }
	              }
	      }
	      if (!this->AddObj(*pClass))
	      {
	              GenError(119);                          
	              if (pClass)
	              {
	                      delete pClass;
	                      pClass = NULL;
	              }
	      }
	}
	              */
	;
}

void cParser::VarList(PTYPEDES type, char* szFirstName)
{
	ArraySize();
	
	      doVarDecl(type, szFirstName);
	      char szName[MAX_IDENTIFIER_LENGTH];
	if (Sym == EqualSym) {
		Get();
		Expression();
		doAssign();
	}
	while (Sym == CommaSym) {
		Get();
		Expect(identifierSym);
		
		              memset(szName, 0, MAX_IDENTIFIER_LENGTH-1);
		              Scanner->GetName(&Scanner->CurrSym, szName, MAX_IDENTIFIER_LENGTH);
		              
		
		ArraySize();
		
		               doVarDecl(type, szName);
		if (Sym == EqualSym) {
			Get();
			Expression();
			        doAssign();
		}
	}
}

void cParser::ArraySize()
{
	while (Sym == LbrackSym) {
		Get();
		if (Sym >= identifierSym && Sym <= numberSym ||
		    Sym >= stringD1Sym && Sym <= charD1Sym ||
		    Sym == LbraceSym ||
		    Sym == LparenSym ||
		    Sym == StarSym ||
		    Sym == AndSym ||
		    Sym >= PlusSym && Sym <= MinusSym ||
		    Sym >= PlusPlusSym && Sym <= MinusMinusSym ||
		    Sym == newSym ||
		    Sym >= BangSym && Sym <= TildeSym) {
			ConstExpression();
		}
		Expect(RbrackSym);
	}
}

void cParser::ConstExpression()
{
	Expression();
}

void cParser::FunctionHeader()
{
	Expect(LparenSym);
	if (Sym >= varSym && Sym <= stringSym) {
		FormalParamList();
	}
	Expect(RparenSym);
	        
	this->m_pMainFunction->m_iParamNum = this->m_pMainFunction->m_SymbolTable.m_nSymbolCount; 
	if    (!m_pCurClassDes->getFuncTable()->AddFunction(this->m_pMainFunction))
	           {
	                   GenError(114);
	           }               
	;
}

void cParser::FunctionBody()
{
	CompoundStatement();
	        ADDCOMMAND0(__ret);
}

void cParser::FormalParamList()
{
	FormalParameter();
	while (Sym == CommaSym) {
		Get();
		FormalParameter();
	}
}

void cParser::CompoundStatement()
{
	Expect(LbraceSym);
	while (Sym >= identifierSym && Sym <= numberSym ||
	       Sym >= stringD1Sym && Sym <= charD1Sym ||
	       Sym == SemicolonSym ||
	       Sym == LbraceSym ||
	       Sym >= staticSym && Sym <= stringSym ||
	       Sym == LparenSym ||
	       Sym >= StarSym && Sym <= caseSym ||
	       Sym >= defaultSym && Sym <= ifSym ||
	       Sym >= returnSym && Sym <= switchSym ||
	       Sym == AndSym ||
	       Sym >= PlusSym && Sym <= MinusSym ||
	       Sym >= PlusPlusSym && Sym <= MinusMinusSym ||
	       Sym == newSym ||
	       Sym >= BangSym && Sym <= TildeSym) {
		if (Sym >= staticSym && Sym <= stringSym) {
			LocalDeclaration();
		} else if (Sym >= identifierSym && Sym <= numberSym ||
		           Sym >= stringD1Sym && Sym <= charD1Sym ||
		           Sym == SemicolonSym ||
		           Sym == LbraceSym ||
		           Sym == LparenSym ||
		           Sym >= StarSym && Sym <= caseSym ||
		           Sym >= defaultSym && Sym <= ifSym ||
		           Sym >= returnSym && Sym <= switchSym ||
		           Sym == AndSym ||
		           Sym >= PlusSym && Sym <= MinusSym ||
		           Sym >= PlusPlusSym && Sym <= MinusMinusSym ||
		           Sym == newSym ||
		           Sym >= BangSym && Sym <= TildeSym) {
			Statement();
		} else GenError(93);
	}
	Expect(RbraceSym);
}

void cParser::FormalParameter()
{
	PTYPEDES type = new TYPEDES;
	Type(type);
	while (Sym == StarSym) {
		//test;
		Get();
		type->refLevel++;
	}
	Expect(identifierSym);
	
	   char szName[MAX_IDENTIFIER_LENGTH];
	   memset(szName, 0, MAX_IDENTIFIER_LENGTH-1);
	   Scanner->GetName(&Scanner->CurrSym, szName, MAX_IDENTIFIER_LENGTH);
	ArraySize();
	
	     long op;
	     long type1;
	     TYPEDES DT1;
	     
	     if (!m_pMainFunction->PopDigit(&op, &type1, &DT1) || type1 <FIRST_ADDRESS_MODE || type1 >LAST_ADDRESS_MODE)
	     {
	
	             if (type->objID > 0 && type->type == dtGeneral && type->refLevel == 0)//如果是结构， 生成他的指针
	             {
	                     type->refLevel++;
	             }
	             if (!AllocVar(type, szName))
	                     GenError(113);
	             else
	                     m_pMainFunction->m_iParamTotalSize += this->UnitSize(*type);
	     }
	     else
	     {
	             
	             int dimsize[64];//数组最大维数64
	             int i = 0;
	             int arraysize = op;//total size
	             dimsize[i] = op;
	             i++;
	             
	             while (m_pMainFunction->PopDigit(&op, &type1, &DT1))
	             {
	                     dimsize[i] = op;                        
	                     i++;
	                     arraysize *= op;
	             }
	
	             //如果是数组， 分配指针类型
	             long temp;
	             long index;
	             temp = AllocTempVar(type->type, 1);
	             if (temp == -1)
	             {
	                     GenError(98);
	             }
	             //设置类型和名字
	             index = this->m_pMainFunction->m_SymbolTable.m_nSymbolCount -1;
	             strcpy(m_pMainFunction->m_SymbolTable.tableEntry[index].szName, szName);
	             type->dim = i-1;
	             type->refLevel = 1;
	             memcpy(type->dimsize, dimsize, (i-1)*sizeof(long));
	             memcpy(&m_pMainFunction->m_SymbolTable.tableEntry[index].type, type, sizeof(TYPEDES));
	             m_pMainFunction->m_iParamTotalSize += this->UnitSize(*type);
	     }
	     delete type;
}

void cParser::Statement()
{
	while (Sym == caseSym ||
	       Sym == defaultSym) {
		Label();
	}
	switch (Sym) {
		case identifierSym: 
		case numberSym: 
		case stringD1Sym: 
		case charD1Sym: 
		case LbraceSym: 
		case LparenSym: 
		case StarSym: 
		case AndSym: 
		case PlusSym: 
		case MinusSym: 
		case PlusPlusSym: 
		case MinusMinusSym: 
		case newSym: 
		case BangSym: 
		case TildeSym:  
			AssignmentStatement();
			break;
		case breakSym:  
			BreakStatement();
			break;
		case continueSym:  
			ContinueStatement();
			break;
		case doSym:  
			DoStatement();
			break;
		case forSym:  
			ForStatement();
			break;
		case ifSym:  
			IfStatement();
			break;
		case SemicolonSym:  
			NullStatement();
			break;
		case returnSym:  
			ReturnStatement();
			break;
		case switchSym:  
			SwitchStatement();
			break;
		case whileSym:  
			WhileStatement();
			break;
		default :GenError(94); break;
	}
	        m_pMainFunction->ClearExpStack();
}

void cParser::Label()
{
	if (Sym == caseSym) {
		Get();
		ConstExpression();
		Expect(ColonSym);
	} else if (Sym == defaultSym) {
		Get();
		Expect(ColonSym);
	} else GenError(95);
}

void cParser::AssignmentStatement()
{
	Expression();
	Expect(SemicolonSym);
}

void cParser::BreakStatement()
{
	Expect(breakSym);
	Expect(SemicolonSym);
	        //recode this command for write back
	   this->m_curloop->AddBreak(this->m_pMainFunction->m_nCurrentCmdNum);
	   //add command
	   ADDCOMMAND1(__jmp, CC, 0);      
}

void cParser::ContinueStatement()
{
	Expect(continueSym);
	Expect(SemicolonSym);
	
	    //recode this command for write back
	    this->m_curloop->AddContinue(this->m_pMainFunction->m_nCurrentCmdNum);
	    //add command
	    ADDCOMMAND1(__jmp, CC, 0);      
	
}

void cParser::DoStatement()
{
	Expect(doSym);
	Statement();
	Expect(whileSym);
	Expect(LparenSym);
	Expression();
	Expect(RparenSym);
	Expect(SemicolonSym);
}

void cParser::ForStatement()
{
	Expect(forSym);
	Expect(LparenSym);
	if (Sym >= identifierSym && Sym <= numberSym ||
	    Sym >= stringD1Sym && Sym <= charD1Sym ||
	    Sym == LbraceSym ||
	    Sym == LparenSym ||
	    Sym == StarSym ||
	    Sym == AndSym ||
	    Sym >= PlusSym && Sym <= MinusSym ||
	    Sym >= PlusPlusSym && Sym <= MinusMinusSym ||
	    Sym == newSym ||
	    Sym >= BangSym && Sym <= TildeSym) {
		Expression();
	}
	Expect(SemicolonSym);
	
	    //循环从下一句开始
	    int loopEntry = this->m_pMainFunction->m_nCurrentCmdNum;
	    int jzCmd = 0;
	    
	    //压栈
	    this->AddNewLoop();//添加一个新的looptree
	    this->m_curloop->m_entry = loopEntry;//设置新looptree的入口
	
	;
	if (Sym >= identifierSym && Sym <= numberSym ||
	    Sym >= stringD1Sym && Sym <= charD1Sym ||
	    Sym == LbraceSym ||
	    Sym == LparenSym ||
	    Sym == StarSym ||
	    Sym == AndSym ||
	    Sym >= PlusSym && Sym <= MinusSym ||
	    Sym >= PlusPlusSym && Sym <= MinusMinusSym ||
	    Sym == newSym ||
	    Sym >= BangSym && Sym <= TildeSym) {
		Expression();
	}
	
	            //add command jz
	            jzCmd = this->m_pMainFunction->m_nCurrentCmdNum;
	            ADDCOMMAND1(__jnz, CC, 0)
	;
	Expect(SemicolonSym);
	
	    PCOMMAND cmd = NULL;
	    int cmdsaved;
	    int cmdnum;
	
	
	if (Sym >= identifierSym && Sym <= numberSym ||
	    Sym >= stringD1Sym && Sym <= charD1Sym ||
	    Sym == LbraceSym ||
	    Sym == LparenSym ||
	    Sym == StarSym ||
	    Sym == AndSym ||
	    Sym >= PlusSym && Sym <= MinusSym ||
	    Sym >= PlusPlusSym && Sym <= MinusMinusSym ||
	    Sym == newSym ||
	    Sym >= BangSym && Sym <= TildeSym) {
		
		              cmdsaved = this->m_pMainFunction->m_nCurrentCmdNum;
		Expression();
		
		              cmdnum = this->m_pMainFunction->m_nCurrentCmdNum - cmdsaved;
		              cmd = new COMMAND[cmdnum];    
		              //save expression cmd to insert before jmp command
		              long t=cmdnum*sizeof(COMMAND);
		              memcpy(cmd, &(m_pMainFunction->m_pCmdTable[cmdsaved]), cmdnum*sizeof(COMMAND));
		              m_pMainFunction->m_nCurrentCmdNum = cmdsaved;           
		
	}
	Expect(RparenSym);
	Statement();
	
	    long nContinue;//continue 语句的跳转目的地。
	    nContinue = this->m_pMainFunction->m_nCurrentCmdNum;
	    this->m_curloop->SetContinue(nContinue);
	
	    //insert saved cmd
	    for (int i = 0; i< cmdnum; i++)
	    {
	            m_pMainFunction->AddCommand(cmd[i]);
	    }
	    delete cmd;
	    //change loop variable
	    //jump back
	    ADDCOMMAND1(__jmp, CC, loopEntry);      
	    //write back jz instructment
	    this->m_pMainFunction->m_pCmdTable[jzCmd].op[0] = this->m_pMainFunction->m_nCurrentCmdNum;
	    //write back break continue statement
	    this->m_curloop->SetExit(this->m_pMainFunction->m_nCurrentCmdNum);
	    this->ExitCurLoop();//destroy current loop tree
	;
}

void cParser::IfStatement()
{
	Expect(ifSym);
	Expect(LparenSym);
	Expression();
	Expect(RparenSym);
	
	    //判断
	    {
	            long op1;
	            long type1;
	            TYPEDES dt1;
	            //pop
	            if (!m_pMainFunction->PopDigit(&op1, &type1, &dt1) || type1 <FIRST_ADDRESS_MODE || type1 >LAST_ADDRESS_MODE) 
	            {
	                    //      REPORT_COMPILE_ERROR("POP UP FAILED");
	                    GenError(96);
	                    return;
	            }
	            else
	            {
	                    if (dt1.type == dtFloat)
	                    {
	                            GenError(112);
	                    }
	                    else
	                    {
	                            //get address mode
	                            int address_mode = (type1<<8);
	                            address_mode |= (log2(UnitSize(dt1))<<14)|(log2(UnitSize(dt1))<<6);     
	                            address_mode |= AMODE_DIRECT;
	                            //add commmand to command table
	                            ADDCOMMAND3(__test, address_mode, op1, 0, 0)    
	                    }
	            }
	    }
	
	    //记录跳转指令的序号
	    int jzcmd = this->m_pMainFunction->m_nCurrentCmdNum;
	    //加入判断语句
	    ADDCOMMAND1(__jz, CC, 0)//跳转目标在下面补上;
	Statement();
	        int nextcmd; bool bElse = false;
	if (Sym == elseSym) {
		Get();
		
		            bElse = true;
		            int jmpcmd = this->m_pMainFunction->m_nCurrentCmdNum;
		            ADDCOMMAND1(__jmp, CC, 0);
		            //补上jnz的跳转目标
		            nextcmd = this->m_pMainFunction->m_nCurrentCmdNum;
		            this->m_pMainFunction->m_pCmdTable[jzcmd].op[0] = nextcmd;
		
		Statement();
		
		            nextcmd = this->m_pMainFunction->m_nCurrentCmdNum;
		            //补上jmp的跳转目标
		            this->m_pMainFunction->m_pCmdTable[jmpcmd].op[0] = nextcmd;
		
	}
	
	if (!bElse)
	{
	           //补上jnz的跳转目标
	           nextcmd = this->m_pMainFunction->m_nCurrentCmdNum;
	           this->m_pMainFunction->m_pCmdTable[jzcmd].op[0] = nextcmd;
	   };
}

void cParser::NullStatement()
{
	Expect(SemicolonSym);
}

void cParser::ReturnStatement()
{
	Expect(returnSym);
	if (Sym >= identifierSym && Sym <= numberSym ||
	    Sym >= stringD1Sym && Sym <= charD1Sym ||
	    Sym == LbraceSym ||
	    Sym == LparenSym ||
	    Sym == StarSym ||
	    Sym == AndSym ||
	    Sym >= PlusSym && Sym <= MinusSym ||
	    Sym >= PlusPlusSym && Sym <= MinusMinusSym ||
	    Sym == newSym ||
	    Sym >= BangSym && Sym <= TildeSym) {
		Expression();
	}
	
	    //pop
	    long op1;
	    long type;
	    TYPEDES dt1;
	    if (!m_pMainFunction->PopDigit(&op1, &type, &dt1)|| type <FIRST_ADDRESS_MODE || type >LAST_ADDRESS_MODE )
	    {
	    //      REPORT_COMPILE_ERROR("POP UP FAILED");
	            this->GenError(96);
	            return;
	    }
	    int address_mode = type&0x00ff;
	    address_mode |= (log2(UnitSize(dt1))<<6);
	    
	    //ADDCOMMAND
	    // move reutrn value to _ax
	    ADDCOMMAND2(__mov, address_mode|0x8200, _AX, op1);
	    // return
	    ADDCOMMAND0(__ret);
	
	Expect(SemicolonSym);
}

void cParser::SwitchStatement()
{
	Expect(switchSym);
	Expect(LparenSym);
	Expression();
	Expect(RparenSym);
	Statement();
}

void cParser::WhileStatement()
{
	Expect(whileSym);
	Expect(LparenSym);
	
	  int loopentry = this->m_pMainFunction->m_nCurrentCmdNum;
	
	  //压栈
	  this->AddNewLoop();//添加一个新的looptree
	  this->m_curloop->m_entry = loopentry;//设置新looptree的入口
	
	;
	Expression();
	
	//判断
	{
	   long op1;
	   long type1;
	   TYPEDES dt1;
	   //pop
	   if (!m_pMainFunction->PopDigit(&op1, &type1, &dt1) || type1 <FIRST_ADDRESS_MODE || type1 >LAST_ADDRESS_MODE) 
	   {
	           //      REPORT_COMPILE_ERROR("POP UP FAILED");
	           GenError(96);
	           return;
	   }
	   else
	   {
	           if (dt1.type == dtFloat)
	           {
	                   GenError(112);
	           }
	           else
	           {
	                   //get address mode
	                   int address_mode = (type1<<8);
	                   address_mode |= (log2(UnitSize(dt1))<<14)|(log2(UnitSize(dt1))<<6);     
	                   address_mode |= AMODE_DIRECT;
	                   //add commmand to command table
	                   ADDCOMMAND3(__test, address_mode, op1, 0, 0)    
	           }
	   }
	}
	
	int jnzCmd =  this->m_pMainFunction->m_nCurrentCmdNum;
	//jnz command
	ADDCOMMAND1(__jz, CC, 0);
	Expect(RparenSym);
	Statement();
	
	 long nContinue;//continue 语句的跳转目的地。
	 nContinue = this->m_pMainFunction->m_nCurrentCmdNum;
	 this->m_curloop->SetContinue(nContinue);
	
	 //jmp back
	 ADDCOMMAND1(__jmp, CC, loopentry);
	 //write back jnz
	 this->m_pMainFunction->m_pCmdTable[jnzCmd].op[0] = this->m_pMainFunction->m_nCurrentCmdNum;
	
	 this->m_curloop->SetExit(this->m_pMainFunction->m_nCurrentCmdNum);
	 this->ExitCurLoop();//destroy current loop tree
	;
}

void cParser::LocalDeclaration()
{
	
	      PTYPEDES type = new TYPEDES;
	      char szName[MAX_IDENTIFIER_LENGTH];
	      
	
	if (Sym >= varSym && Sym <= stringSym) {
		Type(type);
	} else if (Sym >= staticSym && Sym <= functionSym) {
		StorageClass();
		if (Sym >= varSym && Sym <= stringSym) {
			Type(type);
		}
	} else GenError(96);
	Expect(identifierSym);
	        
	                      memset(szName, 0, MAX_IDENTIFIER_LENGTH);
	                      Scanner->GetName(&Scanner->CurrSym, szName, MAX_IDENTIFIER_LENGTH-1);
	              
	VarList(type, szName);
	Expect(SemicolonSym);
	delete type;
}

void cParser::Conditional()
{
	LogORExp();
}

void cParser::AssignmentOperator()
{
	switch (Sym) {
		case EqualSym:  
			Get();
			break;
		case StarEqualSym:  
			Get();
			break;
		case SlashEqualSym:  
			Get();
			break;
		case PercentEqualSym:  
			Get();
			break;
		case PlusEqualSym:  
			Get();
			break;
		case MinusEqualSym:  
			Get();
			break;
		case AndEqualSym:  
			Get();
			break;
		case UparrowEqualSym:  
			Get();
			break;
		case BarEqualSym:  
			Get();
			break;
		case LessLessEqualSym:  
			Get();
			break;
		case GreaterGreaterEqualSym:  
			Get();
			break;
		default :GenError(97); break;
	}
}

void cParser::LogORExp()
{
	LogANDExp();
	        int type = -1;
	while (Sym == BarBarSym) {
		Get();
		
		            type = 7;
		LogANDExp();
		
		              //add command
		              long op1, op2;
		              long type1, type2;
		              TYPEDES dt1, dt2;
		              //pop出乘数和被乘数
		              if (!m_pMainFunction->PopDigit(&op2, &type2, &dt2) || !m_pMainFunction->PopDigit(&op1, &type1, &dt1) || type1 <FIRST_ADDRESS_MODE || type1 >LAST_ADDRESS_MODE || type2 <FIRST_ADDRESS_MODE || type2 >LAST_ADDRESS_MODE)
		              {
		              //      REPORT_COMPILE_ERROR("POP UP FAILED");
		                      GenError(96);
		                      return;
		              }
		              else
		              {
		                      CAST
		                      //get address mode
		                      int address_mode = (type1<<8)|(short)type2;
		                      address_mode |= (log2(UnitSize(dt1))<<14)|(log2(UnitSize(dt2))<<6);
		                      //add commmand to command table
		                      ADDCOMMAND3(__test, address_mode, op1, op2, 7/*==*/)                                    
		                      //push result
		                      TYPEDES dt;//not used
		                      memset(&dt, 0, sizeof(TYPEDES));
		                      this->m_pMainFunction->PushDigit(_PSW, 0x82, dt);
		              };
	}
	
	      if (type >= 0)
	      {
	              long g;
	              long t;
	              TYPEDES DT;
	              m_pMainFunction->PopDigit(&g, &t, &DT);
	              //将结果存入临时变量
	              
	              _typedes(DT1, dtLong)
	              if (m_pMainFunction->AddVal(NULL, DT1))
	              {
	                      long temp;
	                      temp = m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount-1].address;
	                      ADDCOMMAND2(__mov, DR, temp, _PSW);
	                      m_pMainFunction->PushDigit(temp, AMODE_MEM|0x80, DT1);
	              }
	              else
	              {
	                      REPORT_COMPILE_ERROR("add symbol failed");
	              }
	      };
}

void cParser::LogANDExp()
{
	        
	int type = -1;
	    long op1, op2;
	    long type1, type2;
	    TYPEDES dt1, dt2;
	InclORExp();
	while (Sym == AndAndSym) {
		Get();
		                type = 7;
		InclORExp();
		
		              //add command
		              //pop出乘数和被乘数
		              if (!m_pMainFunction->PopDigit(&op2, &type2, &dt2) || !m_pMainFunction->PopDigit(&op1, &type1, &dt1) || type1 <FIRST_ADDRESS_MODE || type1 >LAST_ADDRESS_MODE || type2 <FIRST_ADDRESS_MODE || type2 >LAST_ADDRESS_MODE)
		              {
		              //      REPORT_COMPILE_ERROR("POP UP FAILED");
		                      GenError(96);
		                      return;
		              }
		              else
		              {
		                      CAST
		                      //get address mode
		                      int address_mode = (type1<<8)|(short)type2;
		                      address_mode |= (log2(UnitSize(dt1))<<14)|(log2(UnitSize(dt2))<<6);
		                      //add commmand to command table
		                      ADDCOMMAND3(__test, address_mode, op1, op2, 6/*==*/)                                    
		                      //push result
		                      TYPEDES dt;//not used
		                      memset(&dt, 0, sizeof(TYPEDES));
		                      this->m_pMainFunction->PushDigit(_PSW, 0x82, dt);
		              }
		;
	}
	
	      if (type >= 0)
	      {
	              long g;
	              long t;
	              TYPEDES dt;
	              m_pMainFunction->PopDigit(&g, &t, &dt);
	              
	              //将结果存入临时变量
	              if (m_pMainFunction->AddVal(NULL, dt1))
	              {                       
	                      long temp;
	                      temp = m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount-1].address;
	                      ADDCOMMAND2(__mov, DR, temp, _PSW);
	                      m_pMainFunction->PushDigit(temp, AMODE_MEM|0x80, dt1);
	              }
	              else
	              {
	                      REPORT_COMPILE_ERROR("add symbol failed");
	              }
	              };
}

void cParser::InclORExp()
{
	ExclORExp();
	while (Sym == BarSym) {
		Get();
		ExclORExp();
	}
}

void cParser::ExclORExp()
{
	ANDExp();
	while (Sym == UparrowSym) {
		Get();
		ANDExp();
	}
}

void cParser::ANDExp()
{
	EqualExp();
	while (Sym == AndSym) {
		Get();
		EqualExp();
	}
}

void cParser::EqualExp()
{
	
	    int type = -1;//0: == 1: !=
	    long op1, op2;
	    long type1, type2;
	    TYPEDES dt1, dt2;
	
	RelationExp();
	while (Sym >= EqualEqualSym && Sym <= BangEqualSym) {
		if (Sym == EqualEqualSym) {
			Get();
			                        type = 0;
		} else if (Sym == BangEqualSym) {
			Get();
			
			                      type = 1;
			
		} else GenError(98);
		RelationExp();
		
		              //add command
		              //pop出乘数和被乘数
		              if (!m_pMainFunction->PopDigit(&op2, &type2, &dt2) || !m_pMainFunction->PopDigit(&op1, &type1, &dt1) || type2 <FIRST_ADDRESS_MODE || type2 >LAST_ADDRESS_MODE)
		              {
		              //      REPORT_COMPILE_ERROR("POP UP FAILED");
		                      GenError(96);
		                      return;
		              }
		              else
		              {
		                      CAST
		                      //get address mode
		                      int address_mode = (type1<<8)|(short)type2;
		                      address_mode |= (log2(UnitSize(dt1))<<14)|(log2(UnitSize(dt2))<<6);
		
		                      //add commmand to command table
		                      if (type == 0)
		                      {       
		                              ADDCOMMAND3(__test, address_mode, op1, op2, 0/*==*/)                                    
		                      }
		                      else if (type == 1)
		                      {
		                              ADDCOMMAND3(__test, address_mode, op1, op2, 1/*==*/)    
		                      }
		
		                      //push result
		                      TYPEDES dt;//not used
		                      memset(&dt, 0, sizeof(TYPEDES));
		                      this->m_pMainFunction->PushDigit(_PSW, 0x82, dt);
		              }
		;
	}
	
	      if (type >= 0)
	      {
	              long g;
	              long t;
	              TYPEDES dt;
	              if (m_pMainFunction->PopDigit(&g, &t, &dt))
	              {
	                      //将结果存入临时变量
	                      if (m_pMainFunction->AddVal(NULL,  dt1))
	                      {                       
	                              long temp;
	                              temp = m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount-1].address;
	                              ADDCOMMAND2(__mov, DR, temp , _PSW);
	                              m_pMainFunction->PushDigit(temp, AMODE_MEM|0x80, dt1);
	                      }
	                      else
	                      {
	                              REPORT_COMPILE_ERROR("add symbol failed");
	                      }
	              }
	      };
}

void cParser::RelationExp()
{
	
	    int type = -1;
	    long op1, op2;
	    long type1, type2;
	    TYPEDES dt1, dt2;
	
	ShiftExp();
	while (Sym >= LessSym && Sym <= GreaterEqualSym) {
		switch (Sym) {
			case LessSym:  
				Get();
				type = 5;
				break;
			case GreaterSym:  
				Get();
				type = 4;
				break;
			case LessEqualSym:  
				Get();
				type = 3;
				break;
			case GreaterEqualSym:  
				Get();
				type = 2;
				break;
			default :GenError(99); break;
		}
		ShiftExp();
		
		            //add command
		            //pop出乘数和被乘数
		            if (!m_pMainFunction->PopDigit(&op2, &type2, &dt2) || !m_pMainFunction->PopDigit(&op1, &type1, &dt1) || type2 <FIRST_ADDRESS_MODE || type2 >LAST_ADDRESS_MODE)
		            {
		            //      REPORT_COMPILE_ERROR("POP UP FAILED");
		                    GenError(96);
		                    return;
		            }
		            else
		            {
		                    CAST
		                    //get address mode
		                    int address_mode = (type1<<8)|(short)type2;
		                    address_mode |= (log2(UnitSize(dt1))<<14)|(log2(UnitSize(dt2))<<6);
		
		                    //add commmand to command table
		                    ADDCOMMAND3(__test, address_mode, op1, op2, type)                                       
		
		                    //push result
		                    TYPEDES dt;//not used
		                    memset(&dt, 0, sizeof(TYPEDES));
		                    this->m_pMainFunction->PushDigit(_PSW, 0x82, dt);
		            }
		
		;
	}
	
	    if (type >= 0)
	    {
	            // pop掉多于的操作数, 因为这时结果已在__AX中
	            long g;
	            long t;
	            TYPEDES dt;
	            m_pMainFunction->PopDigit(&g, &t, &dt);
	
	            //将结果存入临时变量
	            _typedes(dt_temp, dtLong)
	            if (m_pMainFunction->AddVal(NULL,  dt_temp))
	            {                       
	                    long temp;
	                    temp = m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount-1].address;
	                    ADDCOMMAND2(__mov, DR, temp, _PSW);
	                    m_pMainFunction->PushDigit(temp, AMODE_MEM|0x80, dt_temp);
	            }
	            else
	            {
	                    REPORT_COMPILE_ERROR("add symbol failed");
	            }
	    };
}

void cParser::ShiftExp()
{
	AddExp();
	while (Sym >= LessLessSym && Sym <= GreaterGreaterSym) {
		if (Sym == LessLessSym) {
			Get();
		} else if (Sym == GreaterGreaterSym) {
			Get();
		} else GenError(100);
		AddExp();
	}
}

void cParser::AddExp()
{
	        int type = -1;//1: add 0: sub;
	MultExp();
	while (Sym >= PlusSym && Sym <= MinusSym) {
		if (Sym == PlusSym) {
			Get();
			        type = 1;
		} else if (Sym == MinusSym) {
			Get();
			type = 0;
		} else GenError(101);
		MultExp();
		
		              //add command
		              long op1, op2;
		              long type1, type2;
		              TYPEDES dt1, dt2;
		              //pop出乘数和被乘数
		              if (!m_pMainFunction->PopDigit(&op2, &type2, &dt2) || !m_pMainFunction->PopDigit(&op1, &type1, &dt1) || type1 <FIRST_ADDRESS_MODE || type1 >LAST_ADDRESS_MODE || type2 <FIRST_ADDRESS_MODE || type2 >LAST_ADDRESS_MODE)
		              {
		              //      REPORT_COMPILE_ERROR("POP UP FAILED");
		                      GenError(96);
		                      return;
		              }
		              else
		              {
		
		                      //类型转换
		                      CAST
		                      //get address mode
		                      int address_mode = (type1<<8)|(short)type2;
		                      address_mode |= (log2(UnitSize(dt1))<<14)|(log2(UnitSize(dt2))<<6);
		
		                      
		                      if (dt1.dim >0)
		                      {//如果是数组变量
		                              //得到增量
		                              int size = UnitSize(dt1);
		                              for (int i=1; i<dt1.dim;i++)
		                              {
		                                      size *= dt1.dimsize[i];
		                              }
		                              //增量乘以op2得到实际增量,存入_AX
		                              ADDCOMMAND2(__mul, type2<<8, op2, size)
		                              //加法
		                              if (dt1.type == dtFloat && dt1.refLevel==0)
		                              {
		                                      if (type == 0)
		                                      ADDCOMMAND2(__fsub, (type1<<8)|0x82, op1, _AX)  
		                                      else if (type == 1)
		                                      ADDCOMMAND2(__fadd, (type1<<8)|0x82, op1, _AX)  
		                              }
		                              else
		                              {
		                                      if (type == 0)
		                                      ADDCOMMAND2(__sub, (type1<<8)|0x82, op1, _AX)   
		                                      else if (type == 1)
		                                      ADDCOMMAND2(__add, (type1<<8)|0x82, op1, _AX)   
		                              }
		                      }
		                      else
		                      {
		                              //add commmand to command table
		                              if (dt1.type == dtFloat && dt1.refLevel==0)
		                              {
		                                      if (type == 0)
		                                      ADDCOMMAND2(__fsub, address_mode, op1, op2)     
		                                      else if (type == 1)
		                                      ADDCOMMAND2(__fadd, address_mode, op1, op2)     
		                              }
		                              else
		                              {
		                                      if (type == 0)
		                                      ADDCOMMAND2(__sub, address_mode, op1, op2)      
		                                      else if (type == 1)
		                                      ADDCOMMAND2(__add, address_mode, op1, op2)      
		                              }
		                      }       
		                      //将AX中的结果存入临时变量
		                      if (m_pMainFunction->AddVal(NULL,  dt1))
		                      {                       
		                              long temp;
		                              long opsize = log2(UnitSize(dt1))<<6;
		                              temp = m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount-1].address;
		                              ADDCOMMAND2(__mov, DR|((opsize<<8)|opsize), temp , _AX);
		                              m_pMainFunction->PushDigit(temp, AMODE_MEM|opsize, dt1);
		                      }
		                      else
		                      {
		                              REPORT_COMPILE_ERROR("add symbol failed");
		                      }
		              };
	}
}

void cParser::MultExp()
{
	        char szName[MAX_IDENTIFIER_LENGTH];     memset(szName, 0, MAX_IDENTIFIER_LENGTH);
	CastExp();
	
	     Scanner->GetName(&Scanner->CurrSym, szName, MAX_IDENTIFIER_LENGTH-1);//得到名称
	     int type = -1;//0: mult 1: div 2:percent;
	while (Sym == StarSym ||
	       Sym >= SlashSym && Sym <= PercentSym) {
		if (Sym == StarSym) {
			Get();
			        type = 0;
		} else if (Sym == SlashSym) {
			Get();
			        type = 1;
		} else if (Sym == PercentSym) {
			Get();
			        type = 2;
		} else GenError(102);
		CastExp();
		
		              Scanner->GetName(&Scanner->CurrSym, szName, MAX_IDENTIFIER_LENGTH-1);//得到名称
		              
		              //add command
		              long op1, op2;
		              long type1, type2;
		              TYPEDES dt1, dt2;
		
		              //pop出乘数和被乘数
		              if (!m_pMainFunction->PopDigit(&op2, &type2, &dt2) || !m_pMainFunction->PopDigit(&op1, &type1, &dt1) ||type1 <FIRST_ADDRESS_MODE || type1 >LAST_ADDRESS_MODE || type2 <FIRST_ADDRESS_MODE || type2 >LAST_ADDRESS_MODE)
		              {
		                      //      REPORT_COMPILE_ERROR("POP UP FAILED");
		                      GenError(96);
		                      return;
		              }
		              else
		              {
		                      CAST
		                      //get address mode
		                      int address_mode = (type1<<8)|(short)type2;
		                      address_mode |= (log2(UnitSize(dt1))<<14)|(log2(UnitSize(dt2))<<6);
		                      //add commmand to command table
		                      if (dt1.type == dtFloat && dt1.refLevel==0)
		                      {
		                              if (type == 0) ADDCOMMAND2(__fmul, address_mode, op1, op2)                                      
		                                      else if (type == 1) ADDCOMMAND2(__fdiv, address_mode, op1, op2)                                 
		                                      else GenError(109);
		                      }
		                      else
		                      {
		                              if (type == 0) ADDCOMMAND2(__mul, address_mode, op1, op2)                                       
		                                      else if (type == 1) ADDCOMMAND2(__div, address_mode, op1, op2)
		                                      else if (type == 2) ADDCOMMAND2(__mod, address_mode, op1, op2)
		                      }
		                      
		                      //将AX中的结果存入临时变量
		                      if (m_pMainFunction->AddVal(NULL, dt1))
		                      {                       
		                              long temp;
		                              long opsize = log2(UnitSize(dt1))<<6;
		                              temp = m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount-1].address;
		                              ADDCOMMAND2(__mov, DR|((opsize<<8)|opsize), temp, _AX);
		                              m_pMainFunction->PushDigit(temp, AMODE_MEM|opsize, dt1);
		                      }
		                      else
		                      {
		                              this->GenError(98);
		                      }
		              }
		;
	}
}

void cParser::CastExp()
{
	
	    //保存一元操作符栈
	    EXPRESSIONOP* pSavedStack = this->m_pExpOpPt;
	    //清空一元操作符栈
	    m_pExpOpPt = &m_ExpOp;
	    //int op;
	
	UnaryExp();
	
	    int op;
	    while (this->PopOp(&op))
	    {               
	
	            //操作数出栈
	            long op1;
	            long type1;
	            TYPEDES dt1;    
	            if (!m_pMainFunction->PopDigit(&op1, &type1, &dt1) ||type1 <FIRST_ADDRESS_MODE || type1 >LAST_ADDRESS_MODE)
	            {
	                    break;//genError(96);
	            }
	            int address_mode = (type1<<8);
	            address_mode |= log2(UnitSize(dt1))<<14;
	            
	            //添加命令
	            switch (op)
	            {
	            case PlusSym:                           
	                    //no operation
	                    break;
	            case MinusSym:                  
	                    {
	                            int opsize;
	                            if (!m_pMainFunction->AddVal(NULL, dt1))
	                                    GenError(98);
	                            if (dt1.type == dtFloat)
	                            {
	                                    opsize = (type1>>6)&0xc0;
	                                    ADDCOMMAND2(__fsub, type1, 0, op1)
	                            }
	                            else
	                            {
	                                    opsize = (type1>>6)&0xc0;
	                                    ADDCOMMAND2(__sub, type1, 0, op1)
	                            }
	                            ADDCOMMAND2(__mov, DR, m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount -1].address, _AX)
	                            m_pMainFunction->PushDigit(m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount -1].address, AMODE_MEM|(opsize<<6), dt1);
	                    }
	                    break;
	            case StarSym:  
	                    {
	                            if (dt1.refLevel <1)
	                                    GenError(102);
	                            dt1.refLevel--;
	                            //取操作的字节类型
	                            int opsize = log2(UnitSize(dt1));
	                            opsize = opsize<< 6;
	                            type1 |= opsize; 
	
	                            //将type1的间接访问级别提高一级
	                            int level = (type1 & 0x30) >> 4;
	                            if (level == 3)
	                            {
	                                    GenError(103);
	                                    break;
	                            }
	                            level ++;
	                            type1 |= level << 4;
	/*                              int j = (type1>>4)&0x3;
	                            if (j < 1)
	                                    GenError(102);//is not a point
	                            else
	                                    j--;
	                            j = j << 4;
	*/                              
	/*                      
	                            type1 &= 0xffcf;
	                            type1 |= j;
	*/
	                            //入栈
	                            m_pMainFunction->PushDigit(op1, type1, dt1);
	                    }
	                    break;
	            case BangSym: 
	                    {
	                            if (!m_pMainFunction->AddVal(NULL, dt1))
	                                    GenError(98);
	                            int opsize = (type1>>6)&0x03;
	                            ADDCOMMAND2(__mov, (0x100|(opsize<<14))|(type1), m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount -1].address, op1)
	                            ADDCOMMAND1(__notr, type1<<8, m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount -1].address)
	                            m_pMainFunction->PushDigit(m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount -1].address, type1, dt1);
	                    }
	                    break;
	            case AndSym:  
	                    {
	                            long temp;
	                            temp = AllocTempVar(dtLong, 1);
	                            if (temp == -1)
	                            {
	                                    GenError(98);
	                            }
	                            else
	                            {
	                                    if (dt1.objID > 0)
	                                    {//如果时obj， 不取地址
	                                            m_pMainFunction->PushDigit(op1, type1, dt1);
	                                    }
	                                    else
	                                    {
	                                            ADDCOMMAND2(__ea, 0x8100|type1, temp, op1)
	                                            dt1.refLevel ++;
	                                            m_pMainFunction->PushDigit(temp, AMODE_MEM|0x80, dt1);
	                                    }
	                            }
	                    }
	                    break;
	            case TildeSym: //'~'
	                    {
	                            if (!m_pMainFunction->AddVal(NULL, dt1))
	                                    GenError(98);
	                            int opsize = (type1>>6)&0xc0;
	                            ADDCOMMAND2(__mov, (0x100|(opsize<<14))|(type1), m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount -1].address, op1)
	                            ADDCOMMAND1(__not, type1<<8, m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount -1].address)
	                            m_pMainFunction->PushDigit(m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount -1].address, type1, dt1);
	                    }
	                    break;
	            default :GenError(93); break;
	            }               
	    }
	    //恢复一元运算符栈
	    this->ClearOpStack();
	    m_pExpOpPt = pSavedStack;  
}

void cParser::UnaryExp()
{
	if (Sym >= identifierSym && Sym <= numberSym ||
	    Sym >= stringD1Sym && Sym <= charD1Sym ||
	    Sym == LbraceSym ||
	    Sym == LparenSym ||
	    Sym == newSym) {
		PostFixExp();
	} else if (Sym >= PlusPlusSym && Sym <= MinusMinusSym) {
		if (Sym == PlusPlusSym) {
			Get();
		} else if (Sym == MinusMinusSym) {
			Get();
		} else GenError(103);
		UnaryExp();
	} else if (Sym == StarSym ||
	           Sym == AndSym ||
	           Sym >= PlusSym && Sym <= MinusSym ||
	           Sym >= BangSym && Sym <= TildeSym) {
		UnaryOperator();
		CastExp();
	} else GenError(104);
}

void cParser::PostFixExp()
{
	Primary();
	while (Sym == identifierSym ||
	       Sym == LbrackSym ||
	       Sym == LparenSym ||
	       Sym >= PlusPlusSym && Sym <= MinusGreaterSym) {
		switch (Sym) {
			case LbrackSym:  
				Get();
				Expression();
				Expect(RbrackSym);
				
				{
				                              //add command
				                              long op1, op2;
				                              long type1, type2;
				                              TYPEDES dt1, dt2;
				                              //pop
				                              if (!m_pMainFunction->PopDigit(&op2, &type2, &dt2) || !m_pMainFunction->PopDigit(&op1, &type1, &dt1) || type1 <FIRST_ADDRESS_MODE || type1 >LAST_ADDRESS_MODE || type2 <FIRST_ADDRESS_MODE || type2 >LAST_ADDRESS_MODE)
				                              {
				                                      //      REPORT_COMPILE_ERROR("POP UP FAILED");
				                                      GenError(96);
				                                      return;
				                                      
				                              }
				                              else
				                              {       
				                              /*处理a[b]的流程(要考虑指针， 数组， 多维数组， 作为参数传进来的数组和指针）
				                                      1. 得到b下标代表的单位长度(如果是数组, 通过维数计算, 如果是指针, reflvl减一后计算UnitSize)
				                                      2. b*单位长度= offset 
				                                      3. a + offset -> temp
				                                      4. push temp, 间接寻址
				                                      */
				                                      int offset;
				                                      BOOL bIsArray = TRUE;
				                                      if (dt1.dim >0)//是数组
				                                      {
				                                              bIsArray = TRUE;
				                                              offset= UnitSize(dt1);
				                                              for (int i = 1; i< dt1.dim; i++)
				                                              {
				                                                      offset *= dt1.dimsize[i];
				                                              }
				                                              //不生成新的typedes, 直接修改dt1, 使之维数减一
				                                              if (dt1.dim >1)
				                                                      memcpy(dt1.dimsize, dt1.dimsize+1, sizeof(long)*(dt1.dim-1));                                   
				                                              dt1.dim--;                                                      
				                                      }
				                                      else
				                                      {//不是数组, 是指针
				                                              bIsArray = FALSE;
				                                              if (dt1.refLevel > 0)//是指针且不是指针数组
				                                              {
				                                                      //不生成新的typedes, 直接修改dt1, 使之*号减一
				                                                      dt1.refLevel--;
				                                                      offset = UnitSize(dt1);
				                                              }
				                                              else//既不是指针也不是数组
				                                                      GenError(101);                                          
				                                      }
				
				                                      //偏移量 = 单位长度* 下标addcommand(mul, DC, 下标, 单位长度)
				                                      ADDCOMMAND2(__mul, (type2<<8)|0x8080, op2, offset)
				                                      //增加临时变量存放中间结果
				                                      long temp = this->AllocTempVar(dtLong);
				                                      if (temp == -1)
				                                      {
				                                              GenError(98);
				                                      }
				                                      //居卫华2001-08-22
				                                      long lIndirect = (type1>>4)&0x03;
				                                      if (bIsArray == FALSE && lIndirect>0)//如果是指针， 则间接寻址
				                                              ADDCOMMAND2(__add, DR|0x1000, op1, _AX)
				                                      else
				                                              ADDCOMMAND2(__add, DR, op1, _AX)
				                                      //将结果保存到temp
				                                      ADDCOMMAND2(__mov, DR, temp, _AX)
				                                      //入栈, 作为指针用
				                                      m_pMainFunction->PushDigit(temp, AMODE_MEM|0x10|((log2(UnitSize(dt1)))<<6), dt1);
				}
				};
				break;
			case LparenSym:  
				
				      
				{
				                      char szName[MAX_IDENTIFIER_LENGTH];
				                      memset(szName, 0, MAX_IDENTIFIER_LENGTH);
				                      Scanner->GetName(&Scanner->CurrSym, szName, MAX_IDENTIFIER_LENGTH-1);//得到名称
				                      long index = m_PubFuncTable->FindFuncByName(szName);
				                      FUNCCALL fn;
				                      fn.name = szName;
				                      if (index < 0) // cannot find function name in public function table, guess is was script function
				                      {
				                              // find in script function table
				                              CFunction* pScript = m_pCurClassDes->getFuncTable()->GetFunction(szName, &index);
				                              if (pScript == NULL)
				                              {       
				                                      GenError(97);
				                                      Get();
				                                      return;
				                              }
				                              else
				                              {
				                                      fn.pVF = pScript;
				                                      fn.nType = 0;
				                                      ADDCOMMAND1(__callv, CC, (long)pScript);
				                                      m_pCurClassDes->getFuncTable()->ReleaseFunc();
				                              }
				
				                      }
				                      else
				                      {
				                              ADDCOMMAND1(__callpub, CC, (long)(this->m_PubFuncTable->m_FuncTable[index].pfn));
				                              fn.pfn = &m_PubFuncTable->m_FuncTable[index];
				                              fn.nType = 1;                           
				                      }
				
				                                                ;
				FunctionCall(&fn);
				};
				break;
			case PointSym:  
				Get();
				Expect(identifierSym);
				
				{
				char* member = GetCurrSym();
				int address = m_pMainFunction->AddStaticData(strlen(member)+1, (BYTE*)member);
				//add command
				long op1, type1;
				TYPEDES dt1;
				
				//pop
				if (!m_pMainFunction->PopDigit(&op1, &type1, &dt1) || type1 != dtGeneral){
				         GenError(96);
				        return;
				}else
				            {
				                 long temp = AllocTempVar(dtGeneral);
				             if (temp == -1)
				                     {
				 GenError(98);
				                    }
				                    // get effective address for object member
				                    ADDCOMMAND3(__eaobj, AMODE_OBJ|AMODE_MEM<<8, temp, op1, address);
				                     _typedes(DT1, dtLong)
				                     m_pMainFunction->PushDigit(temp, AMODE_MEM|0x80, DT1);
				            }
				            };
				break;
			case MinusGreaterSym:  
				{int state = 0;
				Get();
				while (Sym == LbraceSym) {
					Get();
					state++;
				}
				Expect(identifierSym);
				while (Sym == RbraceSym) {
					Get();
					state++;
				}
				
				              if (state == 0)         {                               
				                              //pop
				                              TYPEDES dt;
				                              long type;
				                              long op;
				                              if (!m_pMainFunction->PopDigit(&op, &type, &dt))
				                              {
				                                      GenError(96);
				                              }
				                              else
				                              {
				                                      if (dt.objID > 0)
				                                      {
				                                              char szName[MAX_IDENTIFIER_LENGTH];
				                                              memset(szName, 0, MAX_IDENTIFIER_LENGTH);
				                                              Scanner->GetName(&Scanner->CurrSym, szName, MAX_IDENTIFIER_LENGTH-1);//得到名称                             
				                                              
				                                              #if 0
				                                              //get offset
				                                              OBJMEMDES* pMember = this->m_ObjTable[dt.objID-1]->GetMemberByName(szName);
				                                              if (pMember == NULL)
				                                              {
				                                                      GenError(121);                                  
				                                              }
				                                              else
				                                              {       
				                                                      TYPEDES dtTemp;
				                                                      memcpy(&dtTemp, &pMember->dt, sizeof(TYPEDES));
				                                                      
				                                                      long temp = AllocTempVar(dtLong, 1);
				                                                      
				                                                      if (temp == -1)
				                                                      {
				                                                              GenError(98);
				                                                      }
				                                                      
				                                                      //add(op, offset) = address
				                                                      ADDCOMMAND2(__add, DC, op, pMember->offset)
				                                                      ADDCOMMAND2(__mov, DR, temp, _AX)
				                                                      if ((dtTemp.dim ==0 && dtTemp.objID == 0 )
				                                                              ||(dtTemp.objID > 0 && dtTemp.refLevel > 0))
				                                                              {//如果不是数组和obj
				                                                                      
				                                                                      //取改member的opsize
				                                                                      int opsize;
				                                                                      if (dtTemp.refLevel > 0) //如果是指针类型
				                                                                              opsize = log2(OPSIZE_PTR);
				                                                                      else
				                                                                              opsize = log2(typesize(dtTemp.type, dtTemp.objID));
				                                                                      opsize &= 0x3;
				                                                                      m_pMainFunction->PushDigit(temp, AMODE_MEM|(opsize << 6)|0x10, dtTemp);
				                                                                      
				                                                              }
				                                                              else
				                                                              {
				                                                                      
				                                                                      //如果是数组或obj, 生成临时变量存放数组地址
				                                                                      if (dtTemp.dim != 0)
				                                                                      {                                                                       
				                                                                              /*居卫华2001-08-22修改
				                                                                              dtTemp.refLevel++;
				                                                                              dtTemp.dim--;
				                                                                              */
				                                                                              m_pMainFunction->PushDigit(temp, AMODE_MEM|(log2(OPSIZE_PTR)<<6), dtTemp);
				                                                                      }
				                                                                      else
				                                                                      {
				                                                                              dtTemp.refLevel++;
				                                                                              m_pMainFunction->PushDigit(temp, AMODE_MEM|(log2(OPSIZE_PTR)<<6), dtTemp);
				                                                                      }
				                                                              }
				                                                              
				                                              }
				                                              #endif
				                                      }
				                              }
				                      }
				                      } // int state
				                      ;
				break;
			case identifierSym:  
				Get();
				break;
			case PlusPlusSym:  
				Get();
				
				              {
				                              long op1;
				                              long type1;
				                              TYPEDES dt1;
				                              //pop出被加数
				                              if (!m_pMainFunction->PopDigit(&op1, &type1, &dt1) || type1 <FIRST_ADDRESS_MODE || type1 >LAST_ADDRESS_MODE) 
				                              {
				                                      //      REPORT_COMPILE_ERROR("POP UP FAILED");
				                                                              GenError(96);
				                      return;
				
				                              }
				                              else
				                              {
				                                      if (dt1.type == dtFloat)
				                                      {
				                                              GenError(112);
				                                      }
				                                      else
				                                      {
				                                              //get address mode
				                                              int address_mode = (type1<<8);
				                                              address_mode |= (log2(UnitSize(dt1))<<14)|(log2(UnitSize(dt1))<<6);     
				                                              if (dt1.dim >0)
				                                              {//如果是数组变量
				                                                      //得到增量
				                                                      int size = UnitSize(dt1);
				                                                      for (int i=1; i<dt1.dim;i++)
				                                                      {
				                                                              size *= dt1.dimsize[i];
				                                                      }
				                                                      //加法
				                                                      ADDCOMMAND2(__add, (type1<<8)|0x80, op1, size)
				                                              }
				                                              else
				                                              {
				                                                      //add commmand to command table
				                                                      ADDCOMMAND2(__add, address_mode, op1, 1)        
				                                              }       
				                                              ADDCOMMAND2(__mov, address_mode|0x02, op1, _AX); 
				                                              //将AX中的结果存入临时变量
				                                              m_pMainFunction->PushDigit(op1, type1, dt1);
				                                      }
				                              }
				                      }
				              ;
				break;
			case MinusMinusSym:  
				Get();
				
				                      {
				                              long op1;
				                              long type1;
				                              TYPEDES dt1;
				                              //pop出被加数
				                              if (!m_pMainFunction->PopDigit(&op1, &type1, &dt1) || type1 <FIRST_ADDRESS_MODE || type1 >LAST_ADDRESS_MODE) 
				                              {
				                                      //      REPORT_COMPILE_ERROR("POP UP FAILED");
				                                                              GenError(96);
				                      return;
				
				                              }
				                              else
				                              {
				                                      if (dt1.type == dtFloat)
				                                      {
				                                              GenError(112);
				                                      }
				                                      else
				                                      {
				                                              //get address mode
				                                              int address_mode = (type1<<8);
				                                              address_mode |= (log2(UnitSize(dt1))<<14)|(log2(UnitSize(dt1))<<6);     
				                                              if (dt1.dim >0)
				                                              {//如果是数组变量
				                                                      //得到增量
				                                                      int size = UnitSize(dt1);
				                                                      for (int i=1; i<dt1.dim;i++)
				                                                      {
				                                                              size *= dt1.dimsize[i];
				                                                      }
				                                                      //加法
				                                                      ADDCOMMAND2(__sub, (type1<<8)|0x80, op1, size)
				                                              }
				                                              else
				                                              {
				                                                      //add commmand to command table
				                                                      ADDCOMMAND2(__sub, address_mode, op1, 1)        
				                                              }       
				                                              ADDCOMMAND2(__mov, address_mode|0x02, op1, _AX); 
				                                              //将AX中的结果存入临时变量
				                                              m_pMainFunction->PushDigit(op1, type1, dt1);
				                                      }
				                              }
				                      };
				break;
			default :GenError(105); break;
		}
	}
}

void cParser::UnaryOperator()
{
	switch (Sym) {
		case PlusSym:  
			Get();
			break;
		case MinusSym:  
			Get();
			break;
		case StarSym:  
			Get();
			break;
		case BangSym:  
			Get();
			break;
		case AndSym:  
			Get();
			break;
		case TildeSym:  
			Get();
			break;
		default :GenError(106); break;
	}
}

void cParser::Primary()
{
	
	      char szName[MAX_IDENTIFIER_LENGTH];
	      memset(szName, 0, MAX_IDENTIFIER_LENGTH);
	switch (Sym) {
		case identifierSym:  
			Get();
			
			
			  {       Scanner->GetName(&Scanner->CurrSym, szName, MAX_IDENTIFIER_LENGTH-1);//得到名称
			  
			          
			          int address = -1;
			          
			          if (strcmp(szName, "this")==0){
			          // TODO: get address of current object
			                  address = 0;
			          }else
			              address = GetSymAddress(szName);
			
			           //如果adress<0是函数名
			                    if (address >= 0 )//如果不是函数名， 是变量名。
			                    {
			                            TYPEDES dt;
			                            SYMBOLTABLEELE *pElement = m_pMainFunction->m_SymbolTable.GetSym(szName);
			                            if (pElement == NULL)
			                            {
			                                                            printf("===>generror 115, symbol name=%s", szName);
			                                    GenError(115); // not found it symbol table, check it in runtime
			                            }
			                            else
			                            {
			                                    memcpy(&dt ,&(pElement->type), sizeof(TYPEDES));
			                                    /*if (pdt == NULL)
			                                    {
			                                    GenError(111);
			                                    }
			                                    else*/
			                            
			                                    if ((dt.dim ==0 && dt.objID == 0 )
			                                            ||(dt.objID > 0 && dt.refLevel > 0))
			                                    {//如果不是数组和obj
			                                            int opsize;
			                                            if (dt.refLevel > 0) 
			                                                    opsize = log2((int)OPSIZE_PTR);
			                                            else
			                                                    opsize = log2((int)typesize(dt.type, dt.objID));
			                                            opsize &= 0x3;
			                                            m_pMainFunction->PushDigit(address, AMODE_MEM|(opsize << 6), dt);
			                                    }
			                                    else
			                                    {//如果是数组或obj, 生成临时变量存放数组地址
			                                            if (dt.dim != 0)
			                                            {
			                                                    long temp = AllocTempVar(dtLong, 1);
			                                                    if (temp == -1)
			                                                    {
			                                                            GenError(98);
			                                                    }
			                                                    
			                                                    ADDCOMMAND2(__ea, DD, temp, address)
			                                                    /*居卫华2001－8－22修改
			                                                    dt.refLevel++;
			                                                    dt.dim--;
			                                                    */
			                                                    m_pMainFunction->PushDigit(temp, AMODE_MEM|(log2( (int)(OPSIZE_PTR) )<<6), dt);
			                                            }
			                                            else // if it is object
			                                            {
			                                                                                    int opsize;
			                                                                                    opsize = log2((int)OPSIZE_PTR);
			                                                opsize &= 0x3;
			                                                                                    m_pMainFunction->PushDigit(address, AMODE_MEM|(opsize << 6), dt);
			                                            }
			                                    }
			                                    
			                            }
			                    }
			          else // address < 0
			          {
			                  //GenError(126);
			                  if (this->m_PubFuncTable->FindFuncByName(szName) < 0)
			                  {
			                          long index = 0;
			                          if (m_pCurClassDes->getFuncTable()->GetFunction(szName, &index) == NULL)
			                                  GenError(104);
			                          else
			                                  m_pCurClassDes->getFuncTable()->ReleaseFunc();
			                  }
			                  //      return;
			          }
			};
			break;
		case newSym:  
			Get();
			Creator();
			break;
		case stringD1Sym:  
			Get();
			
			             {
			                                        char* pCh;
			                                        pCh = new char[Scanner->CurrSym.Len+1];
			                                        if (pCh == NULL)
			                                        {
			                                                GenError(106);
			                                                Get();
			                                                break;
			                                        }
			                                        memset(pCh, 0, Scanner->CurrSym.Len+1);
			                                                            Scanner->GetName(&Scanner->CurrSym, pCh, Scanner->CurrSym.Len);//得到名称
			                                           char* string = NULL;
			                                        string = AnalyzeConstString(pCh);
			                                     if (string == NULL)
			                                        {
			                                                if (pCh)
			                                                        delete pCh;
			                                                GenError(100);                          
			                                                Get();
			                                                break;
			                                        }
			                    
			                                        int straddress = this->m_pMainFunction->m_nSSUsedSize;
			                                        if (m_pMainFunction->AddStaticData(strlen(string)+1, (BYTE*)string)>=0)
			                                        {       
			                                                _typedes(dt, dtStr)
			                                                
			                                                if (m_pMainFunction->AddVal(NULL,  dt))
			                                                {
			                                                        long temp;
			                                                        temp = m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount-1].address;
			                                                        ADDCOMMAND2(__ea, DS, temp, straddress);
			                                                        m_pMainFunction->PushDigit(temp, AMODE_MEM, dt);                         
			                                                }
			                                        }
			                                        else
			                                        {
			                                                this->GenError(98);
			                                        }
			                                            
			                                                            if (string)
			                                                delete string;
			                                                            if (pCh)
			                                                delete pCh;         
			                                };
			break;
		case charD1Sym:  
			Get();
			
			            {
			                    Scanner->GetName(&Scanner->NextSym, szName, MAX_IDENTIFIER_LENGTH-1);//得到名称
			                    _typedes(dt,dtChar);
			                    m_pMainFunction->PushDigit(szName[1], AMODE_DIRECT, dt);
			            }
			;
			break;
		case numberSym:  
			Get();
			
			
			             if (strchr(szName, '.'))        // float
			             {
			                     
			                     float number = (float)atof(GetCurrSym());
			                     _typedes(dt, dtFloat);                  
			                     m_pMainFunction->PushDigit(*(long*)&number, AMODE_DIRECT|OPSIZE_4B, dt);
			             }
			             else    // int
			             {
			                     
			                     int number = atoi(GetCurrSym());
			                     _typedes(dt, dtLong);                   
			                     // Address mode: direct, Operation=4Byte =>0x80
			                     m_pMainFunction->PushDigit(number, AMODE_DIRECT|OPSIZE_4B, dt);
			             }
			;
			break;
		case LparenSym:  
			Get();
			Expression();
			Expect(RparenSym);
			break;
		case LbraceSym:  
			SetDef();
			break;
		default :GenError(107); break;
	}
}

void cParser::FunctionCall(FUNCCALL* pFuncEntry)
{
	Expect(LparenSym);
	if (Sym >= identifierSym && Sym <= numberSym ||
	    Sym >= stringD1Sym && Sym <= charD1Sym ||
	    Sym == LbraceSym ||
	    Sym == LparenSym ||
	    Sym == StarSym ||
	    Sym == AndSym ||
	    Sym >= PlusSym && Sym <= MinusSym ||
	    Sym >= PlusPlusSym && Sym <= MinusMinusSym ||
	    Sym == newSym ||
	    Sym >= BangSym && Sym <= TildeSym) {
		ActualParameters(pFuncEntry);
	}
	Expect(RparenSym);
	
	//long lParamNum = 0;
	    if (pFuncEntry->nType)
	    {
	    //addcommand
	    ADDCOMMAND0(__endcallpub);
	    //save return value
	    _typedes(dt, dtLong)
	    if (m_pMainFunction->AddVal(NULL, dt))
	    {                       
	            long temp;
	            temp = m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount-1].address;
	            ADDCOMMAND2(__mov, DR, temp , _AX);
	            m_pMainFunction->PushDigit(temp, AMODE_MEM|0x80, dt);
	    }
	    else
	    {
	            this->GenError(98);
	    }
	                            
	    }
	    else
	    {
	    // check param number
	    //if (lParamNum != pFuncEntry->pVF->m_iParamNum)
	    //{
	    //      GenError(123);
	    //}
	
	    //addcommand
	    ADDCOMMAND0(__endcallv);
	
	    //save return value
	    _typedes(dt, dtLong)
	    if (m_pMainFunction->AddVal(NULL, dt))
	    {
	            long temp;
	            temp = m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount-1].address;
	            ADDCOMMAND2(__mov, DR, temp , _AX);
	            m_pMainFunction->PushDigit(temp, AMODE_MEM|0x80, dt);
	    }
	    else
	    {
	            this->GenError(98);
	    }
	    };
}

void cParser::HashItem()
{
	if (Sym == stringD1Sym) {
		Get();
	} else if (Sym == identifierSym) {
		Get();
	} else GenError(108);
	Expect(ColonSym);
	Expression();
}

void cParser::SetItem()
{
	if (Sym == identifierSym ||
	    Sym == stringD1Sym) {
		HashItem();
	} else if (Sym == numberSym ||
	           Sym == charD1Sym ||
	           Sym == LbraceSym ||
	           Sym == LparenSym ||
	           Sym == StarSym ||
	           Sym == AndSym ||
	           Sym >= PlusSym && Sym <= MinusSym ||
	           Sym >= PlusPlusSym && Sym <= MinusMinusSym ||
	           Sym == newSym ||
	           Sym >= BangSym && Sym <= TildeSym) {
		Expression();
	} else GenError(109);
}

void cParser::SetItems()
{
	SetItem();
	while (Sym == CommaSym) {
		Get();
		SetItem();
	}
}

void cParser::SetDef()
{
	Expect(LbraceSym);
	if (Sym >= identifierSym && Sym <= numberSym ||
	    Sym >= stringD1Sym && Sym <= charD1Sym ||
	    Sym == LbraceSym ||
	    Sym == LparenSym ||
	    Sym == StarSym ||
	    Sym == AndSym ||
	    Sym >= PlusSym && Sym <= MinusSym ||
	    Sym >= PlusPlusSym && Sym <= MinusMinusSym ||
	    Sym == newSym ||
	    Sym >= BangSym && Sym <= TildeSym) {
		SetItems();
	}
	Expect(RbraceSym);
}

void cParser::Creator()
{
	char szName[_MAX_PATH] = "";
	ClassFullName(szName);
	while (Sym == LparenSym) {
		Get();
		while (Sym >= varSym && Sym <= stringSym) {
			FormalParamList();
		}
		Expect(RparenSym);
	}
	        
	   CClassDes* cd = this->m_classTable->getClass(szName);
	   if (cd == NULL){
	           std::string s = JUJU::getFilePath(c->getCurSrcFile())+szName;
	           strcpy(szName, s.c_str());
	           cd = this->m_classTable->getClass(szName);
	           if (cd == NULL){
	                           strcat(szName, ".c");
	                           CCompiler cc;                         
	               cc.Compile(szName);
	       }
	           // TODO generete correct error
	           //GenError(100);
	           //return;
	   }
	           int address = m_pMainFunction->AddStaticData(strlen(szName)+1, (BYTE*)szName);
	   ADDCOMMAND1(__newobj,DS, address);
	   
	
	TYPEDES dt1;
	   memset(&dt1, 0, sizeof(TYPEDES));
	   dt1.type = dtGeneral;
	// 将AX中的结果存入临时变量
	   if (m_pMainFunction->AddVal(NULL,  dt1))
	   {                       
	                   long temp;
	                   long opsize = log2(UnitSize(dt1))<<6;
	                   temp = m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount-1].address;
	                   ADDCOMMAND2(__mov, DR|((opsize<<8)|opsize), temp , _AX);
	                   m_pMainFunction->PushDigit(temp, AMODE_MEM|opsize, dt1);
	   }
	   else
	   {
	                   REPORT_COMPILE_ERROR("add symbol failed");
	   };
}

void cParser::ActualParameters(FUNCCALL* pFuncEntry)
{
	
	//      lParamNum = 0;
	    if (pFuncEntry == NULL)
	            return;
	
	    char cParamNum = 0;     
	
	Expression();
	
	    //pop
	    long op1;
	    long type;
	    TYPEDES dt1;
	    if (!m_pMainFunction->PopDigit(&op1, &type, &dt1)|| type <FIRST_ADDRESS_MODE || type >LAST_ADDRESS_MODE )
	    {
	    //      REPORT_COMPILE_ERROR("POP UP FAILED");
	            this->GenError(96);
	            return;
	    }
	    // address mode of dest op is in hight byte
	    int address_mode = (type<<8)&0xff00;
	    // set op number size
	    address_mode |= (log2(UnitSize(dt1))<<14);
	
	    if (pFuncEntry->nType)  // native function
	            ADDCOMMAND1(__parampub, address_mode, op1)                      
	    else
	            ADDCOMMAND1(__paramv, address_mode, op1)
	
	    cParamNum++; 
	while (Sym == CommaSym) {
		Get();
		Expression();
		
		                    // modified on 20030331 by weihua ju
		//                      if (cParamNum > pFuncEntry->params.size()-1)
		//                      {
		//                              GenError(106);
		//                      }
		
		                    //pop
		                    long op1;
		                    long type;
		                    TYPEDES dt1;
		                    if (!m_pMainFunction->PopDigit(&op1, &type, &dt1)|| type <FIRST_ADDRESS_MODE || type >LAST_ADDRESS_MODE)
		                    {
		                            //      REPORT_COMPILE_ERROR("POP UP FAILED");
		                            this->GenError(96);
		                            return;
		                    }
		                    else
		                    {
		                            //get address mode
		                            int address_mode = (type<<8)&0xff00;
		                            address_mode |= (log2(UnitSize(dt1))<<14);
		                            
		                            /*              保留
		                            if (dt1.dim > 0)
		                            {
		                            _typedes(dt, dtInt)
		                            long temp;
		                            //如果是数组, 把它的地址存入临时变量, 传入临时变量
		                            if (m_pMainFunction->AddVal(NULL, dt))
		                            {                       
		                            temp = m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount-1].address;
		                            ADDCOMMAND2(__ea, DD, temp, op1);
		                            }
		                            long size = sizeof(long*);
		                            size = log2(size);
		                            size = size << 14;
		
		                            ADDCOMMAND1(__parampub, size|0x100, temp)       
		                            }
		                            else    
		                            */                      
		                            if (pFuncEntry->nType)  // native function
		                            ADDCOMMAND1(__parampub, address_mode, op1)                      
		                                    else
		                            ADDCOMMAND1(__paramv, address_mode, op1)                
		                            cParamNum++; 
		                    }               
		;
	}
	
	            // code added on 20030331 weihua ju
	            if ((pFuncEntry->nType!=0) && cParamNum != pFuncEntry->pfn->cParamNum)
	            {
	
	                    GenError(106);
	            }
	    //lParamNum = cParamNum;
}



void cParser::Parse()
{ Scanner->Reset(); Get();
  C();
}

//////////////////////////////////
// add by jackie juju

// all these function maybe should be move to single class
BOOL cParser::AllocVar(PTYPEDES type, char *szName)
{
	/*int size_t = 0;
	//get size 
	if (nType < 0)
	{
		size_t = sizeof(long);
	}
	else
	{
		switch(nType)
		{
		case dtShort:
			size_t = 1;
			break;

		case dtInt:
			size_t = 2;
			break;
		case dtLong:
			size_t = 4;
			break;
		case dtChar:
			size_t = 1;
			break;
		case dtString:
			size_t = 4;
			break;
		case dtStrArray:
			size_t = 4;
			break;
		default:
			REPORT_COMPILE_ERROR("undefined type");
			return -1;
		}
	}
*/

		if (m_pMainFunction) // in function definition
		return m_pMainFunction->AddVal(szName, *type);
	else
		return this->m_pCurClassDes->AddMember(szName, *type);

}



BOOL cParser::PopOp(int *op)
{
	if (op == NULL)
		return FALSE;
	*op = 0;
	if (this->m_pExpOpPt->pPrev == NULL)
		return FALSE;
	*op  = m_pExpOpPt->opcode;
	EXPRESSIONOP *pTemp = m_pExpOpPt;
	m_pExpOpPt = m_pExpOpPt->pPrev;
	delete pTemp;
	return TRUE;
}


/*
   函数名称     : cParser::PushExpOp
   函数功能	    : 一元运算符入栈
   变量说明     : 
   int op: 运算符
   返回值       : 
   编写人       : 居卫华
   完成日期     : 2001 - 6 - 6
*/
void cParser::PushExpOp(int op)
{
	EXPRESSIONOP *pNew = new EXPRESSIONOP;	
	if (pNew == NULL)
	{
		REPORT_MEM_ERROR("memory allocation error");
		return;
	}

	pNew->opcode = op;
	pNew->pPrev = m_pExpOpPt;
	m_pExpOpPt = pNew;
}
/*

void cParser::PushExpOp2(int op)
{
	EXPRESSIONOP *pNew = new EXPRESSIONOP;	
	pNew->opcode = op;
	pNew->pPrev = m_pExpOpPt2;
	m_pExpOpPt2 = pNew;
}

void cParser::PushExpDigit2(long digit, int type)
{
	EXPRESSIONDIGIT *pNew = new EXPRESSIONDIGIT;	
	pNew->digit = digit;
	pNew->type = type;
	pNew->pPrev = this->m_pExpDigitPt2;

	m_pExpDigitPt2 = pNew;
}

BOOL cParser::PopOp2(int *op)
{
	if (this->m_pExpOpPt2->pPrev == NULL)
		return FALSE;
	*op  = m_pExpOpPt2->opcode;
	EXPRESSIONOP *pTemp = m_pExpOpPt2;
	m_pExpOpPt2 = m_pExpOpPt2->pPrev;
	delete pTemp;
	return TRUE;
}

BOOL cParser::PopDigit2(long *digit, int *type)
{
	if (this->m_pExpDigitPt2->pPrev == NULL)
		return FALSE;
	*digit = m_pExpDigitPt2->digit;
	*type = m_pExpDigitPt2->type;
	EXPRESSIONDIGIT *pTemp = m_pExpDigitPt2;
	m_pExpDigitPt2 = m_pExpDigitPt2->pPrev;
	delete pTemp;
	return TRUE;
}

*/


void cParser::ClearOpStack()
{
	while (this->m_pExpOpPt->pPrev != NULL)
	{
		EXPRESSIONOP *pTemp = m_pExpOpPt;
		this->m_pExpOpPt = this->m_pExpOpPt->pPrev;
		delete pTemp;
	}
	m_pExpOpPt = &m_ExpOp;
}

int cParser::GetSymAddress(char *szName)
{
	return this->m_pMainFunction->GetSymbolAddress(szName);
}


void cParser::AddNewLoop()
{
	CLoopTree *pNew = new CLoopTree(m_pMainFunction);

	m_curloop->AddNew(*pNew);

	this->m_curloop = pNew;
}

void cParser::ExitCurLoop()
{
	this->m_curloop = m_curloop->m_parent;
	m_curloop->RemoveChild();

}

void cParser::SetPubFuncTable(CPubFuncTable *pTable)
{
	this->m_PubFuncTable = pTable;
}



BOOL cParser::AllocArrayVar(PTYPEDES type, char *szName, int size)
{
	if (!m_pMainFunction->AddVal(szName, *type))
		return FALSE;
	//if (!m_pMainFunction->AddVal(NULL, type->size_t*size, type->type)||!m_pMainFunction->AddVal(szName, typesize(dtInt), dtInt))
	//	return FALSE;
	
	return TRUE;
}

char* cParser::AnalyzeConstString(char *string)
{
	int size = 0;
	int len = strlen(string);
	if (len == 0)
		return NULL;

	char *p = string;
	char *newstring = new char[len+1];
	memset(newstring, 0, sizeof(char)*(len+1));

	p++;//"
	while(*p != '"' && size<=len)
	{
		while (*p != '\\' && *p != '"' && size<=len)
		{
			newstring[size] = *p;
			p++;
			size++;
		}
		if (size >= len)
			break;
		if (*p == '\\')
		{
			p++;
			switch (*p)
			{
				case '\\': newstring[size] = '\\'; break;
				case 'r':newstring[size] = 0x0d; break;
				case 'n':newstring[size] = 0x0a; break;
				case 't':newstring[size] = '\t'; break;
				default: 
					//	newstring[size] = '\\'; 
					//	size++;
					//	newstring[size] = 't';				
					GenError(124);
					break;
			}
			p++;
			size++;
		}
		else
			break;
	}
	newstring[size] = 0;
	size++;
	return newstring;

}





int cParser::UnitSize(TYPEDES &type)
{
	if (type.refLevel<=0)
		return typesize(type.type, type.objID);
	else
		return sizeof(long*);
}



/*
函数名称     : cParser::NeedCast
函数功能	    : 判断是否需要类型转换, 并返回转换码和要转换的运算数
变量说明     : 
TYPEDES dt1 : 运算数1
TYPEDES dt2 : 运算数2
char &casted : [OUT]要被转换的运算数 
返回值       : 要进行的类型转换代号, -1表示不需要转换
编写人       : 居卫华
完成日期     : 2001 - 6 - 8
*/
long cParser::NeedCast(TYPEDES dt1, TYPEDES dt2, char &casted)
{
	casted = 0;
	TYPEDES DT1, DT2;
	//beckup
	memcpy(&DT1, &dt1, sizeof(TYPEDES));
	memcpy(&DT2, &dt2, sizeof(TYPEDES));

	/*
	//如果是指针, 则不能进行转换
	if (DT1.refLevel > 0 || DT2.refLevel > 0)
		return -1;

*/
	if ((DT1.objID > 0 && DT2.objID == 0 ) || (DT2.objID > 0 && DT1.objID == 0 ))
	{
		return -1;
	}
	//指针都转换成long
	if (DT1.refLevel > 0)
	{
		//指针不能同浮点进行运算 
		if (dt2.type ==dtFloat)
		{
			return -1;
		}
		dt1.type = dtLong;
		dt1.refLevel = 0;
	}

	if (DT2.refLevel > 0)
	{
		//指针不能同浮点进行运算 
		if (DT1.type == dtFloat)
		{
			return -1;
		}
		DT2.type = dtLong;
		DT2.refLevel = 0;
	}


	///////////////////////////////
	// code modify 2002-03-19
	///////////////////////////////
/*	
	original code:
	//如果类型相同, 不需要转换
	if (DT1.type == DT2.type)
		return -1;


	switch (DT1.type)
	{
	case dtFloat:
		{
			casted = 2;
			switch (DT2.type)
			{
			case dtChar: return __CAST_C2F;
			case dtShort: return __CAST_S2F;
			case dtInt: return __CAST_I2F;
			case dtLong: return __CAST_L2F;
			default:GenError(110);return -1;
			}
		}
	case dtLong:
		{
			switch (DT2.type)
			{
			case dtChar: casted = 2; return __CAST_C2L;
			case dtShort: casted = 2; return __CAST_S2L;
			case dtInt: return -1;
			case dtFloat: casted = 1; return __CAST_L2F;
			default:GenError(110);return -1;
			}
		}
	case dtInt:
		{
			switch (DT2.type)
			{
			case dtChar: casted = 2; return __CAST_C2I;
			case dtShort: casted = 2; return __CAST_S2I;
			case dtLong: return -1;
			case dtFloat: casted = 1; return __CAST_I2F;
			default:GenError(110);return -1;
			}
		}
	case dtShort:
		{
			switch (DT2.type)
			{
			case dtChar: casted = 2; return __CAST_C2S;
			case dtInt: casted = 1; return __CAST_S2I;
			case dtLong: casted = 1;  return __CAST_S2L;
			case dtFloat: casted = 1; return __CAST_S2F;
			default:GenError(110);return -1;
			}
		}
	case dtChar:
		{
			switch (DT2.type)
			{
			case dtShort: casted = 1;return __CAST_C2S;
			case dtInt: casted = 1; return __CAST_C2I;
			case dtLong: casted = 1;  return __CAST_C2L;
			case dtFloat: casted = 1; return __CAST_C2F;
			default:GenError(110);return -1;
			}
		}
	default:GenError(110);		
	}
*/
	/* new code:*/
	if (dt1.type == dt2.type)
		return -1;

	switch (dt1.type)
	{
	case dtFloat:
		{
			casted = 2;
			switch (dt2.type)
			{
			case dtChar: return __CAST_C2F;
			case dtShort: return __CAST_S2F;
			case dtInt: return __CAST_I2F;
			case dtLong: return __CAST_L2F;
			default:GenError(110);return -1;
			}
		}
	case dtLong:
		{
			switch (dt2.type)
			{
			case dtChar: casted = 2; return __CAST_C2L;
			case dtShort: casted = 2; return __CAST_S2L;
			case dtInt: return -1;
			case dtFloat: casted = 1; return __CAST_L2F;
			default:GenError(110);return -1;
			}
		}
	case dtInt:
		{
			switch (dt2.type)
			{
			case dtChar: casted = 2; return __CAST_C2I;
			case dtShort: casted = 2; return __CAST_S2I;
			case dtLong: return -1;
			case dtFloat: casted = 1; return __CAST_I2F;
			default:GenError(110);return -1;
			}
		}
	case dtShort:
		{
			switch (dt2.type)
			{
			case dtChar: casted = 2; return __CAST_C2S;
			case dtInt: casted = 1; return __CAST_S2I;
			case dtLong: casted = 1;  return __CAST_S2L;
			case dtFloat: casted = 1; return __CAST_S2F;
			default:GenError(110);return -1;
			}
		}
	case dtChar:
		{
			switch (dt2.type)
			{
			case dtShort: casted = 1;return __CAST_C2S;
			case dtInt: casted = 1; return __CAST_C2I;
			case dtLong: casted = 1;  return __CAST_C2L;
			case dtFloat: casted = 1; return __CAST_C2F;
			default:GenError(110);return -1;
			}
		}
	case 0:// 空操作数(dummy type)
		return -1;
	default:GenError(110);		
	}
	/* new code end.*/
///////////////////////////////
// code modify end. 2002-03-19 
///////////////////////////////

	GenError(110);
	return -1;
}


/*
   函数名称     : cParser::AllocTempVar
   函数功能	    : 分配一个临时变量
   变量说明     : 
   long type	: 变量的类型
   返回值       : 变量的地址, -1 表示失败
   编写人       : 居卫华
   完成日期     : 2001 - 6 - 8
*/
long cParser::AllocTempVar(long type, long reflevel)
{
	_typedes(dt1, type)
	dt1.refLevel = reflevel;

	long temp;
	if (m_pMainFunction->AddVal(NULL,  dt1))
	{			
		temp = m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount-1].address;		
		return temp;
	}
	else
		return -1;				
}

/*
   函数名称     : cParser::Cast
   函数功能	    : 自动类型转换
   变量说明     : 
   返回值       : 
   编写人       : 居卫华
   完成日期     : 2001 - 6 - 11
*/
void cParser::Cast(long &op1, long &type1, TYPEDES &dt1, long &op2, long &type2, TYPEDES &dt2)
{
			long castcode = -1; //cast操作码
			char casted = 0;	//被转换类型的运算数
			castcode = NeedCast(dt1, dt2, casted);
			if (castcode != -1)
			{
				//类型转换
				long temp;//临时变量地址, 存放转换后的变量
				long type;//目标类型
				long am;  //cast指令的寻址方式
				long* castedop = NULL;
				if (casted == 1)
				{
					type = dt2.type;
					castedop = &op1;
				}
				if (casted == 2)
				{
					type = dt1.type;
					castedop = &op2;
				}
				temp = AllocTempVar(type);
				if (temp == -1)
				{
					GenError(98);
				}
				else
				{
					am = 1 | (log2((int)(typesize(type, 0))<<6));
					am = am << 8;//temp的寻址方式
					if (casted == 1)
					{			
						long exchange = am>>8;						
						am |= (type1&0x00ff) /*| (((type2&0x30)<<8)&0xff00)*/;//整条指令的寻址方式
					ADDCOMMAND3(__cast, am, temp, *castedop, castcode)
						//修改运算数
						op1 = temp;
						type1 = exchange;
						dt1.type = type;
					}
					if (casted == 2)
					{
						long exchange = am >> 8;
						am |= (type2&0xff) /*| (((type1&0x30)<<8)&0xff00)*/;//整条指令的寻址方式					
					ADDCOMMAND3(__cast, am, temp, *castedop, castcode)
						//修改运算数
						op2 = temp;
						type2 = exchange;
						dt2.type = type;
					}
				}				
			}
}

BOOL cParser::AddClass(CClassDes& obj)
{
/*
	if (this->m_lClassDesNum == this->m_lClassDesTableSize)
	{
		CClassDes** pNewTable = NULL;

		pNewTable = new CClassDes*[m_lClassDesTableSize+50];
		if (pNewTable == NULL)
		{
			REPORT_MEM_ERROR("Memory allocation failed\r\n");
			return FALSE;
		}
		memcpy(pNewTable, this->m_classTable, sizeof(CClassDes*)*m_lClassDesNum);
		delete this->m_classTable;
		this->m_classTable = pNewTable;		
		m_lClassDesTableSize += 50;
	}
	m_classTable[m_lClassDesNum] = &obj;
	this->m_lClassDesNum++;
    m_pCurClassDes = &obj;
	return TRUE;
*/
    m_classTable->addClass(&obj);
	std::string msg = "add classes";
	debug(msg);
    return TRUE;

    return TRUE;

}
/*
long cParser::GetClassIdByName(char *szName)
{	

	int i;
	char * name;
	for (i = 0; i< m_lClassDesNum; i++)
	{
		name = (*m_classTable)[i]->GetFullName();
		if (!strcmp(name, szName))
			return i+1;
	}
	return 0;


}
	*/



void cParser::SetByteCodeFilePath(char* szPath)
{
	if (szPath == NULL)
		return;
	strcpy(m_szByteCodeFilePath, szPath);
	while (m_szByteCodeFilePath[strlen(m_szByteCodeFilePath) - 1] == PATH_SEPARATOR_C)
		m_szByteCodeFilePath[strlen(m_szByteCodeFilePath) - 1] = 0; 
}


cParser::cParser(CCompiler* c, AbsScanner *S, CRError *E) : CRParser(S,E)
{
	m_conf = NULL;
	
	m_LoopTree = new CLoopTree();

	m_pMainFunction = new CFunction;



	m_pExpOpPt = &m_ExpOp;
	m_ExpOp.pPrev = NULL; 


	m_curloop  = m_LoopTree;

	m_classTable = NULL;
	m_lClassDesNum = 0;
	m_lClassDesTableSize = 0;

	m_szByteCodeFilePath[0] = 0;
    m_pCurClassDes = NULL;
    memset(curFileName, 0, _MAX_PATH);

    this->c = c;
}

cParser::~cParser()
{

	/*if (this->m_classTable)
	{
		for (int i= 0; i < this->m_lClassDesNum; i++)
		{
			if (m_classTable[i])
				delete m_classTable[i];
		}
		delete m_classTable;
	}*/

	SAFEDELETE(m_LoopTree);
    SAFEDELETE(m_pMainFunction);
}


bool cParser::doMove(long type1, long type2, long op1, long op2, TYPEDES& dt1, TYPEDES& dt2 )
{

	//get address mode
	int address_mode = (type1<<8)|(short)type2;
	address_mode |= (log2(UnitSize(dt1))<<14)|(log2(UnitSize(dt2))<<6);

	if (dt1.type != dtGeneral && dt2.type != dtGeneral){

		//add commmand to command table
		ADDCOMMAND2(__mov, address_mode, op1, op2)      
		pushResultAX();
	}
	else{
		if (dt2.type == dtGeneral && dt1.type != dtGeneral) // can not assign object to primitive type
			this->GenError(11); // TODO
		//add commmand to command table
		ADDCOMMAND3(__movobj, address_mode, op1, op2, dt2.type)      
		m_pMainFunction->PushDigit(op1, AMODE_OBJ|0x80, dt1);
	}
	return true;
}


void cParser::doVarDecl(PTYPEDES type, char* szFirstName)
{
     long op;
	      long type1;
	      TYPEDES DT1;
	      if (!m_pMainFunction->PopDigit(&op, &type1, &DT1))
	      {
	              if (!AllocVar(type, szFirstName))
	                      GenError(113);
				  else{
					  long temp =  m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount-1].address;
					  m_pMainFunction->PushDigit(temp, AMODE_MEM|0x80, *type);
				  }
	      }
	      else
	      { 
	              if (type1 >=FIRST_ADDRESS_MODE || type1 >LAST_ADDRESS_MODE)
	              {
	                      //error
	              };
	              /*              int i;
	              BOOL bPop = TRUE;
	              while (!bPop)
	              {       
	              if (type1 != AMODE_DIRECT)
	              {
	              //error
	              }
	              for ( i= 0; i<op; i++0)
	              {
	              AllocVar(type, NULL);
	              }
	              bPop = m_pMainFunction->PopDigit(&op, type1);
	              }
	              */      

	              int dimsize[64];//数组最大维数64
	              int i = 0;
	              int arraysize = op;//total size
	              dimsize[i] = op;
	              i++;

	              while (m_pMainFunction->PopDigit(&op, &type1, &DT1))
	              {
	                      dimsize[i] = op;                        
	                      i++;
	                      arraysize *= op;
	              }
	              type->dim = i;
	              int k,j;
	              k = 0;
	              for (j=i-1; j>=0; j--)
	              {
	                      type->dimsize[j] = dimsize[k];
	                      k++;
	              }

	              if (!AllocArrayVar(type, szFirstName, arraysize))
	                      GenError(113);
				  else{
					  long temp =  m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount-1].address;
					  m_pMainFunction->PushDigit(temp, AMODE_MEM|0x80, DT1);
				  }
	      }
}
void cParser::pushResultAX(){
	//push result
	TYPEDES dt;//无效
	memset(&dt, 0, sizeof(TYPEDES));
	this->m_pMainFunction->PushDigit(_AX, 0x82,dt);
}
bool cParser::doAssign()
{
    //add command
    long op1, op2;
    long type1, type2;
    TYPEDES dt1, dt2;

    //pop
    if (!m_pMainFunction->PopDigit(&op2, &type2, &dt2) || !m_pMainFunction->PopDigit(&op1, &type1, &dt1) 
    || type1 <FIRST_ADDRESS_MODE || type1 >LAST_ADDRESS_MODE 
    || type2 <FIRST_ADDRESS_MODE || type2 >LAST_ADDRESS_MODE)
    {
    //      REPORT_COMPILE_ERROR("POP UP FAILED");
            GenError(96);
            return false;
    }
    else
    {
            //类型转换
            long castcode = -1; //cast操作码
            char casted = 0;        //被转换类型的运算数
            //类型转换
            long temp;//临时变量地址, 存放转换后的变量
            long type;//目标类型
            long level;
            long objid;
            long am;  //cast指令的寻址方式
            long* castedop = NULL;
            type = dt1.type;
            level = dt1.refLevel;
            castedop = &op2;
            objid = dt1.objID;
		    if (dt1.type != dtGeneral && dt2.type != dtGeneral) //get cast code
            {
                    TYPEDES DT1, DT2;
                    memcpy(&DT1, &dt1, sizeof(TYPEDES));
                    memcpy(&DT2, &dt2, sizeof(TYPEDES));
                    if ( (DT1.objID > 0 && DT2.objID ==0) || (DT2.objID > 0 && DT1.objID ==0))
                    {
                            GenError(110);
                            return false;
                    }

                    if (DT1.refLevel > 0)//是指针
                    {
                            if (DT2.refLevel == 0 // 不是指针
                                    && DT2.type != dtLong //不是长整型
                                    && DT2.type != dtInt)
                            {
                                    GenError(110);
                                    return false;
                            }
                            DT1.type = dtLong;                                      
                    }
                    if (DT2.refLevel > 0)
                    {
                            if (DT1.refLevel == 0 && DT1.type != dtLong &&DT1.type != dtInt)
                            {
                                    GenError(110);
                                    return false;
                            }
                            DT2.type = dtLong;                                      
                    } 
                    switch (DT2.type)
                    {
                    case dtChar:
                            {
                                    switch (DT1.type)
                                    {
                                    case dtChar:break;
                                    case dtShort:castcode = __CAST_C2S;break;
                                    case dtInt:castcode = __CAST_C2I;break;
                                    case dtLong:castcode = __CAST_C2L;break;
                                    case dtFloat:castcode = __CAST_C2F;break;
                                    default:GenError(110);return false;
                                    }                                               
                            }
                            break;
                    case dtShort:
                            {
                                    switch (DT1.type)
                                    {
                                    case dtChar:castcode = __CAST_S2C;break;
                                    case dtShort:break;
                                    case dtInt:castcode = __CAST_S2I;break;
                                    case dtLong:castcode = __CAST_S2L;break;
                                    case dtFloat:castcode = __CAST_S2F;break;
                                    default:GenError(110);return false;
                                    }                                               
                            }
                            break;
                    case dtInt:
                     {
                                                switch (DT1.type)
                                                {
                                                case dtChar:castcode = __CAST_I2C;break;
                                                case dtShort:castcode = __CAST_I2S;break;                                                     
                                                case dtFloat:castcode = __CAST_I2F;break;
                                                case dtInt:
                                                case dtLong:
                                                                                            case dtStr:break; // not need to cast

                                                default:GenError(110);return false;
                                                }                                               
                                        }
                            break;
                    case dtLong:
                            {
                                    switch (DT1.type)
                                    {
                                    case dtChar:castcode = __CAST_L2C;break;
                                    case dtShort:castcode = __CAST_L2S;break;
                                    case dtInt:break;
                                    case dtLong:break;
                                    case dtFloat:castcode = __CAST_L2F;break;
                                    default:GenError(110);return false;
                                    }                                               
                            }
                            break;
                    case dtFloat:
                            if (DT1.type != dtFloat) 
                                    GenError(110);
                            break;
                    } // switch
            } // if 
            temp = AllocTempVar(type, level);
            if (temp == -1)
            {
                    GenError(98);
            }
            else if (castcode != -1)
            {//需要转换

                    am = 1 | (log2(UnitSize(dt1))<<6);
                    am = am << 8;//temp的寻址方式
                    long exchange = am >> 8;
                    am |= (type2&0xff) /*| (((type2&0x30)<<8)&0xff00)*/;//整条指令的寻址方式                                        
                    ADDCOMMAND3(__cast, am, temp, *castedop, castcode)
                    //修改运算数
                    type2 = exchange;
                    dt2.type = type;
                    op2 = temp;
            }                               

            doMove(type1, type2, op1, op2, dt1, dt2);

     } // else
    return true;

}
// add by jackie juju
//////////////////////////////////
