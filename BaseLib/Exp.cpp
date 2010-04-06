#include "stdio.h"
#include "memory.h"
#include <string.h>
#include <stdlib.h>
#include "exp.h"

const long CExp::ret_ok							= 0;
const long CExp::err_invalid_param				= err_basic_base + 1;
const long CExp::err_allocate_memory			= err_basic_base + 2;
const long CExp::err_unexpected_status			= err_basic_base + 3;
const long CExp::err_createdbconnectpool_failed = err_basic_base + 4;
const long CExp::err_getdbstmt_failed			= err_basic_base + 5;
const long CExp::err_dbop_failed				= err_basic_base + 6;
const long CExp::err_createevent_failed			= err_basic_base + 7;

//const long CExp::err_user_defined				= 100;

// status
const long CExp::stat_null				= -1;
const long CExp::stat_uninitialized		= 0;
const long CExp::stat_init_ok			= 2;
const long CExp::stat_starting_no_err	= 3;
const long CExp::stat_starting_with_err	= 4;
const long CExp::stat_stoping			= 5;
const long CExp::stat_stopped_ok		= 6;
const long CExp::stat_stopped_with_err	= 7;
const long CExp::stat_user_defined		= 100;

CExp::CExp(long lCode, char* sMsg,  long lState)
{
	m_lState = stat_null;

	memset(m_sMsg, 0, 201);
	if (sMsg)
	{
		snprintf(m_sMsg, 200, "%s", sMsg);
	}

	m_lExpCode = lCode;

}

CExp::~CExp()
{
}