#ifndef __DEBUG_H__
#define __DEBUG_H__
// debug command
#define DBGCMD_NULL		0	//	No command
#define DBGCMD_FORWARD	1	//	go forward n instruction continuously
#define DBGCMD_HALT		2	//	Stop VM
#define DBGCMD_MEM		3	//	show memory
#define DBGCMD_REG		4   //  show register
#define DBGCMD_BP		5	//	show all break point
#define DBGCMD_SP		6	//	add a breakpoint
#define DBGCMD_DP		7	//	remove a breakpoint
#define DBGCMD_GO		8	//	Go

#endif //__DEBUG_H__


