#include "stdafx.h"

ShaderMan shaderMan;

static GLuint CompileShader(int type, const char *fileName)
{
	GLuint shader = glCreateShader(type);

	void* img = LoadFile(fileName);
	glShaderSource(shader, 1, (const char**)&img, NULL);
	glCompileShader(shader);
	free(img);

	int result = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		GLchar* buf = new GLchar[len];
		int dummy;
		glGetShaderInfoLog(shader, len, &dummy, buf);
		aflog("result=%d (%s)%s", result, fileName, buf);
		delete buf;
		glDeleteShader(shader);
		shader = 0;
	}
	return shader;
}

static GLuint CreateProgram(const char* name)
{
	char buf[256];
	snprintf(buf, dimof(buf), "shaders/%s.vs", name);
	GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, buf);
	if (!vertexShader) {
		return 0;
	}
	snprintf(buf, dimof(buf), "shaders/%s.fs", name);
	GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, buf);
	if (!fragmentShader) {
		return 0;
	}
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glDeleteShader(vertexShader);
	glAttachShader(program, fragmentShader);
	glDeleteShader(fragmentShader);
	glLinkProgram(program);

	GLint status = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		GLchar* buf = new GLchar[len];
		int dummy;
		glGetProgramInfoLog(program, len, &dummy, buf);
		aflog("glLinkProgram failed!=%d (%s)%s", status, name, buf);
		delete buf;
		glDeleteProgram(program);
		program = 0;
	}
	return program;
}

void ShaderMan::SetVertexBuffers(SMID id, int numBuffers, GLuint const *vertexBufferIds, const GLsizei* strides)
{
	const Effect& it = effects[id];
	for (int i = 0; i < it.numElements; i++) {
		const InputElement& d = it.elements[i];
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIds[d.inputSlot]);
		int h = glGetAttribLocation(it.program, d.name);
		glEnableVertexAttribArray(h);
		switch (d.format) {
		case SF_R32_FLOAT:
		case SF_R32G32_FLOAT:
		case SF_R32G32B32_FLOAT:
		case SF_R32G32B32A32_FLOAT:
			glVertexAttribPointer(h, d.format - SF_R32_FLOAT + 1, GL_FLOAT, GL_FALSE, strides[d.inputSlot], (void*)d.offset);
			break;
		case SF_R8_UNORM:
		case SF_R8G8_UNORM:
		case SF_R8G8B8_UNORM:
		case SF_R8G8B8A8_UNORM:
			glVertexAttribPointer(h, d.format - SF_R8_UNORM + 1, GL_UNSIGNED_BYTE, GL_TRUE, strides[d.inputSlot], (void*)d.offset);
			break;
		case SF_R8_UINT:
		case SF_R8G8_UINT:
		case SF_R8G8B8_UINT:
		case SF_R8G8B8A8_UINT:
			glVertexAttribPointer(h, d.format - SF_R8_UINT + 1, GL_UNSIGNED_BYTE, GL_FALSE, strides[d.inputSlot], (void*)d.offset);
			break;
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

ShaderMan::SMID ShaderMan::Create(const char *name, const InputElement elements[], int numElements)
{
	NameToId::iterator it = nameToId.find(name);
	if (it != nameToId.end())
	{
		return it->second;
	}

	Effect effect;
	memset(&effect, 0, sizeof(effect));

	effect.program = CreateProgram(name);
	effect.elements = elements;
	effect.numElements = numElements;

	effects[effect.program] = effect;
	return nameToId[name] = effect.program;
}

void ShaderMan::Destroy()
{
	for (Effects::iterator it = effects.begin(); it != effects.end(); it++)
	{
		glDeleteProgram(it->second.program);
	}
	effects.clear();
	nameToId.clear();
}

void ShaderMan::Apply(SMID id)
{
	const Effect& it = effects[id];
	glUseProgram(it.program);
}
