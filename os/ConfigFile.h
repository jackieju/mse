/**
 * @author  ����
 * @author Zhang Yuan
 * @package SSLib
 * @version 1.0.0
 */

//
//////////////////////////////////////////////////////////////////////

#ifndef __CONFIGFILE_H__
#define __CONFIGFILE_H__

#define COMMONLIB_GROUP	"Group"
#define COMMONLIB_GROUPNAMELEN	100
#define COMMONLIB_ITEMNAMELEN	100
#define COMMONLIB_FILEPATH	256

#include "osmacros.h"
#include <vector>

typedef struct comment
{
	char *sLine;
	long nLineNo;

	comment()
	{
		sLine = NULL;
		nLineNo = -1;
	}
} Comment;

//typedef struct ConfigVar  ConfigVar;
typedef struct ConfigVar 
{
	struct ConfigVar *pNext;
	char sName[COMMONLIB_ITEMNAMELEN + 1];
	char *sValue;
	long nLineNo;
	char *sComment;

	ConfigVar()
	{
		pNext = 0;
		*sName = 0;
		sValue = NULL;
		sComment = NULL;
		nLineNo = 0;
	}

	~ConfigVar()
	{
		if(sValue)
		{
			delete []sValue;
			sValue = NULL;
		}

		if (sComment)
		{
			delete sComment;
			sComment = NULL;
		}
	}

} ConfigVar;

//typedef struct ConfigGroup ConfigGroup;
typedef struct ConfigGroup
{
	char sGroupName[COMMONLIB_GROUPNAMELEN + 1];
	struct ConfigGroup *pNext;
	ConfigVar *pVarList, *pLastVar;
	long nLineNo;
	char *sComment;

	ConfigGroup()
	{
		*sGroupName = 0;
		pNext = NULL;
		pVarList = NULL;
		pLastVar = NULL;

		nLineNo = 0;
		sComment = NULL;
	}

	~ConfigGroup()
	{
		if (sComment)
		{
			delete sComment;
			sComment = NULL;
		}
	}
} ConfigGroup;

//typedef struct Config Config;
typedef struct Config 
{
	char sFileName[COMMONLIB_FILEPATH + 1];
	ConfigGroup *pGroupList, *pLastGroup;

	Config()
	{
		pGroupList = NULL;
		pLastGroup = NULL;
		sFileName[0] = 0;
	}
} Config;

using namespace std;

/**
 * Config File class
 */
class OSLIB_DLL_FUNC_HEAD CConfigFile  
{
	typedef vector<Comment *> CommentVector;

	CommentVector m_CommentVector;

public:
	enum COMMONLIB_ERRINFO{
		errInvalideParameter = 1024,
		errAllocateMemory,
		errInitFirst,
		errFindFirst,
		errNotFound
	};
private:
	long m_nLineNumber;
	Config	m_Config;			// �����ļ����б�
	ConfigGroup	*m_pCurrentGroup;
public:

   /**
    * <pre>
	����������	int CConfigFile::Get(int iIndex)
	�������ܣ�	ȡ�õ�ǰ���������ֵ
	����˵����	[IN]int iIndex		- �����������
	����ֵ��	�Ƿ�ɹ�
	* </pre>
	*/
	char* Get(int iIndex);

	/**
	 * <pre>
	����������	int CConfigFile::Get(char *sItemName)
	�������ܣ�	ȡ�õ�ǰ���������ֵ
	����˵����	[IN]char *sItemName		- �����������
	����ֵ��	�Ƿ�ɹ�
	* </pre>
	*/
	char* Get(char *sItemName);


	/**
	 * <pre>
	����������long SetValueByName(char *sItemName, char *sItemValue, char *sComment = NULL)
	�������ܣ�	���FindFirstGroup��FindNextGroupʹ�����ı�ĳ���������ֵ��
	����˵����	[IN]char *sItemName - ������Ŀ����
				[IN]char *sItemValue - �������ֵ�����ۺ������Ͷ���char *��ʽ���롣
				[IN]char *sComment - ����Ŀ��ע�ͣ�ȱʡΪNULL��
	����ֵ��	<0 -- ʧ��  0 -- �޸ĳɹ� 1 -- ��ӳɹ�
	 * </pre>
	 */
	long SetValueByName(char *sItemName, char *sItemValue, char *sComment = NULL);

	/**
	 * <pre>
	����������	int CConfigFile::FindFirstGroup(char *sGroupName)
	�������ܣ�	ȡ�õ�һ��
	����˵����	[IN]char *sGroupName		- �����������
	����ֵ��	�Ƿ�ɹ�
	* </pre>
	*/
	int FindFirstGroup(char *sGroupName);

	/**
	 * <pre>
	����������	int CConfigFile::FindNextGroup()
	�������ܣ�	ȡ����һ��
	����˵����	��
	����ֵ��	�Ƿ�ɹ�
	* </pre>
	*/
	int FindNextGroup();

	/**
	 * <pre>
	����������	void CConfigFile::Dump()
	�������ܣ�	��������Ϣ��ʾ��console
	����˵����	��
	����ֵ��	��
	* </pre>
	*/
	void Dump();

	/**
	 * <pre>
	����������	long Save()
	�������ܣ�	��������Ϣ���浽����
	����˵����	��
	����ֵ��	0 - failure, 1 - success
	* </pre>
	*/
	long Save();

	/**
	 * <pre>
	����������	void CConfigFile::Destroy()
	�������ܣ�	�ͷ���Դ
	����˵����	��
	����ֵ��	��
	* </pre>
	*/
	void Destroy();

	/**
	 * <pre>
	����������	long CConfigFile::Init(char *sFileName, char *sDefaultFileName)
	�������ܣ�	��ʼ��
	����˵����	[IN]char *sFileName				- �����ļ�
	[IN]char *sDefaultFileName		- ȱʡ�����ļ�
	����ֵ��	�Ƿ�ɹ�
	* </pre>
	*/
	long Init(char *sFileName, char *sDefaultFileName = 0);

	/**
	 * <pre>
	����������	CConfigFile::CConfigFile()
	�������ܣ�	���캯��
	����˵����	��
	����ֵ��	��
	* </pre>
	*/
	CConfigFile();

	/**
	 * <pre>
	����������	CConfigFile::~CConfigFile()
	�������ܣ�	��������
	����˵����	��
	����ֵ��	��
	* </pre>
	*/
	~CConfigFile();

private:
	char* ParseValue(char* sValue);
	void AddItemToGroup(ConfigGroup* pGroup,ConfigVar* pVar);
	ConfigVar* FindItem(ConfigGroup* pGroup,char* sItemName);
	long SetValue(ConfigGroup *pGroup, char *sItemName, char *sItemValue, long nLineNo = -1, 
		char *sComment = NULL, long nInit = 0);
	ConfigGroup *AddGroup(char *sGroupName, long nLineNo, char *pComment);
	char* Trim(char* sString);
};

#endif // !defined(AFX_CONFIGFILE_H__C623FC08_DDD8_4ACE_BB05_EDB736F73795__INCLUDED_)
//:~
