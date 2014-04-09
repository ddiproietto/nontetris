#include "texthandler.h"

#include <algorithm>

static inline int chartofontindex (char c)
{
	if ('0' <= c && c <= '9')
		return c - '0';
	else if ('A' <= c && c <= 'Z')
		return c - 'A' + 10;
	else if ('a' <= c && c <= 'z')
		return c - 'a' + 10;
	else
		return -1;
}

template <class vec>
static inline int string_to_vbo_vector(const std::string & s, vec & coords, int rightx, int bottomy, TextAlign align)
{
	int index = 0;

	if (align == TextAlign::ALIGN_LEFT)
	{
		for (auto i = s.begin(); i != s.end(); ++i)
		{
			const auto & c = *i;

			int charindex = chartofontindex(c);
			
			if (charindex < 0)
				continue;

			coords.insert (coords.end(),{
				(rightx + index) * (2/20.0F) -1, (17 - bottomy) * (2/18.0F) -1,  (1+(8.0F*charindex))/512, 1,
				(rightx + index) * (2/20.0F) -1, (18 - bottomy) * (2/18.0F) -1,        (1+(8.0F*charindex))/512, 0,
				(rightx + 1 + index) * (2/20.0F) -1, (17 - bottomy) * (2/18.0F) -1,     (9+(8.0F*charindex))/512, 1,
				(rightx + 1 + index) * (2/20.0F) -1, (18 - bottomy) * (2/18.0F) -1,           (9+(8.0F*charindex))/512, 0
			});

			++index;
		}
	}
	else if (align == TextAlign::ALIGN_RIGHT)
	{
		for (auto i = s.rbegin(); i != s.rend(); ++i)
		{
			const auto & c = *i;

			int charindex = chartofontindex(c);
			
			if (charindex < 0)
				continue;

			coords.insert (coords.end(),{
				(rightx + 1 - index) * (2/20.0F) -1, (17 - bottomy) * (2/18.0F) -1,     (9+(8.0F*charindex))/512, 1,
				(rightx + 1 - index) * (2/20.0F) -1, (18 - bottomy) * (2/18.0F) -1,           (9+(8.0F*charindex))/512, 0,
				(rightx - index) * (2/20.0F) -1, (17 - bottomy) * (2/18.0F) -1,  (1+(8.0F*charindex))/512, 1,
				(rightx - index) * (2/20.0F) -1, (18 - bottomy) * (2/18.0F) -1,        (1+(8.0F*charindex))/512, 0
			});
			++index;
		}
	}

	return index;
}

GraphicText TextHandler::createtextfragment(const std::string & text, TextAlign align, float x, float y)
{
	GraphicText ret;
	TextFragment tf;
	
	tf.text = text;
	tf.align = align;
	tf.x = x;
	tf.y = y;

	string_to_vbo_vector(tf.text, tf.vbocache, tf.x, tf.y, tf.align);

	textfragments.push_back(std::move(tf));

	dirty = true;

	ret.fragiterator = std::prev(textfragments.end());
	return ret;
}

void TextHandler::deletetextfragment(GraphicText gt)
{
	textfragments.erase(gt.fragiterator);
	dirty = true;
}

void TextHandler::updatevbo()
{
	vboglobalcache.clear();
	vboglobalcache_numvertices.clear();

	for (const auto & tf: textfragments)
	{
		vboglobalcache.insert(vboglobalcache.end(),tf.vbocache.begin(), tf.vbocache.end());
		vboglobalcache_numvertices.push_back(tf.vbocache.size()/4);
	}

	if(glIsBuffer(vbo))
		glDeleteBuffers(1, &vbo);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vboglobalcache.size()*sizeof(float), vboglobalcache.data(), GL_STATIC_DRAW);

	dirty = false;
}

GLuint TextHandler::getvbo()
{
	if (dirty)
		updatevbo();
	return vbo;
}

std::vector<float> & TextHandler::getvbo_vector()
{
	if (dirty)
		updatevbo();
	return vboglobalcache;
}

std::vector<int> & TextHandler::getvbo_numvertices()
{
	if (dirty)
		updatevbo();
	return vboglobalcache_numvertices;
}
