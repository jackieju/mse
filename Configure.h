#ifndef __CONFIGURE_H
#define __CONFIGURE_H 
#include <map>
#include <string>

class CConfigure{
private:
	std::map<std::string, std::string> _map;
	
public:
	std::string get(std::string p){
		return _map[p];
	}
	
	std::string set(std::string p, std::string v){
		return _map[p] = v;
	}
	
	std::map<std::string, std::string>* map(){
		return &_map;
	}
};



#endif