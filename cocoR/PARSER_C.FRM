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

/*************** NOTICE ****************
	This file is generated by cocoR       
***************************************/

#include "clib.h"
#include -->TokensHeader

-->UserDeclarations

#include -->ParserHeader
#include -->ScanHeader

unsigned short int -->ParserClass::SymSet[][MAXSYM] = {
  -->SymSets
  {0x0}
};
BOOL -->ParserClass::init(CClassDesTable *table, CPubFuncTable* pft, CConfigure* c){
	m_classTable = table;
	m_PubFuncTable = pft;
	m_conf = c;
	// show options
	showOptions();
	return TRUE;
}

void -->ParserClass::destroy(){

}
void -->ParserClass::Get()
{ do {
    Sym = Scanner->Get();//aa
    Scanner->NextSym.SetSym(Sym);
    if (Sym <= MAXT) Error->ErrorDist ++;
    else {
      -->Pragmas
      Scanner->NextSym = Scanner->CurrSym;
    }
  } while (Sym > MAXT);
}

void -->ParserClass::ExpectWeak(int n, int follow)
{ if (Sym == n) Get();
  else {
    GENERR(n);
    while (!(In(SymSet[follow], Sym) || In(SymSet[0], Sym))) Get();
  }
}

int -->ParserClass::WeakSeparator(int n, int syFol, int repFol)
{ unsigned short int s[MAXSYM];
  int i;

  if (Sym == n) { Get(); return 1; }
  if (In(SymSet[repFol], Sym)) return 0;

  for (i = 0; i < MAXSYM; i++)
    s[i] = SymSet[0][i] | SymSet[syFol][i] | SymSet[repFol][i];
  GENERR(n);
  while (!In(s, Sym)) Get();
  return In(SymSet[syFol], Sym);
}

-->Productions

void -->ParserClass::Parse()
{ Scanner->Reset(); Get();
  -->ParseRoot
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
					GENERR(124);
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
			default:GENERR(110);return -1;
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
			default:GENERR(110);return -1;
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
			default:GENERR(110);return -1;
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
			default:GENERR(110);return -1;
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
			default:GENERR(110);return -1;
			}
		}
	default:GENERR(110);		
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
			default:GENERR(110);return -1;
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
			default:GENERR(110);return -1;
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
			default:GENERR(110);return -1;
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
			default:GENERR(110);return -1;
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
			default:GENERR(110);return -1;
			}
		}
	case 0:// 空操作数(dummy type)
		return -1;
	default:GENERR(110);		
	}
	/* new code end.*/
///////////////////////////////
// code modify end. 2002-03-19 
///////////////////////////////

	GENERR(110);
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
					GENERR(98);
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


-->ParserClass::-->ParserClass(CCompiler* c, AbsScanner *S, CRError *E) : CRParser(S,E)
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

-->ParserClass::~-->ParserClass()
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


bool -->ParserClass::doMove(long type1, long type2, long op1, long op2, TYPEDES& dt1, TYPEDES& dt2 )
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
			this->GENERR(11); // TODO
		//add commmand to command table
		ADDCOMMAND3(__movobj, address_mode, op1, op2, dt2.type)      
		m_pMainFunction->PushDigit(op1, AMODE_OBJ|0x80, dt1);
	}
	return true;
}


void -->ParserClass::doVarDecl(PTYPEDES type, char* szFirstName)
{
     long op;
	      long type1;
	      TYPEDES DT1;
	      if (!m_pMainFunction->PopDigit(&op, &type1, &DT1))
	      {
	              if (!AllocVar(type, szFirstName))
	                      GENERR(113);
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
	                      GENERR(113);
				  else{
					  long temp =  m_pMainFunction->m_SymbolTable.tableEntry[m_pMainFunction->m_SymbolTable.m_nSymbolCount-1].address;
					  m_pMainFunction->PushDigit(temp, AMODE_MEM|0x80, DT1);
				  }
	      }
}
void -->ParserClass::pushResultAX(){
	//push result
	TYPEDES dt;//无效
	memset(&dt, 0, sizeof(TYPEDES));
	this->m_pMainFunction->PushDigit(_AX, 0x82,dt);
}
bool -->ParserClass::doAssign()
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
            GENERR(96);
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
                            GENERR(110);
                            return false;
                    }

                    if (DT1.refLevel > 0)//是指针
                    {
                            if (DT2.refLevel == 0 // 不是指针
                                    && DT2.type != dtLong //不是长整型
                                    && DT2.type != dtInt)
                            {
                                    GENERR(110);
                                    return false;
                            }
                            DT1.type = dtLong;                                      
                    }
                    if (DT2.refLevel > 0)
                    {
                            if (DT1.refLevel == 0 && DT1.type != dtLong &&DT1.type != dtInt)
                            {
                                    GENERR(110);
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
                                    default:GENERR(110);return false;
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
                                    default:GENERR(110);return false;
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

                                                default:GENERR(110);return false;
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
                                    default:GENERR(110);return false;
                                    }                                               
                            }
                            break;
                    case dtFloat:
                            if (DT1.type != dtFloat) 
                                    GENERR(110);
                            break;
                    } // switch
            } // if 
            temp = AllocTempVar(type, level);
            if (temp == -1)
            {
                    GENERR(98);
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
