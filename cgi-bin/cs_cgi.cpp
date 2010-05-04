#include "stdio.h"
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
unsigned int htoi(char s[])
{
    unsigned int val = 0;
    int x = 0;

    if(s[x] == '0' && (s[x+1]=='x' || s[x+1]=='X')) x+=2;

    while(s[x]!='\0')
    {
       if(val > UINT_MAX) return 0;
       else if(s[x] >= '0' && s[x] <='9')
       {
          val = val * 16 + s[x] - '0';
       }
       else if(s[x]>='A' && s[x] <='F')
       {
          val = val * 16 + s[x] - 'A' + 10;
       }
       else if(s[x]>='a' && s[x] <='f')
       {
          val = val * 16 + s[x] - 'a' + 10;
       }
       else return 0;

       x++;
    }
    return val;
}
main()
{
    int i,n;
printf ("Content type: text/plain\n\n");


n=0;
    char qs[2000] ="";
    char* pqs = qs;
    if(getenv("CONTENT-LENGTH")){
    n=atoi(getenv("CONTENT-LENGTH"));
    printf("content-length:%d\n",n);
    for (i=0; i<n;i++){
        //int is-eq=0;
        char c=getchar();
printf("%c", c);
        switch (c){
            case '&':
                *pqs='\n';
                pqs++;
                break;
            case '+':
                *pqs=' ';
                pqs++;
                break;
            case '%':{
                char s[3] = "";
                s[0]=getchar();
                s[1]=getchar();
                s[2]=0;
                *pqs=htoi(s);
                i+=2;
                pqs++;
                }
                break;
//case '=':
//c=':';
//is-eq=1;
//break;
            default:
            *pqs=c;
            pqs++;
        };
    }
    }
printf("%s\n",getenv("QUERY_STRING"));
printf("qs=%s\n",qs);

fflush(stdout);
}
