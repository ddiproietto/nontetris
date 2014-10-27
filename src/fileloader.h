/*****************************************************************************

     Copyright (C) 2013, 2014  Daniele Di Proietto <d.diproietto@sssup.it>

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

#ifdef __CHEERP__
#include <cheerp/client.h>
#include <cheerp/clientlib.h>
#else
#include <fstream>
#include <iostream>
#endif

#include <functional>
#include <string>
#include <map>

#include <type_traits>

/* FileLoader
 *
 * This class implements a very simple interface to load small text files,
 * e.g. shaders. Usage:
 *
 * FileLoader fileloader;
 * const char *filelist = ["shader.frag", "shader.vert"];
 *
 * fileloader.load(filelist, [&](){
 * 	// The files have been loaded. Now we can continue with execution.
 * 	compileshaders(fileloader);
 * 	//...
 * });
 *
 * Later, to use the file contents:
 *
 * auto shaderfrag_source = fileloader.getfilecontent("shader.frag");
 *
 * The type of 'shaderfrag_source' will be std::string, or client::String *
 * if using cheerp.  This is done because conversions between JS and C++
 * strings are expensive and should be avoided.
 *
 * When compiling for cheerp, XMLHttpRequest will be used. The files should be
 * available via HTTP.
 *
 * When compiling not for cheerp, load() will not have any effect (except
 * calling the callback).  The file will be read from disk when requested
 * with getfilecontent().
 */

class FileLoader
{
#ifdef __CHEERP__
	/* Counts the files that still need to be loaded. When it reaches 0, the callback can be triggered. */
	int todo;
	/* Stores the file contents. It is indexed by the file name. */
	std::map <std::string, client::String *> filecontentmap;
#endif
	/* Called when the files have been loaded */
	std::function <void()> done;
public:
	template<typename T>
	void load(T files, const std::function<void()> &f)
	{
		done = f;
#ifdef __CHEERP__
		for (const auto &file: files) {
			auto xhr = new client::XMLHttpRequest();
			xhr->open("get", file, true);
			todo++;
			xhr->set_onload(cheerp::Callback([this, file](client::Event *e){
				auto res = static_cast<client::XMLHttpRequest *>(e->get_target());
				auto contents = res->get_responseText();

				filecontentmap.insert(make_pair(std::string(file), contents));

				if (!(--todo)) {
					done();
				}
			}));
			xhr->send();
		}
#else
		done();
#endif
	}

#ifdef __CHEERP__
	const client::String *getfilecontent(std::string f) const
	{
		/* Old version of cheerp have a bug with map::find() const */
		auto & fcm = const_cast<std::remove_const<decltype(filecontentmap)>::type &>(filecontentmap);
		auto it = fcm.find(f);

		if (it == fcm.end()) {
			return NULL;
		}

		return it->second;
	}
#else
	static std::string getfilecontent(std::string f)
	{
		std::ifstream t(f);
		if(!t) {
			std::cerr << "Error reading file " << f << std::endl;
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

#endif /* _FILELOADER_H */
