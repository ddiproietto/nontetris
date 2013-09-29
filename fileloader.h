#ifndef _FILELOADER_H
#define _FILELOADER_H

#include <functional>
#include <vector>
#include <string>

#ifdef __DUETTO__
#include <map>
#endif


class FileLoader
{
	//TODO: shouldn't be static, but i need it for lambda callback
	#ifdef __DUETTO__
	static int todo;
	#endif
	static std::function <void()> done;
public:
	static void setcallback(std::function<void()> f);
	static void load(const std::vector<std::string>);
	static void go();
	#ifdef __DUETTO__
	static void recvonload(client::Event * e);
	static client::String * getfilecontent(std::string);
	#else
	static std::string getfilecontent(std::string);
	#endif

};

#endif //_FILELOADER_H
