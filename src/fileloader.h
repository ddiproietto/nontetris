/*****************************************************************************

     Copyright (C) 2013  Daniele Di Proietto <d.diproietto@sssup.it>
     
     This file is part of nontetris.
     
     nontetris is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.
     
     nontetris is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
     
     You should have received a copy of the GNU General Public License
     along with nontetris.  If not, see <http://www.gnu.org/licenses/>.

*****************************************************************************/
#ifndef _FILELOADER_H
#define _FILELOADER_H

#ifdef __DUETTO__
#include <duetto/client.h>
#include <duetto/clientlib.h>
#else
#include <fstream>
#include <iostream>
#endif

#include <functional>
#include <vector>
#include <string>

class FileLoader
{
	#ifdef __DUETTO__
	int todo;
	#endif
	std::function <void()> done;
public:
	template<typename T>
	void load(T files, const std::function<void()> &f)
	{
		done = f;
		#ifdef __DUETTO__
		for(const auto & file: files)
		{
			auto xhr = new client::XMLHttpRequest();
			xhr->open("get", file, true);
			todo ++;
			xhr->set_onload(client::Callback([this, file](client::Event * e){
				auto res = static_cast<client::XMLHttpRequest *>(e->get_target());
				auto contents = res->get_responseText();

				auto scriptTag = client::document.createElement("script");
				scriptTag->setAttribute("type", "customscript");
				scriptTag->setAttribute("id", file);

				scriptTag->appendChild(client::document.createTextNode(*contents));

				(client::document.getElementsByTagName("head"))->item(0)->appendChild(scriptTag);

				if(!(--todo))
					done();
					;
			}));
			xhr->send();
		}
		#else
		done();
		#endif
	}

	#ifdef __DUETTO__
	client::String * getfilecontent(std::string f)
	{
		auto elem = client::document.getElementById(f.c_str());
		auto res = (elem->get_childNodes())->item(0)->get_nodeValue();
		//console.log(*res);
		return res;

	}
	#else
	static std::string getfilecontent(std::string f)
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
};

#endif //_FILELOADER_H
