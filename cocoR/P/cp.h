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

#ifndef cParser_INCLUDE
#define cParser_INCLUDE

#include <string>
#include <vector>
#include "cr_parse.hpp"
#include "../datatype.h"
#include "../ObjDes.h"
#include "cr_parse.hpp"
#include "Function.h"
#include "PubFuncTable.h"

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

class CObjDes;

typedef struct _tagExpressionDigit
{
	long digit;
	int  type;            //0: 常量 1: 变量地址 2:寄存器
	TYPEDES dtType;
	_tagExpressionDigit* pPrev;
}EXPRESSIONDIGIT;//表达式使用的操作数栈的节点

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
class CScriptFuncTable;
class CLoopTree;
class CFunction;
// add by jackie juju
/////////////////////////
const int MAXSYM = 5;
const int MAXERROR = 99;

class cParser : CRParser
{

// add by jackie
public:
	void SetByteCodeFilePath(char* szPath);
	long typesize(long type, long id);
	void Cast(long& op1, long& type1, TYPEDES& dt1, long& op2, long& type2, TYPEDES& dt2);
	long AllocTempVar(long type, long reflevel = 0);
	long NeedCast(TYPEDES dt1, TYPEDES dt2, char& casted);
	void ClearOpStack();
	CScriptFuncTable *m_ExeCodeTable;
	CPubFuncTable* m_PubFuncTable;

	int UnitSize(TYPEDES& type);                //实际数据类型的size
	void Init();                                       //初始化
	void SetPubFuncTable(CPubFuncTable* pTable);       //设置脚本将使用的基本函数表

private:

	//obect 描述表
	CObjDes ** m_ObjTable;				//定义的object的描述列表
	long m_lObjDesNum;				//定义的obj描述个数
	long m_lObjDesTableSize;		//列表大小
	BOOL AddObj(CObjDes& obj);
	long GetObjIDByName(char *szName);


	//运算数栈
	EXPRESSIONDIGIT m_ExpDigit;
   	EXPRESSIONDIGIT* m_pExpDigitPt;//栈指针

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
    cParser(AbsScanner *S=NULL, CRError *E=NULL) : CRParser(S,E) {};
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
  private:
    static void C(void);
    static void Definition(void);
    static void StorageClass(void);
    static void Type(PTYPEDES type);
    static void FunctionDefinition(void);
    static void VarList(PTYPEDES type, char* szFirstName);
    static void ClassDef(void);
    static void Inheritance(void);
    static void ClassBody(void);
    static void ArraySize(void);
    static void ConstExpression(void);
    static void FunctionHeader(void);
    static void FunctionBody(void);
    static void FormalParamList(void);
    static void CompoundStatement(void);
    static void FormalParameter(void);
    static void Statement(void);
    static void Label(void);
    static void AssignmentExpression(void);
    static void BreakStatement(void);
    static void ContinueStatement(void);
    static void DoStatement(void);
    static void ForStatement(void);
    static void IfStatement(void);
    static void NullStatement(void);
    static void ReturnStatement(void);
    static void SwitchStatement(void);
    static void WhileStatement(void);
    static void Expression(void);
    static void LocalDeclaration(void);
    static void Conditional(void);
    static void AssignmentOperator(void);
    static void LogORExp(void);
    static void LogANDExp(void);
    static void InclORExp(void);
    static void ExclORExp(void);
    static void ANDExp(void);
    static void EqualExp(void);
    static void RelationExp(void);
    static void ShiftExp(void);
    static void AddExp(void);
    static void MultExp(void);
    static void CastExp(void);
    static void UnaryExp(void);
    static void PostFixExp(void);
    static void UnaryOperator(void);
    static void Primary(void);
    static void FunctionCall(FUNCCALL* pFuncEntry);
    static void ActualParameters(FUNCCALL* pFuncEntry);
    
};

#endif /* cParser_INCLUDE */

