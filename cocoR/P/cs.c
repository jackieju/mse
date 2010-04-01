//****************************************************************
//   CPLUS2\SCAN_C.FRM
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

#include "cc.h"
#include "cs.h"

#define Scan_Ch        Ch
#define Scan_NextCh    NextCh
#define Scan_ComEols   ComEols
#define Scan_CurrLine  CurrLine
#define Scan_CurrCol   CurrCol
#define Scan_LineStart LineStart
#define Scan_BuffPos   BuffPos
#define Scan_NextLen   NextSym.Len

int cScanner::STATE0[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,54,24,33,0,64,49,26,44,45,37,61,41,62,67,63,35,2,2,2,2,2,2,2,2,2,46,38,
                  30,52,56,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,42,0,43,51,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,39,47,40,79,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int cScanner::CheckLiteral(int id)
{ char c;
  c =  CurrentCh(NextSym.Pos);
  if (IgnoreCase) c = Upcase(c);
  switch (c) {
  	case 'a':
  		if (EqualStr("auto")) return autoSym;
  		break;
  	case 'b':
  		if (EqualStr("break")) return breakSym;
  		break;
  	case 'c':
  		if (EqualStr("class")) return classSym;
  		if (EqualStr("char")) return charSym;
  		if (EqualStr("case")) return caseSym;
  		if (EqualStr("continue")) return continueSym;
  		break;
  	case 'd':
  		if (EqualStr("double")) return doubleSym;
  		if (EqualStr("default")) return defaultSym;
  		if (EqualStr("do")) return doSym;
  		break;
  	case 'e':
  		if (EqualStr("extern")) return externSym;
  		if (EqualStr("else")) return elseSym;
  		break;
  	case 'f':
  		if (EqualStr("float")) return floatSym;
  		if (EqualStr("for")) return forSym;
  		break;
  	case 'i':
  		if (EqualStr("inherit")) return inheritSym;
  		if (EqualStr("int")) return intSym;
  		if (EqualStr("if")) return ifSym;
  		break;
  	case 'l':
  		if (EqualStr("long")) return longSym;
  		break;
  	case 'r':
  		if (EqualStr("register")) return registerSym;
  		if (EqualStr("return")) return returnSym;
  		break;
  	case 's':
  		if (EqualStr("static")) return staticSym;
  		if (EqualStr("short")) return shortSym;
  		if (EqualStr("string")) return stringSym;
  		if (EqualStr("switch")) return switchSym;
  		break;
  	case 'u':
  		if (EqualStr("unsigned")) return unsignedSym;
  		break;
  	case 'v':
  		if (EqualStr("void")) return voidSym;
  		break;
  	case 'w':
  		if (EqualStr("while")) return whileSym;
  		break;
  
  }
  return id;
}

int cScanner::Comment()
{ int Level, StartLine, OldCol;
  long OldLineStart;

  Level = 1; StartLine = CurrLine;
  OldLineStart = LineStart; OldCol = CurrCol;
  if (Scan_Ch == '/') { /* 1 */
  	Scan_NextCh();
  	if (Scan_Ch == '*') { /* 2 */
  		Scan_NextCh();
  		while (1) {
  			if (Scan_Ch== '*') { /* 5 */
  				Scan_NextCh();
  				if (Scan_Ch == '/') { /* 6 */
  					Level--; Scan_NextCh(); Scan_ComEols = Scan_CurrLine - StartLine;
  					if(Level == 0) return 1;
  				} /* 6 */ 
  			} else /* 5 */
  			if (Scan_Ch == EOF_CHAR) return 0;
  			else Scan_NextCh();
  		} /* while */
  	} else { /* 2 */
  		if (Scan_Ch == LF_CHAR) { Scan_CurrLine--; Scan_LineStart = OldLineStart; }
  		Scan_BuffPos -= 2; Scan_CurrCol = OldCol - 1; Scan_NextCh();
  	} /* 2 */
  } /* 1*/
  if (Scan_Ch == '/') { /* 1 */
  	Scan_NextCh();
  	if (Scan_Ch == '/') { /* 2 */
  		Scan_NextCh();
  		while (1) {
  			if (Scan_Ch== 10) { /* 5 */
  				Level--; Scan_NextCh(); Scan_ComEols = Scan_CurrLine - StartLine;
  				if(Level == 0) return 1;
  			} else /* 5 */
  			if (Scan_Ch == EOF_CHAR) return 0;
  			else Scan_NextCh();
  		} /* while */
  	} else { /* 2 */
  		if (Scan_Ch == LF_CHAR) { Scan_CurrLine--; Scan_LineStart = OldLineStart; }
  		Scan_BuffPos -= 2; Scan_CurrCol = OldCol - 1; Scan_NextCh();
  	} /* 2 */
  } /* 1*/
  
  return 0;
}

int cScanner::Get()
{ int state, ctx;

  start:
    while (Scan_Ch >= 9 && Scan_Ch <= 10 ||
           Scan_Ch == 13 ||
           Scan_Ch == ' ') Scan_NextCh();
    if ((Scan_Ch == '/') && Comment()) goto start;

    CurrSym = NextSym;
    NextSym.Init(0, CurrLine, CurrCol - 1, BuffPos, 0);
    NextSym.Len  = 0; ctx = 0;

    if (Ch == EOF_CHAR) return EOF_Sym;
    state = STATE0[Ch];
    while(1) {
      Scan_NextCh(); NextSym.Len++;
      switch (state) {
       /* State 0; valid STATE0 Table
      case 0:
      	if (Scan_Ch >= 'A' && Scan_Ch <= 'Z' ||
          Scan_Ch == '_' ||
          Scan_Ch >= 'a' && Scan_Ch <= 'z') state = 1; else
      	if (Scan_Ch >= '1' && Scan_Ch <= '9') state = 2; else
      	if (Scan_Ch == '0') state = 35; else
      	if (Scan_Ch == '"') state = 24; else
      	if (Scan_Ch == 39) state = 26; else
      	if (Scan_Ch == '<') state = 30; else
      	if (Scan_Ch == '#') state = 33; else
      	if (Scan_Ch == '*') state = 37; else
      	if (Scan_Ch == ';') state = 38; else
      	if (Scan_Ch == '{') state = 39; else
      	if (Scan_Ch == '}') state = 40; else
      	if (Scan_Ch == ',') state = 41; else
      	if (Scan_Ch == '[') state = 42; else
      	if (Scan_Ch == ']') state = 43; else
      	if (Scan_Ch == '(') state = 44; else
      	if (Scan_Ch == ')') state = 45; else
      	if (Scan_Ch == ':') state = 46; else
      	if (Scan_Ch == '|') state = 47; else
      	if (Scan_Ch == '&') state = 49; else
      	if (Scan_Ch == '^') state = 51; else
      	if (Scan_Ch == '=') state = 52; else
      	if (Scan_Ch == '!') state = 54; else
      	if (Scan_Ch == '>') state = 56; else
      	if (Scan_Ch == '+') state = 61; else
      	if (Scan_Ch == '-') state = 62; else
      	if (Scan_Ch == '/') state = 63; else
      	if (Scan_Ch == '%') state = 64; else
      	if (Scan_Ch == '.') state = 67; else
      	if (Scan_Ch == '~') state = 79; else
      	return No_Sym;
      	break;
       --------- End State0 --------- */
      case 1:
      	if (Scan_Ch >= '0' && Scan_Ch <= '9' ||
      	    Scan_Ch >= 'A' && Scan_Ch <= 'Z' ||
      	    Scan_Ch == '_' ||
      	    Scan_Ch >= 'a' && Scan_Ch <= 'z') /*same state*/; else
      	return CheckLiteral(identifierSym);
      	break;
      case 2:
      	if (Scan_Ch == 'U') state = 5; else
      	if (Scan_Ch == 'u') state = 6; else
      	if (Scan_Ch == 'L') state = 7; else
      	if (Scan_Ch == 'l') state = 8; else
      	if (Scan_Ch == '.') state = 4; else
      	if (Scan_Ch >= '0' && Scan_Ch <= '9') /*same state*/; else
      	return numberSym;
      	break;
      case 4:
      	if (Scan_Ch == 'U') state = 13; else
      	if (Scan_Ch == 'u') state = 14; else
      	if (Scan_Ch == 'L') state = 15; else
      	if (Scan_Ch == 'l') state = 16; else
      	if (Scan_Ch >= '0' && Scan_Ch <= '9') /*same state*/; else
      	return numberSym;
      	break;
      case 5:
      	return numberSym;
      case 6:
      	return numberSym;
      case 7:
      	return numberSym;
      case 8:
      	return numberSym;
      case 13:
      	return numberSym;
      case 14:
      	return numberSym;
      case 15:
      	return numberSym;
      case 16:
      	return numberSym;
      case 18:
      	if (Scan_Ch >= '0' && Scan_Ch <= '9' ||
      	    Scan_Ch >= 'A' && Scan_Ch <= 'F' ||
      	    Scan_Ch >= 'a' && Scan_Ch <= 'f') state = 19; else
      	return No_Sym;
      	break;
      case 19:
      	if (Scan_Ch == 'U') state = 20; else
      	if (Scan_Ch == 'u') state = 21; else
      	if (Scan_Ch == 'L') state = 22; else
      	if (Scan_Ch == 'l') state = 23; else
      	if (Scan_Ch >= '0' && Scan_Ch <= '9' ||
      	    Scan_Ch >= 'A' && Scan_Ch <= 'F' ||
      	    Scan_Ch >= 'a' && Scan_Ch <= 'f') /*same state*/; else
      	return hexnumberSym;
      	break;
      case 20:
      	return hexnumberSym;
      case 21:
      	return hexnumberSym;
      case 22:
      	return hexnumberSym;
      case 23:
      	return hexnumberSym;
      case 24:
      	if (Scan_Ch == '"') state = 25; else
      	if (Scan_Ch >= ' ' && Scan_Ch <= '!' ||
      	    Scan_Ch >= '#' && Scan_Ch <= 255) /*same state*/; else
      	return No_Sym;
      	break;
      case 25:
      	return stringD1Sym;
      case 26:
      	if (Scan_Ch >= ' ' && Scan_Ch <= '&' ||
      	    Scan_Ch >= '(' && Scan_Ch <= '[' ||
      	    Scan_Ch >= ']' && Scan_Ch <= 255) state = 28; else
      	if (Scan_Ch == 92) state = 36; else
      	return No_Sym;
      	break;
      case 28:
      	if (Scan_Ch == 39) state = 29; else
      	return No_Sym;
      	break;
      case 29:
      	return charSym;
      case 30:
      	if (Scan_Ch == '.' ||
      	    Scan_Ch >= '0' && Scan_Ch <= ':' ||
      	    Scan_Ch >= 'A' && Scan_Ch <= 'Z' ||
      	    Scan_Ch == 92 ||
      	    Scan_Ch >= 'a' && Scan_Ch <= 'z') state = 31; else
      	if (Scan_Ch == '=') state = 57; else
      	if (Scan_Ch == '<') state = 59; else
      	return LessSym;
      	break;
      case 31:
      	if (Scan_Ch == '>') state = 32; else
      	if (Scan_Ch == '.' ||
      	    Scan_Ch >= '0' && Scan_Ch <= ':' ||
      	    Scan_Ch >= 'A' && Scan_Ch <= 'Z' ||
      	    Scan_Ch == 92 ||
      	    Scan_Ch >= 'a' && Scan_Ch <= 'z') /*same state*/; else
      	return No_Sym;
      	break;
      case 32:
      	return librarySym;
      case 33:
      	if (Scan_Ch >= 'A' && Scan_Ch <= 'Z' ||
      	    Scan_Ch >= 'a' && Scan_Ch <= 'z') state = 34; else
      	return No_Sym;
      	break;
      case 34:
      	return PreProcessorSym;
      case 35:
      	if (Scan_Ch == 'U') state = 5; else
      	if (Scan_Ch == 'u') state = 6; else
      	if (Scan_Ch == 'L') state = 7; else
      	if (Scan_Ch == 'l') state = 8; else
      	if (Scan_Ch == '.') state = 4; else
      	if (Scan_Ch >= '0' && Scan_Ch <= '9') state = 2; else
      	if (Scan_Ch == 'X' ||
      	    Scan_Ch == 'x') state = 18; else
      	return numberSym;
      	break;
      case 36:
      	if (Scan_Ch >= ' ' && Scan_Ch <= '&' ||
      	    Scan_Ch >= '(' && Scan_Ch <= 255) state = 28; else
      	if (Scan_Ch == 39) state = 29; else
      	return No_Sym;
      	break;
      case 37:
      	if (Scan_Ch == '=') state = 69; else
      	return StarSym;
      	break;
      case 38:
      	return SemicolonSym;
      case 39:
      	return LbraceSym;
      case 40:
      	return RbraceSym;
      case 41:
      	return CommaSym;
      case 42:
      	return LbrackSym;
      case 43:
      	return RbrackSym;
      case 44:
      	return LparenSym;
      case 45:
      	return RparenSym;
      case 46:
      	return ColonSym;
      case 47:
      	if (Scan_Ch == '|') state = 48; else
      	if (Scan_Ch == '=') state = 76; else
      	return BarSym;
      	break;
      case 48:
      	return BarBarSym;
      case 49:
      	if (Scan_Ch == '&') state = 50; else
      	if (Scan_Ch == '=') state = 74; else
      	return AndSym;
      	break;
      case 50:
      	return AndAndSym;
      case 51:
      	if (Scan_Ch == '=') state = 75; else
      	return UparrowSym;
      	break;
      case 52:
      	if (Scan_Ch == '=') state = 53; else
      	return EqualSym;
      	break;
      case 53:
      	return EqualEqualSym;
      case 54:
      	if (Scan_Ch == '=') state = 55; else
      	return BangSym;
      	break;
      case 55:
      	return BangEqualSym;
      case 56:
      	if (Scan_Ch == '=') state = 58; else
      	if (Scan_Ch == '>') state = 60; else
      	return GreaterSym;
      	break;
      case 57:
      	return LessEqualSym;
      case 58:
      	return GreaterEqualSym;
      case 59:
      	if (Scan_Ch == '=') state = 77; else
      	return LessLessSym;
      	break;
      case 60:
      	if (Scan_Ch == '=') state = 78; else
      	return GreaterGreaterSym;
      	break;
      case 61:
      	if (Scan_Ch == '+') state = 65; else
      	if (Scan_Ch == '=') state = 72; else
      	return PlusSym;
      	break;
      case 62:
      	if (Scan_Ch == '-') state = 66; else
      	if (Scan_Ch == '>') state = 68; else
      	if (Scan_Ch == '=') state = 73; else
      	return MinusSym;
      	break;
      case 63:
      	if (Scan_Ch == '=') state = 70; else
      	return SlashSym;
      	break;
      case 64:
      	if (Scan_Ch == '=') state = 71; else
      	return PercentSym;
      	break;
      case 65:
      	return PlusPlusSym;
      case 66:
      	return MinusMinusSym;
      case 67:
      	return PointSym;
      case 68:
      	return MinusGreaterSym;
      case 69:
      	return StarEqualSym;
      case 70:
      	return SlashEqualSym;
      case 71:
      	return PercentEqualSym;
      case 72:
      	return PlusEqualSym;
      case 73:
      	return MinusEqualSym;
      case 74:
      	return AndEqualSym;
      case 75:
      	return UparrowEqualSym;
      case 76:
      	return BarEqualSym;
      case 77:
      	return LessLessEqualSym;
      case 78:
      	return GreaterGreaterEqualSym;
      case 79:
      	return TildeSym;
      
      default: return No_Sym; /* Scan_NextCh already done */
      }
    }
}

