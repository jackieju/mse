// Function.h: interface for the CFunction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUNCTION_H__521D16E5_8E52_45FD_98E2_6A6DD4F00749__INCLUDED_)
#define AFX_FUNCTION_H__521D16E5_8E52_45FD_98E2_6A6DD4F00749__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "COMMONDEF.H"
#include "SymbolTable.h"

/*typedef struct _tagData
{
	long size_t;              //ʹ�õĳ���
	long MaxSize_t;           //�ܳ���
	unsigned char *m_pData;   //���ݿռ�
}DATA;
*/
typedef struct _tagExpressionDigit
{
	long digit;
	int  type;            //0: ���� 1: ������ַ 2:�Ĵ���
	TYPEDES dtType;
	_tagExpressionDigit* pPrev;
}EXPRESSIONDIGIT;//���ʽʹ�õĲ�����ջ�Ľڵ�

#define CMDTABLEINITSIZE 20                //ָ������ĳ�ʼ����
#define DATASEGMENTINITSIZE 30             //���ݶεĳ�ʼ����

class CSymbolTable;
class CFunction  
{
public:
	static long OutCmd(COMMAND* pcmd, char *buffer);
	long OutupCmd(long index, char* buffer);           //���ָ������
	bool AddCommand(COMMAND& cmd);                     //���ָ��
	int GetSymbolAddress(char* szName);                //��������ȡ���ŵ�ַ 
	void Output(char* szName);                         //�������
	bool AddCommand(long opcode, long address_mode, long opnum, long *opTable, long lineNum);//���ָ��
	bool AddVal(char* szName, TYPEDES type);           //����ű���ӱ���
	int AddStaticData(int size_t, unsigned char* pData);       //����ű���ӳ���
	CFunction();
	virtual ~CFunction();

//	void SetAX(long val);
//	BOOL MoveCmd(unsigned int uFrom, unsigned int uTo);//����ʹ�øù��ܣ� ��Ϊ���ᵼ����ת������
	//long AX, BX, CX, DX;                    //�Ĵ���
	
#define VM_MODE_NORMAL 0
#define VM_MODE_DEBUG  1
#define VM_MODE_STEPDEBUG 2
	int m_nWorkMode;
	
	int m_iParamNum;					//��������
	int m_iParamTotalSize;				//�����ռ䳤��
	char m_szName[21];					//��������

	PCOMMAND m_pCmdTable;				   //ָ���
	int m_nCurrentCmdNum;                  //��ǰ��������
	int m_nCmdTableSize;                   //ָ������ĵ�ǰ��󳤶�
	
	CSymbolTable m_SymbolTable;           //���ݶη��ű�

//	CSymbolTable m_pointTable;            //ָ��������ű�

	unsigned char* m_staticSegment;                 //������
	int   m_nSSUsedSize;                     //��ʹ�õĳ���
	int   m_nSSCurMaxSize;                   //Ŀǰ�����ݿ��С
#define STATICSEGINCSIZE  1024       //�����ε�����

//	DATA m_dataSegment;                   //���ݶ�
	
//	DATA m_StackSegment;                  //��ջ��


public:
		//������ջ
	EXPRESSIONDIGIT m_ExpDigit;
   	EXPRESSIONDIGIT* m_pExpDigitPt;//ջָ��
	/*
   ��������     : cParser::ClearExpStack
   ��������	    : ��ձ��ʽջ
   ����˵��     : 
   ����ֵ       : 
   ��д��       : ������
   �������     : 2001 - 4 - 17
*/
void ClearExpStack()
{
	while (m_pExpDigitPt->pPrev != NULL)
	{
		EXPRESSIONDIGIT *pTemp = m_pExpDigitPt;
		this->m_pExpDigitPt = this->m_pExpDigitPt->pPrev; 		
		delete pTemp;
	}
	m_pExpDigitPt = &m_ExpDigit;
}
BOOL PopDigit(long *digit, long *type, TYPEDES *dtType)
{
	if (digit == NULL || type == NULL)
		return FALSE;
	*digit = 0;
	*type = -1;
	memset(dtType, 0, sizeof(TYPEDES));

	if (this->m_pExpDigitPt->pPrev == NULL)
		return FALSE;
	*digit = m_pExpDigitPt->digit;
	*type = m_pExpDigitPt->type;
	memcpy(dtType, &(m_pExpDigitPt->dtType), sizeof(TYPEDES));
	EXPRESSIONDIGIT *pTemp = m_pExpDigitPt;
	m_pExpDigitPt = m_pExpDigitPt->pPrev;
	delete pTemp;

//	printf("pop %04x, %04x, line:%04x\r\n", *digit, *type, __LINE__);

	return TRUE;
}

void PushDigit(long digit, long type, TYPEDES dtType)
{
	if (dtType.type < dtFirstType || dtType.type > dtLastType)
	{
	//	GenError(125);
		return;
	}
	EXPRESSIONDIGIT *pNew = new EXPRESSIONDIGIT;	
	if (pNew == NULL)
	{
		REPORT_MEM_ERROR("memory allocation error");
		return;
	}
	pNew->digit = digit;
	pNew->type = type;
	memcpy(&pNew->dtType, &dtType, sizeof(TYPEDES));
	pNew->pPrev = this->m_pExpDigitPt;
//	printf("push %04x, %04x, line:%04x\r\n", digit, type, __LINE__);
	m_pExpDigitPt = pNew;
}
};

#endif // !defined(AFX_FUNCTION_H__521D16E5_8E52_45FD_98E2_6A6DD4F00749__INCLUDED_)

