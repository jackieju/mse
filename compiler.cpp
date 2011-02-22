#include "clib.h"
//#include "io.h"
#include "fcntl.h"
#include "string.h"


#include "cocoR/CR_SCAN.hpp"
#include "cocoR/CR_ERROR.hpp"
#include "cocoR/CR_PARSE.hpp"

#include "cocoR/cs.hpp"


#include "cocoR/cp.hpp"
#include "compiler.h"

#include "os/CSS_LOCKEX.H"
#include "ScriptFuncTable.h"



static FILE *output = stderr;
static int Listinfo = 0;
char CCompiler::m_szErrMsg[1024] = "";
char CCompiler::m_szErrFile[_MAX_PATH] = "";


char *MyError::ErrorMsg[] = {
#include "cocoR/ce.hpp"
	"User error number clash",
		""
};

CScriptFuncTable CCompiler::functable[SCRIPTTABLE_NUM];
CClassDesTable CCompiler::classDesTable;
long CCompiler::g_lActiveScriptTable;
CPubFuncTable CCompiler::m_PubFuncTable;

char *MyError::GetUserErrorMsg(int n)
{ switch (n) {
    // Put your customized messages here
		 case 96: return "compiler internal Error: popdigit failed";
		 case 97: return "can not find public function entry";
		 case 98: return "generate temp variable failed";
		 case 99: return "undefined type";
		 case 100: return "analyze string failed";
		 case 101: return  "can not use [] with a no-array variable";
		 case 102: return "can not use * with a variable which is not a point"; 
		 case 103: return "too many layer in indirect address mode";
		 case 104: return "Undefined Symbol";
		 case 105: return "function name can not be longer than 20 characters";
		 case 106: return "function parameter number is wrong";
		 case 107: return "point can not calculated with float";
		 case 108: return "unknow how to cast type";
		 case 109: return "float type can not use mode calculation";
		 case 110: return "can not perform this type cast";
		 case 111: return "type of symbol in symbol table has error";
		 case 112: return "can not use this type operation with float number";
		 case 113: return "variable allocate failed";
		 case 114: return "add script function to srcipt table failed";
		 case 115: return "symbol table has error";
		 case 116: return "compiler internal error: memory alloction failed";
		 case 117: return "expect a identifier";
		 case 118: return "add class member failed";
		 case 119: return "add class failed";
		 case 120: return "can not get this class info";
		 case 121: return "this class member not defined";
		 case 122: return "can not define variable here";
		 case 123: return "parameter number of fucntion call is no correct";
		 case 124: return "string use invalid escape character";
		 case 125: return "Compiler internal error: push digit failed because the data type is invalid";
		 case 126: return "not support function as value";
		 default:
			 return "Unknown error or conflicting error numbers used";
}
}

void SourceListing(CRError *Error, CRScanner *Scanner, char* szFileName)
// generate the source listing
{ char ListName[256];
int  i;

strcpy(ListName, szFileName);
i = strlen(ListName)-1;
while (i>0 && ListName[i] != '.') i--;
if (i>0) ListName[i] = '\0';

strcat(ListName, ".lst");
FILE* f = NULL;
if ((f = fopen(ListName, "w")) == NULL) {
    fprintf(stderr, "Unable to open List file %s\n", ListName);
    return;
}
Error->SetOutput(f);
Error->PrintListing(Scanner);

Error->SetOutput(output);
fprintf(output, "Generated source list file %s", ListName);
fclose(f);
}

void CCompiler::setOutput(FILE *f){
	if (f)
		output = f;
}

CCompiler::CCompiler()
{
	//m_Parser.Scanner = &this->m_Scanner;
	//m_Parser.Error = &this->m_Error;
	//this->m_Error.Scanner = &this->m_Scanner;	
	//m_szErrMsg[0] = 0;
	//memset(m_szErrFile, 0, _MAX_PATH);
	memset(m_szSourceFile, 0, _MAX_PATH);
	//strcpy(m_szErrFile, "./compile_err.txt");
}

CCompiler::~CCompiler()
{
	
}

BOOL CCompiler::Compile(char *szFileName)
{

	int S_src;

	
	
	//if (this->m_Parser.m_ExeCodeTable == NULL || this->m_Parser.m_PubFuncTable == NULL)
	//	{
	//		sprintf(m_szErrMsg, "scipt table or pubfunction table arte not attached");
	//		fprintf(stderr, "scipt table or pubfunction table arte not attached");
	//		return FALSE;
	//	}
	
	// check on correct parameter usage
	if (!szFileName[0]) 
	{
		fprintf(stderr, "No input file specified\n");	
		sprintf(m_szErrMsg, "No input file sepcfied");
		return FALSE;
	}

	
	// open the source file S_src
	std::string path = findSrc(szFileName);
	if (path.empty()){
		fprintf(stderr, "Unable to open input file %s.\n", szFileName);
		sprintf(m_szErrMsg, "Unable to open input file %s.\n", szFileName);		
		return FALSE;
	}
	else {
			strcpy(this->m_szSourceFile, path.c_str());
#ifdef _MACOS	// binary and text files has no difference in unix
		int mode = O_RDONLY;
#else
		int mode = O_RDONLY | O_BINARY;
#endif
		S_src = open(path.c_str(), mode);
	}
	// instantiate Scanner, Parser and Error handler
	cScanner m_Scanner(S_src, 0);
	MyError m_Error(szFileName, &m_Scanner);	
//	m_Error.SetOutput(output);
	cParser m_Parser(this, &m_Scanner, &m_Error);
	m_Parser.init(&CCompiler::classDesTable, &CCompiler::m_PubFuncTable, &m_conf);
//	m_Parser.setConfig(&m_conf);
	m_Parser.setSourceFileName(szFileName);
	//m_Error.Init(szFileName);
	//m_Parser.Init();
	

	if (strlen(m_szErrFile)>0){
		FILE* file = NULL;	
		file = fopen(m_szErrFile, "a");
		if (file)
			m_Error.SetOutput(file);
		else
		{
			fprintf(stderr, "can not open error list output	file '%s', output to stderr\n", m_szErrFile);
			sprintf(m_szErrMsg, "can not open error list output	file '%s'\n, output to stderr", m_szErrFile);
			m_Error.SetOutput(output);
			file = stderr;
		}
	}else{
		m_Error.SetOutput(output);
	}

	//write time
	{
		char msg[1024];
		time_t t;
        struct tm *ptm;
        time(&t);
		ptm = localtime(&t);
	
		sprintf(msg, "----------------------------------------------------------------\n%04d-%02d-%02d %02d:%02d:%02d\tCompile %s\n----------------------------------------------------------------\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, m_szSourceFile);
		//fwrite(msg, 1, strlen(msg), file);
		fprintf(output, msg);
		
		sprintf(msg, "%04d-%02d-%02d %02d:%02d:%02d\tCompile %s", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, m_szSourceFile);
		
		LOG(msg, "compiler");
	}
	
	
	//m_Parser.m_pMainFunction = new CFunction;
	// parse the source
	clock_t s_c = clock();
	m_Parser.Parse();
	close(S_src);
	
	// Add to the following code to suit the application
	if (m_Error.Errors)	  
	{
		fprintf(stderr, "Compilation errors\n");
		sprintf(m_szErrMsg, "Compilation errors\n");
	}
	if (Listinfo) 
		SourceListing(&m_Error, &m_Scanner, szFileName);
	else if (m_Error.Errors)
		m_Error.SummarizeErrors();
	//write end
	{
		// if (!m_Error.Errors)
		// 	fprintf(file, "Compile file '%s' succeeded\r\n", m_szSourceFile);
		// else 
		// 	fprintf(file, "Compile file '%s' failed\r\n", m_szSourceFile);
		char msg[1024];
		time_t t;
        struct tm *ptm;
        time(&t);
		ptm = localtime(&t);
		if (!m_Error.Errors)
			sprintf(msg, "%04d-%02d-%02d %02d:%02d:%02d \tCompile %s succeeded \n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, m_szSourceFile);
		else
			sprintf(msg, "%04d-%02d-%02d %02d:%02d:%02d \tCompile %s failed \n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, m_szSourceFile);
		//fwrite(msg, 1, strlen(msg), file);
		fprintf(output, msg);
	}
	if (!m_Error.Errors)
		printf("compile %s succeeded, cost %ld ms\n", m_szSourceFile, clock() - s_c);
	else
		printf("compile %s failed, cost %ld ms\n\n", m_szSourceFile, clock() - s_c);

	if ( output && output != stderr && output!=stdout)
		fclose(output);
	
	if (m_Error.Errors)
	{
		sprintf(m_szErrMsg, "%s have some error", szFileName);
		ERR(m_szErrMsg);
			
		char msg[1024]="";
		snprintf(msg, 1000, "Compile %s failed.", szFileName);	
		LOG(msg, "compiler");
		
		return FALSE;
	}
	
	{
		char msg[1024]="";
		snprintf(msg, 1000, "Compile %s successfully.", szFileName);	
		LOG(msg, "compiler");
	}
	return TRUE;
}
