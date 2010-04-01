#ifndef __SE_INTERFACE_H__
#define __SE_INTERFACE_H__
#ifdef 	WIN32
#define SEMETHOD __stdcall
#else
#define SEMETHOD __stdcall
#endif
/*@@Setup NCSE 
All NCSE components include:
<table>
File Name							Description 
----------------					-------------
SE.dll								Script Engine core dll 
SE.h								header of se.dll 
external lib						external lib call by NewC 
external lib desription file		descript the interface of external lib.  
									File format: Method Name<White Space>parameter number<end of line>
NC source file						source file written using NewC, will be compiled by se.dll 
se.cfg								config file of NCSE:
</table>
 
NCSE config file format: 
Group = Main 
<table>
Item Name		Item value					Description
------------	------------				------------
scriptpath		"d:\se\Scripts"				path of source files 
errorlistfile	"d:\se\errorlist.txt"		path name of compile-time error output 
workmode		0							VM work mode, 0 is normal, 1 is debug 
</table>
Group = Source File List 
<table>
Item Name		Item value					Description
------------	------------				------------
File			"test.nc"					source file name, can use relative path name or absolute path name 
</table>
Group = Log 
<table>
Item Name		Item value							Description
------------	------------						------------
LogFile			"d:\se\log\log%04d%02d%02d.txt"		Pathname of log file, you can speicify the format of date of log file 
CacheMode		0									cache mode, 0: the log not use cache, 1: log using cache 
CacheSize		4096								log cache size, this value will be ingored if CacheMode = 0 
</table>
*/

/*@@Use SE Step by step
<b>Launch SE Step by step</b>

Step 1: Call SE_Start() to Start script engine<>

Step 2: Call SE_LoadLib() to Load external platform lib

Step 3: Call SE_CompileAll() to compile all scripts define in config file



<b>Run Time compile </b>

Call SE_RecompileAll() or SE_Compile() to compile at run time.
Neither method will impact the running instance of a NC Function. 

<b>Run an NC function</b>

Step 1: You should generate a request for the script you want to execute. 
Step 2: call SE_ExecScript to run the NC function.

<b>How to Generate a request</b>

Step 1: Call SE_REQ_Create to create a request, this call will return the request hanle if succeed. 
If you succeed in calling the SE_ExecScript() using this request, you need not use SE_REQ_ReleaseReq()
 to release it, otherwise you should release it.

Step 2: Call SE_REQ_SetFn To specify the NC script function name you want to call.

Step 3: Call SE_REQ_AddXXX To Add the parameters to be passed to the NC script function.


*/
 

/*@@return code list
<table>
deine									value	meaning
-------------------------				------	------------
REQERR_NOERROR							0		succeeded
REQERR_NOFUNCNAME						-1		request has not function name
REQERR_SIZELESSZERO						-2		request size if less than 0
REQERR_FUNCNOTFOUND						-3		requested function not found
REQERR_CREATEMACHINEFAILED				-4		create virtual machine failed
REQERR_LOADFUNCTIONFAILED				-5		load function failed
REQERR_PARAMNUMERROR					-6		parameter number does not match the function
REQERR_CREATEPARAMFAILED		        -7		create parameter failed
REQERR_CHECKPOINTERROR			        -8		paraemter block fomat has error
REQERR_INVALIDDATATYPE			        -9		create
REQERR_CREATEPARAMBLOCKFAILED			-10		create parameter block failed
REQERR_ALLOCARRAYFAILED					-11		allocate parameter of array failed
REQERR_LOADDATAOBJFAILED				-12		load data object failed
REQERR_DATAOBJHASNOATTR					-13		data object has no attribute
REQERR_ADDEXTERNALSPACEFAILED			-14		add external access space for VM failed
REQERR_INVALIDLOGFUNCTIONENTRY			-15		invalid log functio entry
REQERR_INVALIDCONFIGFILE				-16		invalid config file
REQERR_PARAMBLOCKOVERFLOW				-17		parameter block is too long
REQ_UNITSIZEERROR						-18     unit size error
REQ_LOADREQUESTFAILED					-19		load request failed

MEMERR_ALLOCFAILED						-100	can not allocate memory

SEERR_NOERROR							0		no error
SEERR_CONFIGFILENOTFIND					-200	config file not found
SEERR_HFUNCTABLEISNULL					-201	function talbe is null
SEERR_NOAVAILABLESCRIPTTABLEFOUND		-202	there is no available script table
SEERR_INVALIDPARAM						-203	invalid parameter
SEERR_ATTACHPARAMFAILED					-204	attach parameters to function failed
SEERR_NODEBUGVM							-205	there is no vm is in debug mode
SEERR_TIMEOUT							-206	timeout

*/
#define REQERR_NOERROR					0		// succeeded
#define REQERR_NOFUNCNAME               -1		// request has not function name
#define REQERR_SIZELESSZERO             -2		// request size if less than 0
#define REQERR_FUNCNOTFOUND             -3		// requested function not found
#define REQERR_CREATEMACHINEFAILED      -4		// create virtual machine failed
#define REQERR_LOADFUNCTIONFAILED       -5		// load function failed
#define REQERR_PARAMNUMERROR            -6		// parameter number does not match the function
#define REQERR_CREATEPARAMFAILED        -7		// create parameter failed
#define REQERR_CHECKPOINTERROR          -8		// paraemter block fomat has error
#define REQERR_INVALIDDATATYPE          -9		// create
#define REQERR_CREATEPARAMBLOCKFAILED   -10		// create parameter block failed
#define REQERR_ALLOCARRAYFAILED         -11		// allocate parameter of array failed
#define REQERR_LOADDATAOBJFAILED        -12		// load data object failed
#define REQERR_DATAOBJHASNOATTR         -13		// data object has no attribute
#define REQERR_ADDEXTERNALSPACEFAILED   -14		// add external access space for VM failed
#define REQERR_INVALIDLOGFUNCTIONENTRY  -15		// invalid log functio entry
#define REQERR_INVALIDCONFIGFILE        -16		// invalid config file
#define REQERR_PARAMBLOCKOVERFLOW		-17		// parameter block is too long
#define REQERR_UNITSIZEERROR				-18     // unit size error
#define REQERR_LOADREQUESTFAILED		    -19		// load request failed
#define REQERR_INSERTPARAMFAILED		-20
#define REQERR_CREATEFAILED				-21
#define MEMERR_ALLOCFAILED              -100	// can not allocate memory

#define SEERR_BASE						-200
#define SEERR_NOERROR						0	// no error
#define SEERR_CONFIGFILENOTFIND				-200	// config file not found
#define SEERR_HFUNCTABLEISNULL				-201	// function talbe is null
#define SEERR_NOAVAILABLESCRIPTTABLEFOUND	-202	// there is no available script table
#define SEERR_INVALIDPARAM					-203	// invalid parameter
#define SEERR_ATTACHPARAMFAILED				-204	// attach parameters to function failed
#define SEERR_NODEBUGVM						-205	// there is no vm is in debug mode
#define SEERR_TIMEOUT						-206	// timeout
#define SEERR_GETFREEVMFAILED				-207
#define SEERR_INSTOPPING					-208
#define SEERR_COMPILEERROR					-209	// error occur when compiling

#define VM_MODE_NORMAL 0
#define VM_MODE_DEBUG  1
#define VM_MODE_STEPDEBUG 2

/*@@
  Virtual Machine Status
*/
struct	VMSTATUS
{
	std::vector<std::string>	sCallStack;	//@@ Call stack
	std::vector<long>		lines;			//@@ line array
	VMSTATUS()							
	{
		sCallStack.reserve(10);
		lines.reserve(10);
	}
};
/*@@
  Virtual Machine Runtime Infomation
*/

struct VMINFO
{
	long		id;			//@@ VM id
	SYSTEMTIME start_time;	//@@ VM start time
	VMSTATUS	status;		//@@ VM status
};



#ifndef WIN32
extern "C"{
#endif

/*@@
  Description:
         Stop Script Engine and release resouces used by SE


  Remarks:
        

  Parameters:
        nMillionSeconds	 - [in] Specifies the time-out interval, in milliseconds. The function returns if the interval elapses and not all runing script exit. 
        force	 - [in]If force = 1, when the interval elapses and there are still running scripts, all runing scripts will be terminated.
						

  Return Value:
        int - please see return code list

  See Also:
        

  Author: Jackie Ju 

  Date:   2002-11-25
 */
int SEMETHOD SE_Stop(long nMillionSeconds, long force);

/*@@
  Description:
        Compile a NC file

  Remarks:
        

  Parameters:
        char* szName	 - 
			[in]This path name of NC script file

  Return Value:
        long - 
			If the function succeeds, return 1, else return 0
  See Also:
        

  

  
 */
long	SEMETHOD SE_Compile(char* szName);

/*@@
  Description:
        compile all NC file defined in config file 

  Remarks:
        If a NC function is being run, the change will take effect in next run.

  Parameters:
        NONE


  Return Value:
        long - please see return code list

  See Also:
        

  

  
 */
long	SEMETHOD SE_CompileAll();


/*@@
  Description:
		Get Script engine version info

  Remarks:
        

  Parameters:

  Return Value:
        char* - version info string, need not be deleted.

  See Also:
        

  

  
 */
char*	SEMETHOD SE_GetVersion();

/*@@
  Description:
        Load external lib from lib file(dll or is) using interface description file(*.int)

  Remarks:
        

  Parameters:
        szLibName	 - [in]Lib file pathname
        szFileName	 - [in]Interface description file(*.int) pathname

  Return Value:
        long - please see return code list

  See Also:
        

  

  
 */
long	SEMETHOD SE_LoadLib(char *szLibName, char* szFileName);



/*@@
  Description:
        Add add a function by its entry

  Remarks:
        

  Parameters:
        pfn	 - [in]function entry
        szName	 - [in]function name used by scripts
        cParamNum	 - [in]parameter number

  Return Value:
        BOOL SEMETHOD- 

  See Also:
        

  Author: Jackie Ju 

  Date:   2003-5-21
 */
BOOL SEMETHOD SE_AddPubFunc(long pfn, char* szName, char cParamNum);

/*@@
  Description:
        Init NC script engine

  Remarks:
        This function will init script engine, compile all NC file defined in config file 'Source File List' group.
		So if these NC script call any function of external lib, you must load those lib using SE_LoadLib()
		before you call SE_ComileAll


  Parameters:
        char* szConfigFile	 - [in]config file path name

  Return Value:
        int- see return code list

  See Also:
        

  

  
 */
int		SEMETHOD SE_Start(char* szConfigFile);



/*@@
  Description:
        Set the register code

  Remarks:
        If you not set the code, you can only call SE_ExecScript no more than 100 times.
		If you want to use the se unlimitedly, you should buy a register key.

  Parameters:
        sUsername	 - [in]user name
        key	 - [in]the key you got from author

  Return Value:
        void- 

  See Also:
        

  Author: Jackie Ju 

  Date:   2002-11-25
 */
void  SEMETHOD SE_SetRegKey(char* sUsername, char* key);

/*@@
  Description:
        Get compiling error list file path name

  Remarks:
        

  Parameters:
        NONE


  Return Value:
        char* - The compiling error list file path name

  See Also:
        

  

  
 */
char*	SEMETHOD SE_GetErrFileName();



/*@@
  Description:
        create a new NC request.

  Remarks:
        If you want to run a NC function, you must create a NC request.
		The NC request contain the infomation about the function, including the function name
		and parameters.

  Parameters:
        NONE


  Return Value:
        long	- 
		If this function succeeds, the return value is address of CRequest

  See Also:
        

  

  
 */
long SEMETHOD SE_REQ_Create();

long SEMETHOD SE_REQ_Load(long req, unsigned char* p);

long SEMETHOD SE_REQ_InsertParam(long req, long type, unsigned char* p, long lIndex);


/*@@
  Description:
        Release reqeust if it is not executed. If you have executed this request, you must not
		use this function to release it.

  Remarks:
        

  Parameters:
        long hReq	 - [in]request handel

  Return Value:
        void - 

  See Also:
        

  

  
 */
void SEMETHOD SE_REQ_ReleaseReq(long hReq);

/*@@
  Description:
        Execute script using request, the request must not be release using SE_REQ_ReleaseReq();

  Remarks:
        

  Parameters:
        req	 - [in]request handle
        lWorkMode	 - [in]0: normal, 1: debug
		lFlag	- [in]0: in the same thread, 1: launch another thread
  Return Value:
        long - 

  See Also:
        

  

  
 */
long SEMETHOD SE_ExecScript(long req, long lWorkMode, long lFlag);

//long SEMETHOD SE_RunScript(long req, long lWorkMode);

/*@@
  Description:
        set NC script function name of request

  Remarks:
        

  Parameters:
        hReq	 - [in]
        szFuncName	 - [in]

  Return Value:
        long - 

  See Also:
        

  

  
 */
long SEMETHOD SE_REQ_SetFn(long hReq, char* szFuncName);


/*@@
  Description:
        get function name of the request

  Remarks:
        

  Parameters:
        long hReq	 - [in]request handle

  Return Value:
        char* -  return NULL if the request has not set function name

  See Also:
        

  

  Date:   2002-10-13
 */
char* SEMETHOD SE_REQ_GetFn(long hReq);

/*@@
  Description:
        Add a parameter with short type to NC Script request

  Remarks:
        

  Parameters:
        hReq	 - [in]
        param	 - [in]

  Return Value:
        long - 

  See Also:
        

  

  
 */
long SEMETHOD SE_REQ_AddParamShort(long hReq, short param);

/*@@
  Description:
        add a parameter with string type to request

  Remarks:
        

  Parameters:
        hReq	 - [in]request handle
        param	 - [in]parameter

  Return Value:
        long - 

  See Also:
        

  

  
 */
long SEMETHOD SE_REQ_AddParamString(long hReq, char* param);


/**
Declaration		: long SE_REQ_AddParamFloat(long hReq, float param);
Decription		: add param float to request
Parameter		: 
			[IN]long hReq	-	request
			[IN]float param	-	param value
Return value	: long  - 0: ok else failed
Written by		: Weihua Ju
Completed at	: 2002-7-4
**/

/*@@
  Description:
        add a parameter with float type to request

  Remarks:
        

  Parameters:
        hReq	 - [in]request handle
        param	 - [in]parameter

  Return Value:
        long - 

  See Also:
        

  

  
 */
long SEMETHOD SE_REQ_AddParamFloat(long hReq, float param);



/*@@
  Description:
        add a parameter with long type to request

  Remarks:
        

  Parameters:
        hReq	 - [in]request handle
        param	 - [in]parameter

  Return Value:
        long - 

  See Also:
        

  

  
 */
long SEMETHOD SE_REQ_AddParamLong(long hReq, long param);


/*@@
  Description:
        Add a long array as parameter to reqeust

  Remarks:
        

  Parameters:
        long size	 - [in]array size

  Return Value:
        long - please see return code list

  See Also:
        

  

  Date:   2002-10-13
 */
long SEMETHOD SE_REQ_AddLongArray(long size);

/*@@
  Description:
        Add a long array element

  Remarks:
        

  Parameters:
        long p	 - [in]long array element

  Return Value:
        long - please see return code list

  See Also:
        

  

  Date:   2002-10-13
 */
long SEMETHOD SE_REQ_AddLongElement(long p);

/*@@
  Description:
        Add a float array as parameter to reqeust

  Remarks:
        

  Parameters:
        long size	 - [in]array size

  Return Value:
        long - please see return code list

  See Also:
        

  

  Date:   2002-10-13
 */
long SEMETHOD SE_REQ_AddFloatArray(long size);

/*@@
  Description:
        Add a float array element

  Remarks:
        

  Parameters:
        float p	 - [in]float array element

  Return Value:
        long - please see return code list

  See Also:
        

  

  Date:   2002-10-13
 */
long SEMETHOD SE_REQ_AddFloatElement(float p);

/*@@
  Description:
        Add a string array as parameter to reqeust

  Remarks:
        

  Parameters:
        long size	 - [in]array size

  Return Value:
        long - please see return code list

  See Also:
        

  

  Date:   2002-10-13
 */
long SEMETHOD SE_REQ_AddStringArray(long size);

/*@@
  Description:
        Add a string array element

  Remarks:
        

  Parameters:
        char* p	 - [in]string array element

  Return Value:
        long - please see return code list

  See Also:
        

  

  Date:   2002-10-13
 */
long SEMETHOD SE_REQ_AddStringElement(char* p);

/*@@
  Description:
        Stop all virtual machine launched by SE_ExecScript()

  Remarks:
        

  Parameters:
        NONE


  Return Value:
        void - 

  See Also:
        

  

  
 */
void SEMETHOD SE_StopAllVM();

/*@@
  Description:
        get number of active vm

  Remarks:
        

  Parameters:
        NONE


  Return Value:
        long SEMETHOD- active vm number

  See Also:
        

  Author: Jackie Ju 

  Date:   2003-5-21
 */
//long SEMETHOD  SE_GetActiveVMNum();


/*@@
  Description:
        get status of all active VM

  Remarks:
        

  Parameters:
        NONE


  Return Value:
        std::vector<VMINFO>* SEMETHOD- 

  See Also:
        

  Author: Jackie Ju 

  Date:   2003-5-21
 */
//std::vector<VMINFO>* SEMETHOD  SE_GetActiveVMStatus();


/*@@
  Description:
        Release vm status got by SE_GetActiveVMStatus

  Remarks:
        

  Parameters:
        std::vector<VMINFO>* p	 - [in]vm status got by SE_GetActiveVMStatus

  Return Value:
        void SEMETHOD- 

  See Also:
        

  Author: Jackie Ju 

  Date:   2003-5-21
 */
//void SEMETHOD  SE_ReleaseVMStatus(std::vector<VMINFO>* p);



//---------- debug input method ----------//

/*@@
  Description:
        put one debug command into VM. You should include the "debug.h" which define the debug command id


  Remarks:
		This function is to give a debug command to VM. The VM must work in the debug mode.
		Following is the list of debug comand:
		<table>
				Commond ID		Function								param 1				param 2			param 3
				----------		----------								----------			-------------	-----------------------------------------------------------------------------------------------------------------------------
				DBGCMD_NULL		No command																	
				DBGCMD_FORWARD	go forward n instruction continuously	commond number								
				DBGCMD_HALT		Stop VM																				
				DBGCMD_MEM		show memory								start address		byte number					
				DBGCMD_REG		show register																reg name, should one of "ax", "bx", "cx", "dx", "ex", "psw", "ip", and the case is significant, if "", show all registers
				DBGCMD_BP		show all break point																
				DBGCMD_SP		add a breakpoint						line number									
				DBGCMD_DP		remove a breakpoint						line number									
				DBGCMD_GO		Go																					
		</table>
		VM has 8 registers:
		AX, BX, CX, DX is general register
		PSW is machine status register
		IP is intruction address register, storing the next instruction address

  Parameters:
        lDbgCmdID	 - [in] debug command id
        p1	 - [in]	parameter 1
        p2	 - [in]	parameter 2
        p3	 - [in]	parameter 3

  Return Value:
        void - 

  See Also:
        

  Author: Jackie Ju 

  Date:   2002-10-24
 */
void SEMETHOD SE_SetDbgCmd(long lDbgCmdID, long p1, long p2, char* p3);

/*@@
  Description:
        get out msg from vm. The VM must work in the debug mode.

  Remarks:
        

  Parameters:
        std::string &msg	 - [out]

  Return Value:
        long - if function succeeded, return 0, else return negative value

  See Also:
        

  Author: Jackie Ju 

  Date:   2002-10-24
 */
long SEMETHOD SE_GetDbgOut(std::string &msg);

/*@@
  Description:
        Get register valuse

  Remarks:
        

  Parameters:
        long lIndex	 - [in]

  Return Value:
        long - 

  See Also:
        

  Author: Jackie Ju 

  Date:   2002-10-24
 */
long SEMETHOD SE_GetDebugVMReg(long lIndex);

#ifndef WIN32

}
#endif
#endif //__SE_INTERFACE_H__
