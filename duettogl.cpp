#include <duetto/client.h>
#include <duetto/clientlib.h>
#include <duetto/webgl.h>

#include "duettogl.h"

using namespace client;

void myteximage2Dnull(WebGLRenderingContext* gl, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type);
void myglbindnullframebuffer(WebGLRenderingContext* gl);
WebGLRenderingContext* gl;



WebGLShader * gaShader[200];
unsigned int gaiShader = 0;

WebGLProgram * gaProgram[200];
unsigned int gaiProgram = 0;

WebGLUniformLocation * gaUniformLocation[200];
unsigned int gaiUniformLocation = 0;

WebGLTexture * gaTexture[200];
unsigned int gaiTexture = 0; //TODO: 0 should be reserved

WebGLFramebuffer * gaFramebuffer[200];
unsigned int gaiFramebuffer = 1; //0 is reserved

WebGLBuffer * gaBuffer[200];
unsigned int gaiBuffer = 0;


void duettoGLInit(int width, int height)
{
	auto canvas = static_cast<client::HTMLCanvasElement*>(client::document.getElementById("glcanvas"));
	gl = reinterpret_cast<client::WebGLRenderingContext*>(canvas->getContext("experimental-webgl"));
}

GLuint glCreateShader(GLenum type)
{
	gaShader[gaiShader] = gl->createShader(type);
	return gaiShader ++;
}

void glShaderSource(GLuint shader,  GLsizei count,  const GLchar * const * string,  const GLint *length)
{
	//LIMITATION: count must be one, length must be NULL
	return gl->shaderSource(gaShader[shader], string[0]);
}

void glShaderSource(GLuint shader,  GLsizei count,  const String * const * string,  const GLint *length)
{
	//LIMITATION: count must be one, length must be NULL
	return gl->shaderSource(gaShader[shader], *string[0]);
}

void glCompileShader(GLuint shader)
{
	gl->compileShader(gaShader[shader]);
}

GLuint glCreateProgram (void)
{
	gaProgram[gaiProgram] = gl->createProgram();
	return gaiProgram ++;
}

void glAttachShader (GLuint program, GLuint shader)
{
	gl->attachShader(gaProgram[program], gaShader[shader]);
}

void glLinkProgram (GLuint program)
{
	gl->linkProgram(gaProgram[program]);
}
void glUseProgram (GLuint program)
{
	gl->useProgram(gaProgram[program]);
}
GL_APICALL GLint        GL_APIENTRY glGetAttribLocation (GLuint program, const GLchar* name)
{
	return gl->getAttribLocation(gaProgram[program], name);
}
GL_APICALL void         GL_APIENTRY glEnableVertexAttribArray (GLuint index)
{
	gl->enableVertexAttribArray(index);
}
GL_APICALL GLint        GL_APIENTRY glGetUniformLocation (GLuint program, const GLchar* name)
{
	gaUniformLocation[gaiUniformLocation] = gl->getUniformLocation(gaProgram[program], name);
	return gaiUniformLocation++;
}
GL_APICALL void         GL_APIENTRY glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	//XXX: check count, ugly const cast
	gl->uniformMatrix4fv(gaUniformLocation[location], transpose, Float32Array(const_cast<float*>(value)));
}
GL_APICALL void         GL_APIENTRY glGenTextures (GLsizei n, GLuint* textures)
{
	for(GLsizei i = 0; i < n; i++)
	{
		gaTexture[gaiTexture] = gl->createTexture();
		textures[i] = gaiTexture++;
	}
}
GL_APICALL void         GL_APIENTRY glBindTexture (GLenum target, GLuint texture)
{
	gl->bindTexture(target, gaTexture[texture]);
}

GL_APICALL void         GL_APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param)
{
	gl->texParameteri(target, pname, param);
}

GL_APICALL void         GL_APIENTRY glDisable (GLenum cap)
{
	gl->disable(cap);
}
GL_APICALL void         GL_APIENTRY glGenBuffers (GLsizei n, GLuint* buffers)
{
	for(GLsizei i = 0; i < n; i++)
	{
		gaBuffer[gaiBuffer] = gl->createBuffer();
		buffers[i] = gaiBuffer++;
	}
	
}
GL_APICALL void         GL_APIENTRY glBufferData (GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
	//XXX:check size?
	//TODO assume float
	gl->bufferData(target, Float32Array(const_cast<float*>(static_cast<const float *>(data))),usage);
}

GL_APICALL void         GL_APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels)
{
	if(pixels == NULL)
		gl->texImage2D(target, level, internalformat, width, height, border, format, type, NULL);
	else
		gl->texImage2D(target, level, internalformat, width, height, border, format, type, (ArrayBufferView *)pixels);
}

GL_APICALL void         GL_APIENTRY glGenFramebuffers (GLsizei n, GLuint* framebuffers)
{
	for(GLsizei i = 0; i < n; i++)
	{
		gaFramebuffer[gaiFramebuffer] = gl->createFramebuffer();
		framebuffers[i] = gaiFramebuffer++;
	}
}

GL_APICALL void         GL_APIENTRY glBindFramebuffer (GLenum target, GLuint framebuffer)
{
	if( framebuffer == 0)
		gl->bindFramebuffer(target, NULL);
	else
		gl->bindFramebuffer(target, gaFramebuffer[framebuffer]);
}

GL_APICALL void         GL_APIENTRY glFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	gl->framebufferTexture2D(target, attachment, textarget, gaTexture[texture], level);
}
GL_APICALL void         GL_APIENTRY glBindBuffer (GLenum target, GLuint buffer)
{
	gl->bindBuffer(target, gaBuffer[buffer]);
}
GL_APICALL void         GL_APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
	gl->viewport(x, y, width, height);
}
GL_APICALL void         GL_APIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	gl->clearColor(red, green, blue, alpha);
}
GL_APICALL void         GL_APIENTRY glClear (GLbitfield mask)
{
	gl->clear(mask);
}
GL_APICALL void         GL_APIENTRY glUniform4fv (GLint location, GLsizei count, const GLfloat* v)
{
	//TODO:check size float32array
	gl->uniform4fv(gaUniformLocation[location], Float32Array(const_cast<float *>(v)));
}
GL_APICALL void         GL_APIENTRY glVertexAttribPointer (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLsizei ptr)
{
    gl->vertexAttribPointer(indx, size, type, normalized, stride, ptr);
}
GL_APICALL void         GL_APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count)
{
	gl->drawArrays(mode, first, count);
}
GL_APICALL void         GL_APIENTRY glDisableVertexAttribArray (GLuint index)
{
	gl->disableVertexAttribArray(index);
}
GL_APICALL void         GL_APIENTRY glGenerateMipmap (GLenum target)
{
	gl->generateMipmap(target);
}
