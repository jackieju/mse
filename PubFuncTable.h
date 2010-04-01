// PubFuncTable.h: interface for the CPubFuncTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBFUNCTABLE_H__E9E34579_3E3D_40FF_8CFE_2404A7E276C5__INCLUDED_)
#define AFX_PUBFUNCTABLE_H__E9E34579_3E3D_40FF_8CFE_2404A7E276C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define MAX_FUNCNAME_LENGTH 128

//��Ź��ú�����Ϣ�Ľṹ
typedef struct _tagFunctionEntry
{
	long pfn;                       //����ָ��
	char szName[MAX_FUNCNAME_LENGTH];  //������
	/*unsigned int uMaxParamNum;	       //���������� 
	unsigned int uMinParamNum;         //���ٲ�������*/
	//��ʱ�涨���еĺ���������������һ��, ����֧�ֲ������������ĺ���
	char cParamNum;                    //��������
}FUNCTIONENTRY;
#define FUNCNUM 20

class CPubFuncTable  
{
	int m_iFuncNum;
	int m_iMaxFuncNum;
	std::vector<long>	m_libs;

public:
	CPubFuncTable();
	virtual ~CPubFuncTable();
	FUNCTIONENTRY *m_FuncTable;	
	
	int FindFuncByName(char* szName);
	BOOL AddFunction(long pfn, char* szName, char cParamNum);
	BOOL AddPubFunction(HMODULE hDll, char* fnName, char* szName, char cParamNum);
	long LoadLib(char *szDLLName, char* szFileName);
	
};

#endif // !defined(AFX_PUBFUNCTABLE_H__E9E34579_3E3D_40FF_8CFE_2404A7E276C5__INCLUDED_)
