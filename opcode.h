/*
	File name		: opcode.h
	Decsription		: define assemble instructions and address mode of VM
*/
#ifndef OPCODE_H
#define OPCODE_H
//����ָ��
#define __mov		0x1010							//mov (����, ����)
#define __ea		0x10f4							//ȡ��Ч��ַ(ֻ����AMODE_MEMѰַ��ʽ)

//��������ָ��
//����
#define __add		0x1021			            //add (����, ����)-> ax
#define __sub		0x1050						//sub (����, ����)
#define __mul		0x1070						//mul (����, ����)
#define __div		0x1090						//div (����, ����)
#define __mod		0x10f0						//%
#define __not		0x10d0						//not (����), �൱��~����
#define __test		0x10f2						//�Ƚ�test(a, b, ����)
#define __notr		0x10f5						//(__notr(����) !����
//����
#define __fadd		0x1022
#define __fsub		0x1051
#define __fmul		0x1071
#define __fdiv		0x1091

//��תָ��
#define __jmp		0x10a0                     //jmp (����)
#define __jz		0x10a1						//jz (���)
#define __jnz		0x10a2						//jnz (constant)
#define __ret		0x10f3
/*#define __jmpz   0x10a2                   //jmpz(address, constant) 
*/

// �����ⲿ����
#define __callpub		0x10a3					//call (function entry)
#define __parampub		0x10a4					//param (address)
#define __endcallpub	0x10a5					//endcall

// ���ýű�����
#define __callv			0x10a6
#define __paramv		0x10a7
#define __endcallv		0x10a8

// extending for script-like interpreting
#define __loadlib		0x10a9				// LoadLib xxx
#define __eaobj			0x10aa				// get address of object
#define __newobj		0x10ab				// create new object or array
#define __movobj		0x10ac				// assignment for object

//����ת��(__cast(Ŀ���ַ,  ԭ��ַ, ת������)
#define	__cast			0x10f6				

//����ת����
//ʹ�ó���
#define __CAST_C2F 0 // char  -> float
#define __CAST_S2F 1 // short -> float
#define __CAST_L2F 2 // long  -> float
#define __CAST_I2F 3 // int   -> float
#define __CAST_C2L 4 // char  -> long
#define __CAST_S2L 5 // short -> long
#define __CAST_I2L 6 // int   -> long
#define __CAST_C2S 7 // char  -> short
#define __CAST_L2S 8 // long  -> short
#define __CAST_I2S 9 // int   -> short
#define __CAST_S2C 10// short -> char
#define __CAST_L2C 11// long  -> char
#define __CAST_I2C 12// int   -> char
#define __CAST_C2I 13// char  -> int
#define __CAST_S2I 14// short -> int
#define __CAST_L2I 15// long  -> int

/*
//ʹ���������͵����
#define __CAST_C2F (dtChar<<8||dtFloat) // char  -> float
#define __CAST_S2F (dtShort<<8||dtFloat) // short -> float
#define __CAST_L2F (dtLong<<8||dtFloat) // long  -> float
#define __CAST_I2F (dtInt<<8||dtFloat) // int   -> float
#define __CAST_C2L (dtChar<<8||dtLong) // char  -> long
#define __CAST_S2L (dtShort<<8||dtLong) // short -> long
#define __CAST_I2L (dtInt<<8||dtLong) // int   -> long
#define __CAST_C2S (dtChar<<8||dtShort) // char  -> short
#define __CAST_L2S (dtLong<<8||dtShort) // long  -> short
#define __CAST_I2S (dtInt<<8||dtShort) // int   -> short
#define __CAST_S2C (dtShort<<8||dtChar)// short -> char
#define __CAST_L2C (dtLong<<8||dtChar)// long  -> char
#define __CAST_I2C (dtInt<<8||dtChar)// int   -> char
#define __CAST_C2I (dtChar<<8||dtInt)// char  -> int
#define __CAST_S2I (dtShort<<8||dtInt)// short -> int
#define __CAST_L2I (dtLong<<8||dtInt)// long  -> int
*/
#define OPSIZE_1B 0x00
#define OPSIZE_2B 0x40
#define OPSIZE_4B 0x80
#define OPSIZE_8B 0xc0
#define OPSIZE_PTR sizeof(long*)
/*
//ջ����
Ŀǰ����, ��Ϊû�ж�ջ��
#define __pop    0x10b0                     //pop (����)
#define __push   0x10c0                    //push (�Ĵ���, ����)
*/
                                       //��: test(a, b, 0)

                                       //0: == 1: != 2: >= 3:<= 4:> 5:< 6:&& 7:||
                                       //��������ñ�־λ1����������

//address mode
/*
address mode have tow BYTES, the 1st(high) byte is the dest address and the 2nd(low) byte is
the source address.
Every byte has the same format(high to low):
0, 1, 2, 3bit:	
addressing mode, 0: Immediate 1: mem 2: register 3: static 4: preserved
4, 5 bit:		reference level
6, 7 bit:		0: byte operation, 1: word operation 2: DWORD operation 3: preserved
*/
// addressing mode
//0:����c 1: ֱ��Ѱַd 2:registerR 3: ������Ѱַ
//���ӽ�Ϊop1, ���ֽ�Ϊop2
#define FIRST_ADDRESS_MODE 0
#define AMODE_DIRECT 0  //������
#define AMODE_MEM   1   //�ڴ�
#define AMODE_REG   2
#define AMODE_STATIC 3  //������Ѱַ
//#define AMODE_ARRAY   4  //����Ѱַ, ����ַ��ַѰַEA = DS + BX + OP)
#define AMODE_OBJ      4// address mode for object
//#define AMODE_POINT  4 //ָ��Ѱַ(EA = PS + OFFSET)
#define LAST_ADDRESS_MODE 0xf4

#define DD 0x8181
#define CC 0x8080
#define CD 0x8081
#define CR 0x8082
#define DR 0x8182
#define RD 0x8281
#define RR 0x8282
#define RC 0x8280
#define DC 0X8180
#define DS 0x8183
#define DA 0x8184
#define AR 0x8482

#endif

