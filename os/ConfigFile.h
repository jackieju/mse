/**
 * @author  刘骏
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
	Config	m_Config;			// 配置文件组列表
	ConfigGroup	*m_pCurrentGroup;
public:

   /**
    * <pre>
	函数声明：	int CConfigFile::Get(int iIndex)
	函数功能：	取得当前组的配置项值
	变量说明：	[IN]int iIndex		- 配置项的索引
	返回值：	是否成功
	* </pre>
	*/
	char* Get(int iIndex);

	/**
	 * <pre>
	函数声明：	int CConfigFile::Get(char *sItemName)
	函数功能：	取得当前组的配置项值
	变量说明：	[IN]char *sItemName		- 配置项的名称
	返回值：	是否成功
	* </pre>
	*/
	char* Get(char *sItemName);


	/**
	 * <pre>
	函数声明：long SetValueByName(char *sItemName, char *sItemValue, char *sComment = NULL)
	函数功能：	配合FindFirstGroup和FindNextGroup使用来改变某个配置项的值。
	变量说明：	[IN]char *sItemName - 配置项目名称
				[IN]char *sItemValue - 配置项的值，无论何种类型都以char *形式传入。
				[IN]char *sComment - 该项目的注释，缺省为NULL。
	返回值：	<0 -- 失败  0 -- 修改成功 1 -- 添加成功
	 * </pre>
	 */
	long SetValueByName(char *sItemName, char *sItemValue, char *sComment = NULL);

	/**
	 * <pre>
	函数声明：	int CConfigFile::FindFirstGroup(char *sGroupName)
	函数功能：	取得第一组
	变量说明：	[IN]char *sGroupName		- 配置组的名称
	返回值：	是否成功
	* </pre>
	*/
	int FindFirstGroup(char *sGroupName);

	/**
	 * <pre>
	函数声明：	int CConfigFile::FindNextGroup()
	函数功能：	取得下一组
	变量说明：	无
	返回值：	是否成功
	* </pre>
	*/
	int FindNextGroup();

	/**
	 * <pre>
	函数声明：	void CConfigFile::Dump()
	函数功能：	将配置信息显示到console
	变量说明：	无
	返回值：	无
	* </pre>
	*/
	void Dump();

	/**
	 * <pre>
	函数声明：	long Save()
	函数功能：	将配置信息保存到磁盘
	变量说明：	无
	返回值：	0 - failure, 1 - success
	* </pre>
	*/
	long Save();

	/**
	 * <pre>
	函数声明：	void CConfigFile::Destroy()
	函数功能：	释放资源
	变量说明：	无
	返回值：	无
	* </pre>
	*/
	void Destroy();

	/**
	 * <pre>
	函数声明：	long CConfigFile::Init(char *sFileName, char *sDefaultFileName)
	函数功能：	初始化
	变量说明：	[IN]char *sFileName				- 配置文件
	[IN]char *sDefaultFileName		- 缺省配置文件
	返回值：	是否成功
	* </pre>
	*/
	long Init(char *sFileName, char *sDefaultFileName = 0);

	/**
	 * <pre>
	函数声明：	CConfigFile::CConfigFile()
	函数功能：	构造函数
	变量说明：	无
	返回值：	无
	* </pre>
	*/
	CConfigFile();

	/**
	 * <pre>
	函数声明：	CConfigFile::~CConfigFile()
	函数功能：	析构函数
	变量说明：	无
	返回值：	无
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
