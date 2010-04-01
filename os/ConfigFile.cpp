// ConfigFile.cpp: implementation of the CConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ConfigFile.h"


#ifndef SAFEDELETE
#define SAFEDELETE(x) \
	do { \
		if ((x) != NULL) \
		{ \
			delete (x); \
		} \
		(x) = NULL; \
		}while(0)
#endif

// snprintf macro
#ifdef WIN32
 #ifndef snprintf
  #define snprintf _snprintf
 #endif
#endif

//struct ConfigVar 
//{
//	struct ConfigVar *pNext;
//	char sName[COMMONLIB_ITEMNAMELEN + 1];
//	char *sValue;
//
//	ConfigVar()
//	{
//		pNext = 0;
//		*sName = 0;
//		sValue = 0;
//	}
//
//	~ConfigVar()
//	{
//		if(sValue)
//		{
//			delete []sValue;
//		}
//	}
//
//} ;
//
// struct ConfigGroup
//{
//	char sGroupName[COMMONLIB_GROUPNAMELEN + 1];
//	struct ConfigGroup *pNext;
//	ConfigVar *pVarList, *pLastVar;
//
//	ConfigGroup()
//	{
//		*sGroupName = 0;
//		pNext = NULL;
//		pVarList = NULL;
//		pLastVar = NULL;
//	}
//} ;
//
//struct Config
//{
//	char sFileName[COMMONLIB_FILEPATH + 1];
//	ConfigGroup *pGroupList, *pLastGroup;
//
//	Config()
//	{
//		pGroupList = NULL;
//		pLastGroup = NULL;
//	}
//};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
����������	int COMMONLIB_STRICMP(const char* sSrc,const char* sDst)
�������ܣ�	�Ƚ��ַ���
����˵����	[IN]const char* sSrc	- Դ�ַ���
			[IN]const char* sDst	- Ŀ���ַ���
����ֵ��	= 0						- ���
			> 0						- ����
			< 0						- С��
**/
int COMMONLIB_STRICMP(const char* sSrc,const char* sDst)
{
#ifdef WIN32
	// Windows Platform
	return stricmp(sSrc,sDst);
#else
	// Unix platform
	return strcasecmp(sSrc,sDst);
#endif
}

/**
����������	CConfigFile::CConfigFile()
�������ܣ�	���캯��
����˵����	��
����ֵ��	��
**/
CConfigFile::CConfigFile()
{
	m_nLineNumber = 0;
	m_pCurrentGroup = 0;
}

/**
����������	CConfigFile::~CConfigFile()
�������ܣ�	��������
����˵����	��
����ֵ��	��
**/
CConfigFile::~CConfigFile()
{
	Comment *pComment = NULL;
	int i = 0;

	for(i = 0; i < m_CommentVector.size(); i++)
	{
		pComment = m_CommentVector[i];
		SAFEDELETE(pComment->sLine);
		SAFEDELETE(pComment);
	}
}

/**
����������	long CConfigFile::Init(char *sFileName, char *sDefaultFileName)
�������ܣ�	��ʼ��
����˵����	[IN]char *sFileName				- �����ļ�
			[IN]char *sDefaultFileName		- ȱʡ�����ļ�
����ֵ��	�Ƿ�ɹ�
**/
long CConfigFile::Init(char *sConfFileName, char *sDefaultFileName)
{
	Comment *pComment = NULL;
	char *pTmpCommentTxt = NULL;
	FILE *pFile = 0;
	char sLine[1024] = {0};
	char *s, *p, *value, *sFileName = sConfFileName;
	ConfigGroup *pGroup = 0;
	long nLineNum = 0;

	//check paramter
	if((sConfFileName == 0) && (sDefaultFileName == 0))
		return errInvalideParameter;

	if((sConfFileName == 0) && sDefaultFileName)
		sFileName = sDefaultFileName;
	else if((sConfFileName == 0)||(*sConfFileName == 0))
		return errInvalideParameter;

	
	//open config file
	pFile = fopen(sFileName, "r");
	if(pFile == NULL)
	{//open file failed

		if((sDefaultFileName != 0)&&(*sDefaultFileName != 0))
		{
			//open default file
			pFile = fopen(sDefaultFileName,"r");
			if(pFile == NULL)
			{//open default file failed
				goto error;
			}
			else
			{//open default file successfully

				//copy filename
				strncpy(m_Config.sFileName, sDefaultFileName,COMMONLIB_FILEPATH);
				m_Config.sFileName[COMMONLIB_FILEPATH] = 0;
			}
		}
		else//no default file
			goto error;
	}
	else
	{//open file ok
		strncpy(m_Config.sFileName, sFileName,COMMONLIB_FILEPATH);
		m_Config.sFileName[COMMONLIB_FILEPATH] = 0;
	}
	
	nLineNum = 0;
	while(1)
	{
		++nLineNum;

		//get a line
		if (fgets(sLine, sizeof(sLine) - 1, pFile) == NULL) 
		{
			if (!ferror(pFile))
				break;
			goto error;
		}

		//remove space
		s = Trim(sLine);
		
		//ע���кͿ���
		if (*s == '#' || *s == '\0')
		{
			pComment = new Comment;
			if (!pComment)
			{
				printf("operator 'new' error!\n");
				goto error;
			}

			pComment->sLine = new char[strlen(s) + 1];
			if (!pComment->sLine)
			{
				printf("operator 'new' error!\n");
				delete pComment;
				pComment = NULL;
				goto error;
			}

			pComment->nLineNo = nLineNum;
			strcpy(pComment->sLine, s);

			m_CommentVector.push_back(pComment);
			continue;
		}


//		//����
//		if (*s == '\0') 
//		{
//			pComment = new Comment;
//			if (!pComment)
//			{
//				printf("operator 'new' error!\n");
//				goto error;
//			}
//
//			pComment->sLine = new char[strlen(s) + 1];
//			if (!pComment->sLine)
//			{
//				printf("operator 'new' error!\n");
//				goto error;
//			}
//
//			pComment->nLineNo = nLineNum;
//			strcpy(pComment->sLine, s);
//
//			continue;
//		}
		
		// Added for comment and value in the same line, Zhang Yuan, 2000-12-19.
		p = strchr(s, '#');
		if (p)
		{
			pTmpCommentTxt = p + 1;
			*p = '\0';
		}
		// End of the modification.
		
		p = strchr(s, '=');
		if (p == NULL) 
		{
			printf("Invalid syntax, '=' is expected at line:%d of file:%s\n", 
				nLineNum, m_Config.sFileName);
			goto error;
		}

		// set '=' as '\0'
		*p++ = '\0';
		s = Trim(s);
		value = ParseValue(p);

		if (value == NULL)
		{
			printf("Invalid value of %s, at line:%d of file:%s\n", s,
				nLineNum, m_Config.sFileName);
			goto error;
		}

		if(COMMONLIB_STRICMP(s, COMMONLIB_GROUP) == 0)
		{
			ConfigGroup *pTemp = NULL;

			pTemp = AddGroup(value, nLineNum, pTmpCommentTxt);

			delete []value;
			value = NULL;

			if(pTemp == NULL)
			{
				printf("operator 'new' error!\n");
				goto error;
			}
			else
			{
				pGroup = pTemp;
				pTmpCommentTxt = NULL;
				continue;
			}
		}

		if (pGroup == NULL)
		{
			pTmpCommentTxt = NULL;
			continue;
		}

		// init = 1 means this thing takes place while init phase
		SetValue(pGroup, s, value, nLineNum, pTmpCommentTxt, 1);

		delete []value;

		value = NULL;
		pTmpCommentTxt = NULL;
	}
	
	m_nLineNumber = nLineNum;
	if (fclose(pFile) != 0) {
		goto error;
	}
	return 0;
	
error:
	if (pFile != NULL)
	{
		fclose(pFile);
		pFile = NULL;
	}

	return -1;
}

/**
����������	void CConfigFile::Destroy()
�������ܣ�	�ͷ���Դ
����˵����	��
����ֵ��	��
**/
void CConfigFile::Destroy()
{
	ConfigGroup *pGroup, *pNextGroup;
	ConfigVar *pVar, *pNextVar;

	//check member var
	if (m_Config.pGroupList == NULL)
		return;

	//remove all group config
	for(pGroup = m_Config.pGroupList; pGroup != NULL; pGroup = pNextGroup)
	{
		pNextGroup = pGroup->pNext;

		//remove all var
		for(pVar = pGroup->pVarList; pVar != NULL; pVar = pNextVar)
		{
			pNextVar = pVar->pNext;

			delete pVar;
		}
		
		delete pGroup;
		pGroup = NULL;
	}

	m_Config.pGroupList = NULL;
	m_Config.pLastGroup = NULL;
}

/**
����������	void CConfigFile::Dump()
�������ܣ�	��������Ϣ��ʾ��console
����˵����	��
����ֵ��	��
**/
void CConfigFile::Dump()
{
	ConfigGroup *pGroup = NULL;
	ConfigVar *pVar = NULL;

	printf("Config dump begins:\n");
	printf("filename = <%s>\n", m_Config.sFileName);

	for (pGroup = m_Config.pGroupList; pGroup != NULL; pGroup = pGroup->pNext) {
		printf("group:<%s>\n",pGroup->sGroupName);
		for (pVar = pGroup->pVarList; pVar != NULL; pVar = pVar->pNext)
			printf("  <%s> = <%s>\n", pVar->sName, pVar->sValue);
	}
	
	printf("Config dump ends.\n");
}

/**
����������	long CConfigFile::Save()
�������ܣ�	��������Ϣ���浽����
����˵����	��
����ֵ��	0 - failure, 1 - success
**/
long CConfigFile::Save()
{
	long nCurrentNo = 0;
	int i = 0, nCurrentI = 0;
	Comment *pComment = NULL;
	FILE *fp = NULL;

	ConfigGroup *pGroup = NULL, *pCurrentGroup = m_Config.pGroupList;
	ConfigVar *pVar = NULL;

	fp = fopen(m_Config.sFileName, "w");
	if (!fp)
	{
		printf("Open file %s error!\n", m_Config.sFileName);
		return 0L;
	}

	nCurrentI = 0;

ss_comment:	
	for (i = nCurrentI; i < m_CommentVector.size(); i ++)
	{
		pComment = m_CommentVector[i];
		if (!pComment)
			break;

		if (nCurrentNo + 1 == pComment->nLineNo)
		{
#ifdef WIN32
			fprintf(fp, "%s\n", pComment->sLine);
#else
			fprintf(fp, "%s\n", pComment->sLine);
#endif
			nCurrentNo ++;
		} 
		else // 
		{
			nCurrentI = i;
			goto content;
		}
	}

content:
	for (pGroup = pCurrentGroup; pGroup != NULL; pGroup = pGroup->pNext) 
	{
		if ( nCurrentNo + 1 == pGroup->nLineNo)
		{
#ifdef WIN32
			if (pGroup->sComment) // ugly codes :(, all for that '#'
			{
				fprintf(fp, "Group = %s #%s\n", pGroup->sGroupName, pGroup->sComment);
			}
			else
			{
				fprintf(fp, "Group = %s\n", pGroup->sGroupName);
			}
#else
			if (pGroup->sComment) 
			{
				fprintf(fp, "Group = %s #%s\n", pGroup->sGroupName, pGroup->sComment);
			}
			else
			{
				fprintf(fp, "Group = %s\n", pGroup->sGroupName);
			}
#endif
			nCurrentNo ++;
		}
		else if (nCurrentNo + 1 < pGroup->nLineNo)
		{
			pCurrentGroup = pGroup;
			goto ss_comment;
		}

		// write items
		for (pVar = pGroup->pVarList; pVar != NULL; pVar = pVar->pNext)
		{
			if (nCurrentNo + 1 == pVar->nLineNo)
			{
#ifdef WIN32
				if (pVar->sComment)
				{
					fprintf(fp, "%s = %s #%s\n", pVar->sName, pVar->sValue, pVar->sComment);
				}
				else
				{
					fprintf(fp, "%s = %s\n", pVar->sName, pVar->sValue);
				}
#else
				if (pVar->sComment)
				{
					fprintf(fp, "%s = %s #%s\n", pVar->sName, pVar->sValue, pVar->sComment);
				}
				else
				{
					fprintf(fp, "%s = %s\n", pVar->sName, pVar->sValue);
				}
#endif
				nCurrentNo ++;
			}
			else if (nCurrentNo + 1 < pVar->nLineNo)
			{
				goto ss_comment;
			}
		}
	}
	
	fflush(fp);
	fclose(fp);

	return 1L;
}

/**
����������	int CConfigFile::FindNextGroup()
�������ܣ�	ȡ����һ��
����˵����	��
����ֵ��	�Ƿ�ɹ�
**/
int CConfigFile::FindNextGroup()
{
	ConfigGroup *pGroup = NULL;
	
	if(m_Config.pGroupList == NULL)
		return errInitFirst;

	if(m_pCurrentGroup == NULL)
		return errFindFirst;

	for(pGroup = m_pCurrentGroup->pNext; pGroup != NULL; pGroup = pGroup->pNext)
	{
		if(COMMONLIB_STRICMP(pGroup->sGroupName,m_pCurrentGroup->sGroupName) == 0)
		{
			m_pCurrentGroup = pGroup;

			return 0;
		}
	}

	return errNotFound;
}

/**
����������	int CConfigFile::FindFirstGroup(char *sGroupName)
�������ܣ�	ȡ�õ�һ��
����˵����	[IN]char *sGroupName		- �����������
����ֵ��	�Ƿ�ɹ�
**/
int CConfigFile::FindFirstGroup(char *sGroupName)
{
	ConfigGroup *pGroup;

	if((sGroupName == 0)||(*sGroupName == 0))
		return errInvalideParameter;

	if (m_Config.pGroupList == NULL)
		return errInitFirst;

	m_pCurrentGroup = NULL;

	for(pGroup = m_Config.pGroupList; pGroup != NULL; pGroup = pGroup->pNext)
	{
		if(COMMONLIB_STRICMP(pGroup->sGroupName,sGroupName) == 0)
		{
			m_pCurrentGroup = pGroup;

			return 0;
		}
	}

	return errNotFound;
}

/**
����������	int CConfigFile::Get(char *sItemName)
�������ܣ�	ȡ�õ�ǰ���������ֵ
����˵����	[IN]char *sItemName		- �����������
����ֵ��	�Ƿ�ɹ�
**/
char* CConfigFile::Get(char *sItemName)
{
	ConfigVar* pVar = NULL;

	if((sItemName == NULL)||(*sItemName == 0))
		return NULL;
	
	if(m_Config.pGroupList == NULL)
		return NULL;

	if(m_pCurrentGroup == NULL)
		return NULL;

	for(pVar = m_pCurrentGroup->pVarList; pVar != NULL; pVar = pVar->pNext)
	{
		if(COMMONLIB_STRICMP(pVar->sName,sItemName) == 0)
		{
			return pVar->sValue;
		}
	}

	return NULL;
}

/**
����������	int CConfigFile::Get(int iIndex)
�������ܣ�	ȡ�õ�ǰ���������ֵ
����˵����	[IN]int iIndex		- �����������
����ֵ��	�Ƿ�ɹ�
**/
char* CConfigFile::Get(int iIndex)
{
	ConfigVar* pVar = NULL;
	int iTemp = 0;

	if(iIndex < 0)
		return NULL;
	
	if(m_Config.pGroupList == NULL)
		return NULL;

	if(m_pCurrentGroup == NULL)
		return NULL;

	for(pVar = m_pCurrentGroup->pVarList; pVar != NULL; pVar = pVar->pNext)
	{
		if(iTemp == iIndex)
		{
			return pVar->sValue;
		}

		iTemp ++;
	}

	return NULL;
}

/**
����������	char* CConfigFile::Trim(char *sString)
�������ܣ�	��ȥ�ַ�����ͷ�Ŀո�
����˵����	[IN]char *sString		- Դ�ַ���
����ֵ��	���ɵ��ַ���
**/
char* CConfigFile::Trim(char *sString)
{
	unsigned char *ustr, *end;
	
	ustr = (unsigned char *)sString;
	while (isspace(*ustr))
		++ustr;
	end = (unsigned char *)strchr((char *)ustr, '\0');
	while (ustr < end && isspace(end[-1]))
		--end;
	*end = '\0';

	return (char *)ustr;
}

/**
����������	ConfigGroup * CConfigFile::AddGroup(char *s, long nLineNo, char *pCommentGroupName)
�������ܣ�	����һ��������
����˵����	[IN]char *sGroupName		- ����������
			[IN]long nLineNo		- �к�
			[IN]char *pComment		- ���е�ע��

����ֵ��	������
**/
ConfigGroup * CConfigFile::AddGroup(char *sGroupName, long nLineNo, char *pComment)
{
	if((sGroupName == NULL)||(*sGroupName == 0))
		return NULL;

    ConfigGroup *pGroup;

    pGroup = new ConfigGroup;
	if(pGroup == NULL)
		return NULL;

	strncpy(pGroup->sGroupName,sGroupName,COMMONLIB_GROUPNAMELEN);
	pGroup->sGroupName[COMMONLIB_GROUPNAMELEN] = 0;
	pGroup->nLineNo = nLineNo;
	
	// set comment
	if (pComment)
	{
		pGroup->sComment = new char[strlen(pComment) + 1];
		if (!pGroup->sComment)
		{
			delete pGroup;
			pGroup = NULL;
			return NULL;
		}

		// copy comment
		strcpy(pGroup->sComment, pComment);
	}

	// add this new group node to the group list
	if(m_Config.pGroupList == NULL)
	{
		m_Config.pGroupList = pGroup;
		m_Config.pLastGroup = pGroup;
	}
	else
	{
		m_Config.pLastGroup->pNext = pGroup;
		m_Config.pLastGroup = pGroup;
	}

    return pGroup;
}


long CConfigFile::SetValueByName(char *sItemName, char *sItemValue, char *sComment)
{
	return SetValue(NULL, sItemName, sItemValue, -1, sComment, 0);
}

/**
����������long CConfigFile::SetValue(ConfigGroup *pGroup, char *sItemName, char *sItemValue, long nLineNo, 
						   char *sComment, long nInit)
�������ܣ�	�����������ֵ
����˵����	[IN]ConfigGroup *pGroup		- ������ָ��
			[IN]char *sItemName			- �����������
			[IN]char *sItemValue		- �������ֵ
����ֵ��	<0 -- ʧ��  0 -- �޸ĳɹ� 1 -- ��ӳɹ�
**/
long CConfigFile::SetValue(ConfigGroup *pGroup, char *sItemName, char *sItemValue, long nLineNo, 
						   char *sComment, long nInit)
{
	ConfigVar *pVar = NULL;
	ConfigGroup *pTmpGroup = pGroup;
	char* sTempValue = NULL;
	Comment *pComment = NULL;
	long ret = -1;

	// set the group
	if (pGroup == NULL)
		if (m_pCurrentGroup == NULL)
			return -1;
		else
			pTmpGroup = m_pCurrentGroup;

	if((sItemName == 0)||(*sItemName == 0))
		return -1;

	if(sItemValue == NULL)
		return -1;

	// modify
	pVar = new ConfigVar;
	if(pVar == NULL)
		return -1;
	
	AddItemToGroup(pTmpGroup, pVar);
	// modify end
#if 0

		ret = 1;
	// find this item in current group.
	pVar = FindItem(pTmpGroup, sItemName);
	if(pVar == NULL) // new one ?
	{
		pVar = new ConfigVar;
		if(pVar == NULL)
			return -1;

		AddItemToGroup(pTmpGroup, pVar);

		ret = 1;
	}
	else
	{
		// old one
		if (nInit) // while init phase
		{
			char tmpBuf[1024];

			snprintf(tmpBuf, 1023, "%s = %s #%s", pVar->sName, pVar->sValue, 
				pVar->sComment ? pVar->sComment : "");
			tmpBuf[1024] = 0;

			pComment = new Comment;
			if (!pComment)
			{
				printf("operator 'new' error!\n");
				return -1;
			}

			pComment->sLine = new char[strlen(tmpBuf) + 1];
			if (!pComment->sLine)
			{
				printf("operator 'new' error!\n");
				delete pComment;
				pComment = NULL;

				return -1;
			}
			pComment->nLineNo = pVar->nLineNo;
			strcpy(pComment->sLine, tmpBuf);

			// push list
			m_CommentVector.push_back(pComment);
		}
		ret = 0;
	}
#endif

	sTempValue = new char[strlen(sItemValue) + 1];
	if(sTempValue == NULL)
		return -1;
	
	strncpy(pVar->sName, sItemName, COMMONLIB_ITEMNAMELEN);
	pVar->sName[COMMONLIB_ITEMNAMELEN] = 0;

	strcpy(sTempValue, sItemValue);
	
	if(pVar->sValue)
		delete []pVar->sValue;

	pVar->sValue = sTempValue;

	if (nLineNo > 0)
		pVar->nLineNo = nLineNo;

	if (sComment)
	{
		if (pVar->sComment) // delete old comments
		{
			// has enough space?
			if (strlen(sComment) <= strlen(pVar->sComment))
			{
				strcpy(pVar->sComment, sComment);
			}
			else // allocate new space for this new comment
			{
				char *p = NULL;

				p = new char[strlen(sComment) + 1];
				if (!p)
					return -1;

				delete pVar->sComment;
				pVar->sComment = p;
				strcpy(pVar->sComment, sComment);
			}
		}
		else // add new comments
		{
			pVar->sComment = new char[strlen(sComment) + 1];
			if (!pVar->sComment)
				return -1;
			
			strcpy(pVar->sComment, sComment);
		} // end if (pVar->sComment) 
	} // end if (sComment)

	return ret;
}

/**
����������	ConfigVar* CConfigFile::FindItem(ConfigGroup *pGroup, char *sItemName)
�������ܣ�	����������
����˵����	[IN]ConfigGroup *pGroup		- ������ָ��
			[IN]char *sItemName			- �����������
����ֵ��	������ָ��
**/
ConfigVar* CConfigFile::FindItem(ConfigGroup *pGroup, char *sItemName)
{
	ConfigVar *pVar = NULL;

	if(pGroup == NULL)
		return NULL;

	if((sItemName == 0)||(*sItemName == 0))
		return NULL;

    for(pVar = pGroup->pVarList ; pVar != NULL; pVar = pVar->pNext)
        if (COMMONLIB_STRICMP(pVar->sName,sItemName) == 0)
            return pVar;

    return NULL;
}

/**
����������	void CConfigFile::AddItemToGroup(ConfigGroup *pGroup, ConfigVar *pVar)
�������ܣ�	����������
����˵����	[IN]ConfigGroup *pGroup		- ������ָ��
			[IN]ConfigVar *pVar			- �������ָ��
����ֵ��	��
**/
void CConfigFile::AddItemToGroup(ConfigGroup *pGroup, ConfigVar *pVar)
{
	if((pGroup == NULL)||(pVar == NULL))
		return;

	if(pGroup->pVarList == NULL)
	{
		pGroup->pVarList = pVar;
		pGroup->pLastVar = pVar;
	}
	else
	{
		pGroup->pLastVar->pNext = pVar;
		pGroup->pLastVar = pVar;
	}
}

/**
����������	char* CConfigFile::ParseValue(char *sValue)
�������ܣ�	�����ַ���
����˵����	[IN]char *sValue		- ��Ҫ���ɵ��ַ���
����ֵ��	���ɵ��ַ���
**/
char* CConfigFile::ParseValue(char *sValue)
{
    char *p, *str2;

    sValue = Trim(sValue);

    if (*sValue == '"')
	{
		str2 = new char[strlen(sValue) + 1];

        ++sValue; 	/* skip leading '"' */
        p = str2;
        while (*sValue != '"' && *sValue != '\0') {
            switch (*sValue) {
            case '\\':
                switch (sValue[1]) {
                case '\0':
                    *p++ = '\\';
                    sValue += 1;
                    break;
                case '\\':
                    *p++ = '\\';
                    sValue += 2;
                    break;
                case '"':
                    *p++ = '"';
                    sValue += 2;
                    break;
                default:
                    *p++ = '\\';
                    *p++ = sValue[1];
                    sValue += 2;
                    break;
                }
                break;
            default:
                *p++ = *sValue++;
                break;
            }
        }
        *p = '\0';
    }
	else
	{
		str2 = new char[strlen(sValue) + 1];
		strcpy(str2,sValue);
    }

    return str2;
}
