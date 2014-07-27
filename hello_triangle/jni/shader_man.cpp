#include "stdafx.h"

ShaderMan shaderMan;

#ifdef _MSC_VER
#include "shader_utils.h"
#include "path_utils.h"
static bool CreateProgram(const char* name, GLuint* id)
{
	*id = CompileProgramFromFiles(
		GetExecutableDirectory() + "/../../assets/shaders/" + name + ".vs",
		GetExecutableDirectory() + "/../../assets/shaders/" + name + ".fs");
	return *id != 0;
}
#else
static bool CreateProgram(const char* name, GLuint* id)
{
	jclass myview = jniEnv->FindClass(boundJavaClass);
	jmethodID method = jniEnv->GetStaticMethodID(myview, "createProgram", "(Ljava/lang/String;)I");
	if (method == 0) {
		return false;
	}

	*id = jniEnv->CallStaticIntMethod(myview, method, jniEnv->NewStringUTF(name));
	return true;
}
#endif

static void ApplyElements(GLuint program, const InputElement decl[], int nDecl)
{
	int stride = 0;
	for (int i = 0; i < nDecl; i++) {
		const InputElement& d = decl[i];
		stride += d.size * sizeof(float);
	}
	for (int i = 0; i < nDecl; i++) {
		const InputElement& d = decl[i];
		int h = glGetAttribLocation(program, d.name);
		glEnableVertexAttribArray(h);
		glVertexAttribPointer(h, d.size, GL_FLOAT, GL_FALSE, stride, (void*)d.offset);
	}
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

	CreateProgram(name, &effect.program);
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
	Effect& it = effects[id];
	glUseProgram(it.program);
	ApplyElements(it.program, it.elements, it.numElements);
}
