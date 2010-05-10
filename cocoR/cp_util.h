#ifndef __CP_UTIL_H
#define __CP_UTIL_H

 class CPUtil{
	public:
		long typesize(long type, long id)
{
#ifdef _64
	switch (type)
	{
	case dtChar:
	case dtUChar:
		return sizeof(char);break;
	case dtShort:
	case dtUShort:
		return sizeof(short);break;
	case dtInt:
	case dtUInt:		
	case dtLong:
	case dtULong:
	case dtFloat:
		return sizeof(long);break;
	case dtStr:
		return sizeof(void*);break; //it's a pointer
	case dtGeneral:		
		/*if (id == 0)
		{
			nLOG("OBJECT WITH ERROR ID", 300);
			return -1;
		}
		return m_ObjTable[id-1]->GetSize();*/
		return sizeof(void*);break;//it's a pointer
		break;

/*	case dtDouble:
		return 8;break;
*/	default: return -1;
	}
#else
	switch (type)
	{
	case dtChar:
	case dtUChar:
		return 1;break;
	case dtShort:
	case dtUShort:
		return 2;break;
	case dtInt:
	case dtUInt:		
	case dtLong:
	case dtULong:
	case dtFloat:
		return 4;break;
	case dtStr:
		return sizeof(void*);break; //it's a pointer
	case dtGeneral:		
		/*if (id == 0)
		{
			nLOG("OBJECT WITH ERROR ID", 300);
			return -1;
		}
		return m_ObjTable[id-1]->GetSize();*/
		return sizeof(void*);break;//it's a pointer
		break;

/*	case dtDouble:
		return 8;break;
*/	default: return -1;
	}
#endif
}
int log2(int x)
{
	printf("===>x=%d\n", x);
	int i = 0;
	while (x >= 2)
	{
		i++;
		x >>= 1;
		printf("->x=%d", x);
	}
	if (i >=0 && i<=3)
		return i;
	else
		return 0;
}

	
	
};

#endif
