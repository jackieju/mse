// Function.cpp: implementation of the CFunction class.
//
//////////////////////////////////////////////////////////////////////

//#include "StdAfx.h"
#include "clib.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "memory.h"
#include "os/css_lockex.H"

#include "PubFuncTable.h"
#include "cocor/cp.hpp"
#include "Function.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFunction::CFunction()
{
/*	m_dataSegment.m_pData = new unsigned char[1024];
	m_dataSegment.MaxSize_t = 1024;
	m_dataSegment.size_t = 0;
*/
	this->m_nCmdTableSize = 0;
	this->m_nCurrentCmdNum = 0;
	this->m_pCmdTable = NULL;
	this->m_staticSegment = NULL;
	this->m_nSSCurMaxSize = 0;
	this->m_nSSUsedSize = 0;
	this->m_iParamTotalSize = 0;
	m_iParamNum = 0;
	m_nWorkMode = 0;
	m_pExpDigitPt = &m_ExpDigit;
	m_ExpDigit.pPrev = NULL;

}

CFunction::~CFunction()
{
	if (m_pCmdTable != NULL)
		delete this->m_pCmdTable;
	if (this->m_staticSegment)
		delete m_staticSegment;
}


/*
   函数名称     : CFunction::AddVal
   函数功能	    : 为变量分配内存
   变量说明     : 
   返回值       : 指针在变量表中的index
   编写人       : 居卫华
   完成日期     : 2001 - 4 - 12
*/
bool CFunction::AddVal(char* szName, TYPEDES type)
{
	return m_SymbolTable.AddSymbol(szName, type);	
}
int CFunction::AddStaticData(int size_t, BYTE* pData)
{

	int size16;//按16bit字对齐后的大小
	size16 = size_t + (4 - size_t%4);

	int start = m_nSSUsedSize;

	//如果数组不够长
	while (m_nSSUsedSize + size16 > this->m_nSSCurMaxSize)
	{  //if full
		BYTE* pNew = new BYTE[m_nSSCurMaxSize + STATICSEGINCSIZE];
		if (pNew == NULL)
		{
			REPORT_MEM_ERROR("interal error: can not allocate more symbol table");
			return FALSE;
		}
		memset(pNew, 0, m_nSSCurMaxSize + STATICSEGINCSIZE);
		memcpy(pNew, this->m_staticSegment, this->m_nSSUsedSize);
		delete this->m_staticSegment;
		m_staticSegment = pNew;

		m_nSSCurMaxSize += STATICSEGINCSIZE;
	}

	// add

	if ((BYTE*)m_staticSegment+start+size_t - m_staticSegment > m_nSSCurMaxSize)
		return FALSE;

	memcpy((void*)((BYTE*)m_staticSegment+start), (void*)pData, size_t);
	
	this->m_nSSUsedSize += size16;	
	return start;
}



bool CFunction::AddCommand(long opcode, long address_mode, long opnum, long *opTable, long lineNum)
{
	if (this->m_nCurrentCmdNum == this->m_nCmdTableSize)
	{
		COMMAND* pNewTable = NULL;
		//char* t = new char[65536];
		pNewTable = new COMMAND[m_nCmdTableSize+50];
		if (pNewTable == NULL)
		{
			REPORT_MEM_ERROR("Memory allocation failed\r\n");
			return FALSE;
		}
		memcpy(pNewTable, this->m_pCmdTable, sizeof(COMMAND)*m_nCurrentCmdNum);
		delete m_pCmdTable;
		m_pCmdTable = pNewTable;		
		m_nCmdTableSize += 50;
	}

	COMMAND *pNewCmd = &m_pCmdTable[this->m_nCurrentCmdNum];	
	pNewCmd->opcode = opcode;
	pNewCmd->address_mode = address_mode;
	pNewCmd->opnum = (char)opnum;
	pNewCmd->line = lineNum;

	for (int i = 0; i< opnum; i++)
	{
		pNewCmd->op[i] = opTable[i];
	}
	m_nCurrentCmdNum++;
	return TRUE;
}

void CFunction::Output(char *szName)
{
	int i;
	FILE *file = NULL;

	char cpath[_MAX_PATH] = "";
	getcwd(cpath, _MAX_PATH-1);
	JUJU::CheckPath(szName);
	file = fopen(szName, "w+");
	if (file == NULL)
	{
		printf("can not open file %s, current path:%s, errno %d\n", szName, cpath, errno);
		return;
	}
//	char *temp = new char[65536];
//	memset(temp, 0, 65536);
#ifdef WIN32
	__try
	{
#endif

	fprintf(file, "************************command table*********************\r\n");
	fprintf(file, "index   mode   line   code    op1   op2   op3\r\n");

#ifdef WIN32
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		unsigned long err;
		err = GetLastError();
		printf("fprintf has error: %d\r\n", err);
		fclose(file);
		return;
	}
#endif
	//output command table
	char buffer[1024];
	for (i = 0; i < this->m_nCurrentCmdNum; i++)
	{
		OutupCmd(i, buffer);
		fprintf(file, buffer);
	}

	fprintf(file, "************************segment*********************\r\n");
	fprintf(file, "index offset size name\r\n");

	//output datasegment 
	for (i = 0; i < this->m_SymbolTable.m_nSymbolCount; i++)
	{
		fprintf(file, "%04x: %04d,    %x,    %s\r\n", i, this->m_SymbolTable.tableEntry[i].address, this->m_SymbolTable.tableEntry[i].size_t, m_SymbolTable.tableEntry[i].szName);
	}
	fclose(file);
}

/*
void CFunction::SetAX(long val)
{
//	this->AX = val;
}
*/

int CFunction::GetSymbolAddress(char *szName)
{
	return this->m_SymbolTable.GetSymAddress(szName);
}

/*bool CFunction::MoveCmd(unsigned int uFrom, unsigned int uTo)
{
	if (uFrom > this->m_nCurrentCmdNum || uTo >m_nCurrentCmdNum)
		return FALSE;
	COMMAND command = m_pCmdTable[uFrom];
	for (int i = uFrom+1; i <= uTo; i++)
	{
		this->m_pCmdTable[i-1] = m_pCmdTable[i];
	}
	m_pCmdTable[uTo] = command;
	return TRUE;
}
*/
bool CFunction::AddCommand(COMMAND &cmd)
{

/*	if (this->m_nCurrentCmdNum == this->m_nCmdTableSize)
	{
		COMMAND* pNewTable = new COMMAND[this->m_nCmdTableSize+50];
		memcpy(pNewTable, this->m_pCmdTable, sizeof(COMMAND)*m_nCmdTableSize);
		delete m_pCmdTable;
		m_pCmdTable = pNewTable;		
	}

	COMMAND *pNewCmd = &m_pCmdTable[this->m_nCurrentCmdNum];	
	pNewCmd->opcode = cmd.opcode;
	pNewCmd->address_mode = cmd.address_mode;
	pNewCmd->opnum = cmd.opnum;
	pNewCmd->line = lineNum;

	for (int i = 0; i< cmd.opnum; i++)
	{
		pNewCmd->op[i] = cmd.op[i];
	}
	m_nCurrentCmdNum++;
	return TRUE;
*/
	return AddCommand(cmd.opcode, cmd.address_mode, cmd.opnum, cmd.op, cmd.line);
}

/*bool CFunction::AddPtVal(char *szName, int size_t)
{
//	return this->m_pointTable.AddSymbol(szName, size_t);	
	TYPEDES dt;
	dt.dim = 0;
	dt.type = dtInt;
	return m_SymbolTable.AddSymbol(szName, size_t, dt);	

}
*/

long CFunction::OutupCmd(long index, char *buffer)
{
	if (buffer == NULL)
		return 0;
		int j;	
		char cmd[20];
		char* p = buffer;

		int size = sprintf(p, "%04x: mode=%04x line=%04x opcode = %04x  ", index, this->m_pCmdTable[index].address_mode, this->m_pCmdTable[index].line, this->m_pCmdTable[index].opcode);
		p += size;
		memset(cmd, 0, 10);
		switch (m_pCmdTable[index].opcode)
		{
		case 0x1070: sprintf(cmd, "mul");break;
		case 0x1010: sprintf(cmd, "mov");break;
		case 0x1021: sprintf(cmd, "add");break;
		case 0x1090: sprintf(cmd, "div");break;
		case 0x10a0: sprintf(cmd, "jmp");break;
		case 0x10d0: sprintf(cmd, "not");break;
		case 0x1050: sprintf(cmd, "sub");break;
		case 0x10a1: sprintf(cmd, "jz");break;
		case 0x10a2: sprintf(cmd, "jnz");break;
		case 0x10f2: sprintf(cmd, "test");break;
		case 0x10f3: sprintf(cmd, "ret");break;
		case 0x10a3: sprintf(cmd, "callpub");break;
		case 0x10a4: sprintf(cmd, "parampub");break;
		case 0x10a5: sprintf(cmd, "endcallpub");break;
		case 0x10f4: sprintf(cmd, "ea");break;
		case 0x10f5: sprintf(cmd, "notr"); break;
	}
	size = sprintf(p, "   %s(", cmd);	
	p += size;
	for (j = 0; j < this->m_pCmdTable[index].opnum; j++)
	{
			if (j != 0)
			{
				sprintf(p, ",  ");
				p += 3;
			}
			size = sprintf(p, "%04x", m_pCmdTable[index].op[j]);
			p += size;
	}
	size = sprintf(p, ")\r\n");
	p += size;

	return strlen(buffer);
}

long CFunction::OutCmd(COMMAND *pcmd, char *buffer)
{
	//validatae
	if (pcmd == NULL || buffer == NULL)
		return 0;
	
	int j;	
	char cmd[10];
	char* p = buffer;
	
	int size = sprintf(p, "mode=%04x line=%04x opcode = %04x  ", pcmd->address_mode, pcmd->line, pcmd->opcode);
	p += size;
	memset(cmd, 0, 10);
	switch (pcmd->opcode)
	{
	case 0x1070: sprintf(cmd, "mul");break;
	case 0x1010: sprintf(cmd, "mov");break;
	case 0x1021: sprintf(cmd, "add");break;
	case 0x1090: sprintf(cmd, "div");break;
	case 0x10a0: sprintf(cmd, "jmp");break;
	case 0x10d0: sprintf(cmd, "not");break;
	case 0x1050: sprintf(cmd, "sub");break;
	case 0x10a1: sprintf(cmd, "jz");break;
	case 0x10a2: sprintf(cmd, "jnz");break;
	case 0x10f2: sprintf(cmd, "test");break;
	case 0x10f3: sprintf(cmd, "ret");break;
	case 0x10a3: sprintf(cmd, "callpub");break;
	case 0x10a4: sprintf(cmd, "parampbu");break;
	case 0x10a5: sprintf(cmd, "endcallpub");break;
	case 0x10f4: sprintf(cmd, "ea");break;
	}
	size = sprintf(p, "   %s(", cmd);	
	p += size;
	for (j = 0; j < pcmd->opnum; j++)
	{
		if (j != 0)
		{
			sprintf(p, ",  ");
			p += 3;
		}
		size = sprintf(p, "%04x", pcmd->op[j]);
		p += size;
	}
	size = sprintf(p, ")\r\n");
	p += size;

	return strlen(buffer);


}
