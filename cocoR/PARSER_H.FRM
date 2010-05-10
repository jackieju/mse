//****************************************************************
//   CPLUS2\PARSER_H.FRM
//   Coco/R C++ Support Frames.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   Jun 12, 1996  Version 1.06
//      Many fixes and suggestions thanks to
//      Pat Terry <p.terry@.ru.ac.za>
//   May 05, 1999  Version 1.12
//      Added methods to parser to retrieve token position easily
//****************************************************************

#ifndef -->ParserClass_INCLUDE
#define -->ParserClass_INCLUDE

#include "clib.h"
#include <string>
#include <vector>
#include "cr_parse.hpp"
#include "../datatype.h"
#include "cr_parse.hpp"
#include "Function.h"
#include "PubFuncTable.h"
#include "ClassDesTable.h"
#include "Configure.h"
#include "cp_util.h"
//////////////////////////	
// add by jackie juju
#define MAX_IDENTIFIER_LENGTH 64

typedef struct _tagFUNCCALL {
        std::string     name;           // function name
        std::vector<long>       params;         // param list, the first element is return type
        union {
                CFunction *pVF;                         // script function entry
                FUNCTIONENTRY*          pfn;            // native function entry 
        };
        int     nType; // 0: script function 1: native function
}FUNCCALL;

class CClassDes;



//register
#define _AX        1
#define _BX        2
#define _CX        3
#define _DX        4
#define _PSW       0

typedef struct _tagExpressionOp
{
	int opcode;
	_tagExpressionOp *pPrev;
}EXPRESSIONOP;//表达式栈使用的操作栈的节点
#define ADDCOMMAND0(code){\
	long _op_[4];\
	memset(_op_, 0, 4*sizeof(long));\
	m_pMainFunction->AddCommand(code, 0, 0, _op_, Scanner->CurrSym.Line);}

#define ADDCOMMAND1(code, address_mode, param){\
	long _op_[4];\
	memset(_op_, 0, 4*sizeof(long));\
	_op_[0] = (long)param;\
	m_pMainFunction->AddCommand(code, address_mode, 1, _op_, Scanner->CurrSym.Line);}

#define ADDCOMMAND2(code, address_mode, param1, param2){\
	long _op_[4];\
	memset(_op_, 0, 4*sizeof(long));\
	_op_[0] = (long)param1;\
	_op_[1] = (long)param2;\
	m_pMainFunction->AddCommand(code, address_mode, 2, _op_, Scanner->CurrSym.Line);}

#define ADDCOMMAND3(code, address_mode, param1, param2, param3){\
	long _op_[4];\
	memset(_op_, 0, 4*sizeof(long));\
	_op_[0] = (long)param1;\
	_op_[1] = (long)param2;\
	_op_[2] = (long)param3;\
	m_pMainFunction->AddCommand(code, address_mode, 3, _op_, Scanner->CurrSym.Line);}

class CPubFuncTable;
class CLoopTree;
class CFunction;
class CCompiler;
// add by jackie juju
/////////////////////////
const int MAXSYM = -->MaxSymSets;
const int MAXERROR = -->MaxErr;

class -->ParserClass : public CRParser, public CPUtil
{

// add by jackie
public:
	void SetByteCodeFilePath(char* szPath);
	void Cast(long& op1, long& type1, TYPEDES& dt1, long& op2, long& type2, TYPEDES& dt2);
	long AllocTempVar(long type, long reflevel = 0);
	long NeedCast(TYPEDES dt1, TYPEDES dt2, char& casted);
	void ClearOpStack();

	CPubFuncTable* m_PubFuncTable;

	BOOL init(CClassDesTable *table, CPubFuncTable* pft, CConfigure* c=NULL);
	void destroy();

	int UnitSize(TYPEDES& type);                //实际数据类型的size
	//void Init(CClassTable *table, CPubFuncTable* ptf);                                       //初始化
	void SetPubFuncTable(CPubFuncTable* pTable);       //设置脚本将使用的基本函数表
    CClassDes *getCurClassDes(){
                    return m_pCurClassDes;
                    }

    void setSourceFileName(char* szName){
            strcpy(curFileName, szName);
    }

    char* GetCurrSym(){
		szCurrSymName[0] = 0;
		Scanner->GetName(&Scanner->CurrSym, szCurrSymName, MAX_IDENTIFIER_LENGTH-1);//得到名称;
		return szCurrSymName;
	}
    char* GetNextSym(){
		szNextSymName[0] = 0;
		Scanner->GetName(&Scanner->CurrSym, szNextSymName, MAX_IDENTIFIER_LENGTH-1);//得到名称;
		return szNextSymName;
	}
	
	void setConfig(CConfigure* p){
		m_conf = p;
		if (p)
			showOptions();
	}
	void showOptions(){
		if (m_conf==NULL){
			printf("Option is empty\n");
			return;
		}
		printf("debug=%s\n", m_conf->get("debug").c_str());
	}
private:

    CCompiler *c; // used to load other object when parsing current source
    char szCurrSymName[MAX_IDENTIFIER_LENGTH];
    char szNextSymName[MAX_IDENTIFIER_LENGTH];

	// class 描述表
	CClassDesTable *m_classTable;
    CClassDes *m_pCurClassDes;

	long m_lClassDesNum;				//定义的class描述个数
	long m_lClassDesTableSize;		//列表大小
	BOOL AddClass(CClassDes& obj);
	//long GetClassIdByName(char *szName);

    char curFileName[_MAX_PATH];  // current source file name

	CConfigure* m_conf; // options

	//一元操作符栈
	EXPRESSIONOP m_ExpOp;
	EXPRESSIONOP* m_pExpOpPt;//栈指针

	CLoopTree* m_curloop;
	CLoopTree* m_LoopTree;
	CFunction* m_pMainFunction;   

	char m_szByteCodeFilePath[_MAX_PATH];

	char* AnalyzeConstString(char* string);
	BOOL AllocArrayVar(PTYPEDES type,char* szName, int size);
	void ExitCurLoop();
	void AddNewLoop();
	int GetSymAddress(char* szName);

	void ClearExpStack();
	BOOL PopDigit(long *digit, long *type, TYPEDES *dtType);
	void PushDigit(long digit, long type, TYPEDES dtType);
	BOOL AllocVar(PTYPEDES type, char* szName);

	BOOL PopOp(int* op);    //pop一元操作符
	void PushExpOp(int op); //push一元操作符
// <-  add by jackie

  public:
    -->ParserClass(CCompiler* c, AbsScanner *S=NULL, CRError *E=NULL);// : CRParser(S,E) {};
    ~-->ParserClass();
    void Parse();
    inline void LexString(char *lex, int size)
    { Scanner->GetString(&Scanner->CurrSym, lex, size); };
    inline void LexName(char *lex, int size)
    { Scanner->GetName(&Scanner->CurrSym, lex, size); };
    inline long LexPos()
    { return Scanner->CurrSym.GetPos(); };
    inline void LookAheadString(char *lex, int size)
    { Scanner->GetString(&Scanner->NextSym, lex, size); };
    inline void LookAheadName(char *lex, int size)
    { Scanner->GetName(&Scanner->NextSym, lex, size); };
    inline long LookAheadPos()
    { return Scanner->NextSym.GetPos(); };
    inline int Successful()
    { return Error->Errors == 0; }

  protected:
    static unsigned short int SymSet[][MAXSYM];
    virtual void Get();
    void ExpectWeak (int n, int follow);
    int  WeakSeparator (int n, int syFol, int repFol);
	void debug(std::string s){
		if (m_conf){
			const char* d = m_conf->get("debug").c_str();
			if (!stricmp(d, "yes"))
				printf("%s\n", s);
		}
	}
	
  private:
    -->ProdHeaders

// added by jackie
private: // function to generate code
	bool doAssign();
	bool doMove(long type1, long type2, long op1, long op2, TYPEDES& dt1, TYPEDES& dt2 );
	void pushResultAX();
	void doVarDecl(PTYPEDES type, char* szFirstName);
//<--
};

#endif /* -->ParserClass_INCLUDE */

