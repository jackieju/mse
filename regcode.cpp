#include "os/os.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "gencode.h"

char* GenCode(char* sUsername)
{
	char key[KEY_NUMBER][KEY_LENGTH+1] = {
			"dasfiomfdk",
			"dafebbbaaa",
			"baeefasfee",
			"dffweegfbb",
			"eewwaaaaag",
			"oiyiiouioi",
			"jhjmtyymrt",
			"asbaaefeff",
			"eeererwewe",
			"fbbbrfdaww",
	};
	if (sUsername == NULL)
	{
		printf("GenCode: input sUsername is null\n");
		return NULL;
	}

	int i;
	char *output = new char[MAX_CODELENGTH + 1];
	if (output == NULL)
	{
		return NULL;
	}
	memset(output, 0, MAX_CODELENGTH + 1);

	char input[MAX_CODELENGTH + 1] = "";
	long key_index = 0;
	long code_len = 10;

	// make Username
	long len = strlen(sUsername);
	if (len < 6)
	{
		printf("length of user name must be greater than 6 letters\n");
		return NULL;
	}

	long j = 0;
	for (i = 0; i < len; i++)
	{
		if (!isalpha(sUsername[i]))
		{
			printf("username must be composed of 'a~z' or 'A~Z'\n");
			return NULL;
		}
		j = (i + sUsername[i]) % 5;
		switch (j)
		{
			case 0: key_index += sUsername[i];code_len += sUsername[i];break;
			case 1: key_index += sUsername[i]*2;code_len += sUsername[i]*2;break;
			case 2: key_index += sUsername[i] * i;code_len += sUsername[i]*+i;break;
			case 3: key_index += sUsername[i] % i;code_len += sUsername[i]%i;break;
			case 4: key_index += sUsername[i] / i;code_len += sUsername[i]/i;break;
		}
		
	}

	// make key_index (0 to 9)	
	key_index = key_index%KEY_NUMBER;

	// make code len (6 to 30)
	code_len = code_len%20 + 6;

	// make input
	snprintf(input, MAX_CODELENGTH, "%s", sUsername);
	if (len < code_len)
	{
		int i = 0;
		for (; i < code_len - len; i++)
		{			
			input[len + i] = 'a' + (key_index + code_len + i)%26;
		}
	}
	for ( i = 0; i < code_len; i++)
	{
		output[i] = 'a' + (key[key_index][i%KEY_LENGTH] * input[i]) % 26;
	}


//	printf(output);

	return output;
}

bool CheckUserCode(char* sUsername, char* code)
{
	if (sUsername == NULL || code == NULL)
	{
		printf("CheckUserCode: input username or code is null\n");
		return false;
	}
	char *check_code = GenCode(sUsername);
	if (check_code == NULL)
	{
		printf("CheckUserCode: Generate check code failed\n");
		return false;
	}

	if (strcmp(check_code, code))
		return false;

	return true;
}