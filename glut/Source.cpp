#include "LibBitmap.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "freeglut/include/GL/glut.h"
#include "freeglut/include/GL/freeglut.h"
#include "freeglut/include/GL/glext.h"
#include "freeglut/include/GL/wglext.h"
#include <string.h>
#include <gl/GL.h>

#pragma comment(lib, "glew32.lib")

// settings
const int SCR_WIDTH = 320, SCR_HEIGHT = 240;
GLuint base_prog;
GLuint quad_vbo;
GLuint g_texID;

void glAttachShaderSource(GLuint prog, GLenum type, const char * source)
{
	GLuint sh = glCreateShader(type);
	glShaderSource(sh, 1, &source, NULL);
	glCompileShader(sh);
	char buffer[4096];
	glGetShaderInfoLog(sh, sizeof(buffer), NULL, buffer);
	glAttachShader(prog, sh);
	glDeleteShader(sh);
}


void Init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEBUG_OUTPUT);
	base_prog = glCreateProgram();

	static const char quad_shader_vs[] =
		"#version 330 core\n"
		"\n"
		"layout (location = 0) in vec2 in_position;\n"
		"layout (location = 1) in vec2 in_tex_coord;\n"
		"\n"
		"out vec2 tex_coord;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"    gl_Position = vec4(in_position, 0.0, 1.0);\n"
		"    tex_coord = in_tex_coord;tex_coord.y = 1 - tex_coord.y;\n"
		"}\n"
		;

	static const char quad_shader_fs[] =
		"#version 330 core\n"
		"\n"
		"in vec2 tex_coord;\n"
		"\n"
		"layout (location = 0) out vec4 color;\n"
		"\n"
		"uniform sampler2D tex;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"    color = texture(tex,tex_coord);\n"
		"}\n"
		;
	glAttachShaderSource(base_prog, GL_VERTEX_SHADER, quad_shader_vs);
	glAttachShaderSource(base_prog, GL_FRAGMENT_SHADER, quad_shader_fs);
	glLinkProgram(base_prog);

	static const GLfloat quad_data[] =
	{
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
	};


	glGenBuffers(1, &quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(8 * sizeof(float)));
	glEnableVertexAttribArray(1);


	char buf[1024];
	glGetProgramInfoLog(base_prog, 1024, NULL, buf);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// texture
	glGenTextures(1, &g_texID);
	glBindTexture(GL_TEXTURE_2D, g_texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	LibBitmap lbmp;
	lbmp.ReadFile(R"(C:\Users\panpan\Pictures\test2.bmp)");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, lbmp.Width(), lbmp.Height(), 0, GL_BGR, GL_UNSIGNED_BYTE, lbmp.Data());
	
	glBindTexture(GL_TEXTURE_2D, 0);

	const int width = lbmp.Width();
	const int height = lbmp.Height();
	
	//test read back texture data
	glBindTexture(GL_TEXTURE_2D, g_texID);
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_texID, 0);

	LibBitmap bmpNew;
	bmpNew.CreateEmpty(width, height);
	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, bmpNew.Data());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);
	glBindTexture(GL_TEXTURE_2D, 0);

	bmpNew.WriteFile(R"(C:\Users\panpan\Pictures\testNew.bmp)");
}

void display(void)
{
	glClearColor(0.0, 255, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(base_prog);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_texID);
	glUniform1i(glGetUniformLocation(base_prog, "tex"), 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glutSwapBuffers();
}

void OnShutdown()
{
	glUseProgram(0);
	glDeleteProgram(base_prog);
	glDeleteTextures(1, &g_texID);
	glDeleteVertexArrays(1, &g_texID);
	glDeleteBuffers(1, &quad_vbo);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	glutCreateWindow("testTexCopy");

	if (glewInit() != GLEW_OK)
	{
		exit(EXIT_FAILURE);
	}

	Init();

	glutDisplayFunc(display);

	glutCloseFunc(OnShutdown);

	glutMainLoop();

	return 0;
}
