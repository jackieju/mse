#ifndef _DATATYPE_H
#define _DATATYPE_H

//define data type
#define dtFirstType 0
#ifdef __SUPPORT_OBJ
#define dtGeneral 0
#else
#define dtDummyType 0
#endif
#define dtShort 1
#define dtInt 2
#define dtLong 3
#define dtChar 4
#define dtUShort 5
#define dtUInt 6
#define dtULong 7
#define dtUChar 8
#define dtString 9  //it is not used by compiler, but by request
#define dtFloat 10
#define dtStr 11
//#define dtLastType 11
#ifdef __SUPPORT_OBJ
#define dtObject 12
#define dtLastType 12
#else
#define dtLastType 11
#endif

typedef struct _tagTypeDes
{
	long type;      //datatype
//	int size_t;
	long refLevel;  //�ж��ٿ��� ��:charΪ0, char*Ϊ1, char**Ϊ2
	long dim;       //���������, ��ʾ����ά��
	long dimsize[8];  //ÿһά�Ĵ�С, ���8ά
	long objID;    //obect�����ͣ� ��ʼֵΪ0
	_tagTypeDes(){
		type = dtGeneral;
		refLevel = 0;// not used in web game c
		dim = 0;
		memset(dimsize, 0, 8*sizeof(long));
		objID = 0;
	}

}TYPEDES, *PTYPEDES;

#define _typedes(var, dt)\
	TYPEDES var;\
	var.type = dt;\
	var.dim = 0;\
	var.refLevel = 0;\
	var.objID = 0;\
	memset(var.dimsize, 0, sizeof(long)*8);
#endif
