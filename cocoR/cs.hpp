//****************************************************************
//   CPLUS2\SCAN_H.FRM
//   Coco/R C++ Support Frames.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   Jun 12, 1996  Version 1.06
//      Many fixes and suggestions thanks to
//      Pat Terry <p.terry@.ru.ac.za>
//   Oct 31, 1999  Version 1.14
//      LeftContext Support
//   Mar 24, 2000  Version 1.15
//      LeftContext Support no longer needed
//****************************************************************

#ifndef cScanner_INCLUDE
#define cScanner_INCLUDE

#include "cr_scan.hpp"

#if __MSDOS__ || MSDOS
#       define SCANNER  CRDosScanner
#else
#       define SCANNER  CRScanner
#endif
#define MAX_IDENTIFIER_LENGTH 64
class cScanner : public SCANNER
{
  public:
    cScanner(int SrcFile, int ignoreCase) : SCANNER(SrcFile, ignoreCase) {};
    	char* GetName(){
		szName[0] = 0;
		CRScanner::GetName(&CurrSym, szName, MAX_IDENTIFIER_LENGTH-1);//µÃµ½Ãû³Æ;
		return szName;
	}
  protected:
    static int STATE0[];
    int CheckLiteral(int id);
    int LeftContext(char *s);
    int Comment();
    int Get();
    char szName[MAX_IDENTIFIER_LENGTH];
};

#endif /* cScanner_INCLUDE */

