#pragma once
#include <gl/GL.h>
#include <string>

class GlBase
{
public:
	GlBase();
	~GlBase();
	unsigned int CompileShader(const std::string &path, int shader);
	unsigned int CreateShaderProg(const std::string &vertex, const std::string &fragment);
	
private:
	
};

