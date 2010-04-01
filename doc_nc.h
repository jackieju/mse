// this header file is noly for document generation

//@@$NC Developer's Guide
//<TITLE NC Developer's Guide>

/*@@Syntax Specification  Overview
<GROUP $NC Developer's Guide>
The NC language is a reduced C language. It's syntax is a subset of C syntax. So some features of C are not supported by NC language, which should be take a look
 */

/*@@Syntax Specification 
<GROUP $NC Developer's Guide>
The syntax is defined in <EXTLINK href="nc.arg">atg file</EXTLINK>(used by cocoR).
NOTICE: Although the <font color = "red">switch</font> sentence can be parsed, but it was not be implemented, so you can not use switch sentence.  
 */

/*@@Data Type Supported
<GROUP $NC Developer's Guide>
char(1 bytes), short(2 bytes), long(4 bytes), float(4 bytes) NOTICE: unsigned types are not supported by NC. 
 
 */

/*@@Data Type Cast
<GROUP $NC Developer's Guide>
NC noly support the implicit type cast, but not the explicit cast.

 */

/*@@Operator Supported

<GROUP $NC Developer's Guide>
1. Mathematic	:	+ - * / % ++ --
2. Logical		:	< > <= >= ==
3. Relational	:	&& || !
4. others		:	& ~
 */

/*@@Function Call
<GROUP $NC Developer's Guide>
There are tow kinds of function: NC Function and external lib function
If call a NC function, the called function must be defined before the caller.
If call a external lib function, the called function must be loaded before the 
caller be compiled. 
 */

/*@@Other Specifications
<GROUP $NC Developer's Guide>
1. The variables can only be defined in the start of the function, and the 
2. variable can not be initialized while being difined. 
3. The following operators are not supported:
4. +=, -=, *=, /=, %=, |, & 
5. class and structure are not supported in the version 
6. swith statement are not supported 
7. bit operators are not supported 
8. dose not support new and delete 
9. dose not support comma expression 
 */


//@@$FAQ
//<title FAQ>

/*@@What is NCSE
<GROUP $FAQ>
*/
