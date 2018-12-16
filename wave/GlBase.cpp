#include "stdafx.h"
#include "GlBase.h"
#include "gl/glext.h"
#include <fstream>
#include <memory>

#pragma comment(lib, "glext.lib")

GlBase::GlBase()
{
}


GlBase::~GlBase()
{
}




static std::shared_ptr<GLchar> LoadSrc(const std::string &path)
{
	std::ifstream f(path);
	if (!f)
	{
		return nullptr;
	}

	f.seekg(std::ios::end);
	std::streampos pos = f.tellg();
	if (pos == 0)
	{
		return nullptr;
	}
	std::shared_ptr<char> srcBuf(new char[(unsigned int)pos]);

	f.read(srcBuf.get(), pos);
	return srcBuf;
}

unsigned int GlBase::CompileShader(const std::string &path, int shader)
{
	std::shared_ptr<char> srcBuf = LoadSrc(path);
	if (!srcBuf)
	{
		return 0;
	}
	GLuint verShader = glCreateShader(shader);
	char *buf = srcBuf.get();
	glShaderSource(verShader, 1, &buf, 0);
	glCompileShader(verShader);
	return verShader;
}


unsigned int GlBase::CreateShaderProg(const std::string &vertex, const std::string &fragment)
{
	CompileShader(vertex, GL_VERTEX_SHADER) ;
	CompileShader(fragment, GL_FRAGMENT_SHADER);

	unsigned int prog = glCreateProgram();
	return true;
}