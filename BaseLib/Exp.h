#ifndef __EXP_H__
#define __EXP_H__

#define MSGSIZE 201

#define err_basic_base -1000
#define err_user_base  -100000

class CExp
{
private:
	char m_sMsg[201];
	long m_lExpCode;
	long m_lState;

public:
	char* GetExp(long& lExpCode, long &lState){lExpCode = m_lExpCode; lState = m_lState; return m_sMsg;};
	long GetExpCode(){
		return m_lExpCode;		
	};
	char* GetExpMsg(){return m_sMsg;};
	long  GetState(){
		return m_lState;
	};
	CExp(long lCode, char* sMsg, long lState);
	virtual	~CExp();

public:
	// return code
	const static	long ret_ok;
	const static	long err_invalid_param;
	const static	long err_allocate_memory;
	const static	long err_unexpected_status;
	const static	long err_createdbconnectpool_failed;
	const static	long err_getdbstmt_failed;
	const static	long err_dbop_failed;
	const static	long err_createevent_failed;

//	const static	long err_user_defined;

	// status
	const static	long stat_null;

	const static	long stat_uninitialized;
	const static	long stat_init_ok;
	const static	long stat_starting_no_err;
	const static	long stat_starting_with_err;
	const static	long stat_stoping;
	const static	long stat_stopped_ok;
	const static	long stat_stopped_with_err;
	const static	long stat_user_defined;
};



#endif //__VMEXCEPTION_H__