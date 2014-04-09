#ifndef _TEXTHANDLER_H
#define _TEXTHANDLER_H

#include "glwrapper.h"

#include <string>
#include <list>
#include <vector>

enum class TextAlign {ALIGN_LEFT, ALIGN_RIGHT};

struct GraphicText;

class TextHandler
{
	struct TextFragment
	{
		std::string text;
		TextAlign align;
		float x,y;
		std::vector<float> vbocache;
	};
	std::list<TextFragment> textfragments;
	std::vector<float> vboglobalcache;
	std::vector<int> vboglobalcache_numvertices;
	GLuint vbo;

	friend struct GraphicText;

	bool dirty;
public:
	TextHandler():vbo(0),dirty(true)
	{}
	GraphicText createtextfragment(const std::string & text, TextAlign align, float x, float y);
	void deletetextfragment(GraphicText);

	GLuint getvbo();
	std::vector<float> & getvbo_vector();
	std::vector<int> & getvbo_numvertices();
	void updatevbo();
};

struct GraphicText
{
	decltype(TextHandler::textfragments.end()) fragiterator;
};


#endif // _TEXTHANDLER_H
