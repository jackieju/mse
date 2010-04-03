#ifndef __VMEXCEPTION_H__
#define __VMEXCEPTION_H__

#define MSGSIZE 201

class CVMException
{
private:
	char m_sMsg[201];
	long m_lExpCode;

public:
	char* GetExp(long& lExpCode){lExpCode = m_lExpCode; return m_sMsg;};
	CVMException(char* sMsg, long lCode);
	virtual	~CVMException();

};



#endif //__VMEXCEPTION_H__

