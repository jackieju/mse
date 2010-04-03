// this header file is noly for document generation

//@@$Introduction
//<TITLE Introduction>

/*@@Overview
<GROUP $Introduction>
Do you want to make your game or software system have nice programablity like QuakeIII or Unreal, which both use the script engine technology? NewC(NC) is one c-like program language. The NCSE(NewC script engine) provide the compiler and the executor of NC program. To Embed NCSE in your system will enhance your system's extendibility and flexibility hugely. 
The NCSE is consist of two parts: NC compiler and virtual machine. The NC compiler compile the NC source to one kind of middle-code, which is platform-independent. The NC viurtal machine execute the middle-code. 

The goal of NC is to seperate the business logic from software development. 

All other program languages blend the logic with implementation, which make the burden of business developer. In NC architecture, the concrete implementation of system is provided by third-party vendor, such as http communictaion or database access. This make the NC developer need not take care of how the logic is implemented. That is, the NC is only responsible for describing the business logic of one software system, and care nothing for the concrete implementation(such as io, DataBase). When the requirement change, the developer should noly modify the NC code but not other implements. A System integrated with NCSE will present the significant flexibility. 

 
 */

/*@@Architecture 
<GROUP $Introduction>
NCSE is composed of the NC complier and NC Assembly code Executor. The NC compiler compile the NC source to NC Assembly code. The NC assembly code executor can run the NC assembly code. That user should privode is the external Libs(in win32 is dll), and NC source. 


architecture: <br>
<IMAGE index.1.gif>
 */


