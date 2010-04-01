#define MAX_SITE_URL_LEN 100
#include <string.h>
int IsWanted(char* pLink,char* pFomate)
{
	if(strlen(pLink) == 0 && strlen(pFomate) == 0)
		return true;
	char pSavedLink[MAX_SITE_URL_LEN];
	char pSavedFomate[MAX_SITE_URL_LEN];

	memset(pSavedLink, 0, MAX_SITE_URL_LEN);
	memset(pSavedFomate, 0, MAX_SITE_URL_LEN);
	strcpy(pSavedLink, pLink);
	strcpy(pSavedFomate, pFomate);
	char *pMark = strchr(pSavedFomate, '*');
	char *pMark2 = strchr(pSavedFomate, '?');
	if(pMark2)
	{
		if(pMark)
			pMark = pMark < pMark2?pMark:pMark2;
		else
			pMark = pMark2;
	}

	if(!pMark)
	{
		if(strcmp(pLink, pFomate) == 0)
			return 1;
		else
			return 0;
	}
	else if(*pMark == '*')
	{
		*pMark = '\0';
		if(strncmp(pSavedLink, pSavedFomate, strlen(pSavedFomate)) == 0)
		{
			if(*(pMark+1) == '\0')				// fomate???¨ª?¨¢??
			{
				return 1;
			}
			else
			{
				pMark++;
				char *pSec = strchr(pMark, '*');
				char *pSec2 = strchr(pMark, '?');
				if(pSec2)
				{
					if(pSec)
						pSec = pSec < pSec2?pSec:pSec2;
					else
						pSec = pSec2;
				}
				if(!pSec)					// ?????¨®????*?¨°???
				{
					if(strlen(pSavedLink) <= strlen(pSavedFomate))		// ¡Á?¡¤????????????¡§????
						return 0;
					else
					{
						char *pThird = pSavedLink + strlen(pSavedFomate);
						if(strlen(pThird) < strlen(pMark))
							return 0;
						else
						{
							pThird += strlen(pThird)-strlen(pMark);
							if(strcmp(pThird, pMark) == 0)
								return 1;
							else
								return 0;
						}
					}
				}
				else
				{
					if(pMark == pSec)				// *?????????????
						return 0;
					char c = *pSec;
					*pSec = 0;
					char *pThird = strstr(pSavedLink + strlen(pSavedFomate), pMark);
					int nL = strlen(pMark);
					if(pThird)						// ?????????????¨¦
					{
						*pSec = c;
						return IsWanted(pThird+nL, pSec);
					}
					else
					{
						return 0;
					}
				}
			}
		}
		else
		{
			return 0;
		}
	}
	else					// ??¡¤????
	{
		*pMark = 0;
		if(strncmp(pSavedLink, pSavedFomate, strlen(pSavedFomate)) == 0)
		{
			if(strlen(pSavedLink) > strlen(pSavedFomate))
			{
				return IsWanted(pSavedLink+strlen(pSavedFomate)+1, pMark+1);
			}
			else
				return 0;
		}
		else
			return 0;
	}

}