#ifndef __GENCODE_H__
#define __GENCODE_H__

#define KEY_LENGTH 10
#define KEY_NUMBER 10
#define MAX_CODELENGTH 30

char* GenCode(char* sUsername);
bool CheckUserCode(char* sUsername, char* code);

#endif //__GENCODE_H__
