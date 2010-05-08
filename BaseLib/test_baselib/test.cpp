#include "../baselib.h"
#include <dlfcn.h>
typedef void (*PS) (char* msg) ;
int main(){
	void* h =dlopen("libBaselib.so", RTLD_NOW/*RTLD_LAZY*/);
	PS ps = (PS) dlsym(h, "put_str");
	ps("aa");
}
