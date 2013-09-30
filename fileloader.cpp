#ifdef __DUETTO__
#include <duetto/client.h>
#include <duetto/clientlib.h>

using namespace client;
#else
#include <fstream>
#include <iostream>
#endif
#include "fileloader.h"

#ifdef __DUETTO__
int FileLoader::todo;
#endif
std::function <void()> FileLoader::done;

void FileLoader::setcallback(std::function <void()>f)
{
	done = f;
}
#ifdef __DUETTO__
void FileLoader::recvonload(client::Event * e, const char * file)
{
	auto res = static_cast<client::XMLHttpRequest *>(e->get_target());
	auto contents = res->get_responseText();

	auto scriptTag = document.createElement("script");
	scriptTag->setAttribute("type", "customscript");
	scriptTag->setAttribute("id", file);

	scriptTag->appendChild(document.createTextNode(*contents));

	(document.getElementsByTagName("head"))->item(0)->appendChild(scriptTag);

	if(!(--todo))
		done();
}
#endif

void FileLoader::load(const std::vector<std::string> files)
{
	#ifdef __DUETTO__
	for(const auto & file: files)
	{
		auto xhr = new client::XMLHttpRequest();
		xhr->open("get", file.c_str(), true);
		todo ++;
		xhr->set_onload(client::Callback([=](client::Event * e){recvonload(e, file.c_str());}));
		xhr->send();
	}
	#endif
}

void FileLoader::go()
{
	#ifndef __DUETTO__
	done();
	#endif
}


#ifdef __DUETTO__
client::String * FileLoader::getfilecontent(std::string f)
{
	auto elem = document.getElementById(f.c_str());
	auto res = (elem->get_childNodes())->item(0)->get_nodeValue();
	//console.log(*res);
	return res;

}
#else
std::string FileLoader::getfilecontent(std::string f)
{
	std::ifstream t(f);
	if(!t)
	{
		std::cerr << "Error reading file "<< f <<std::endl;
		return "";
	}
	std::string str;
	t.seekg(0, std::ios::end);   
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	return str;
}
#endif
