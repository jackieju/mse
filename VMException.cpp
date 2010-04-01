#include "os/os.h"
#include "stdio.h"
#include "memory.h"
#include "VMException.h"

CVMException::CVMException(char* sMsg, long lCode)
{
	memset(m_sMsg, 0, 201);
	if (sMsg)
	{
		snprintf(m_sMsg, 200, "%s", sMsg);
	}
	m_lExpCode = lCode;
}

CVMException::~CVMException()
{
	return;
}