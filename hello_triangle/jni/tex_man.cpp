#include "stdafx.h"

TexMan texMan;

#ifdef _MSC_VER
#include "shader_utils.h"
#include "path_utils.h"
#include "tga_utils.h"
static bool LoadTexture(const char* name, GLuint* tex)
{
    TGAImage img;
    if (!LoadTGAImageFromFile(GetExecutableDirectory() + "/../../assets/" + name, &img))
    {
        return false;
    }

	*tex = LoadTextureFromTGAImage(img);
	return *tex != 0;
}
#else
static bool LoadTexture(const char* name, GLuint* tex)
{
	jclass myview = jniEnv->FindClass(boundJavaClass);
	jmethodID method = method = jniEnv->GetStaticMethodID(myview, "loadTexture", "(Ljava/lang/String;)I");
	if (method == 0) {
		return false;
	}

	*tex = jniEnv->CallStaticIntMethod(myview, method, jniEnv->NewStringUTF(name));
	return true;
}
#endif

TexMan::TMID TexMan::Create(const char *name)
{
	NameToId::iterator it = nameToId.find(name);
	if (it != nameToId.end())
	{
		return it->second;
	}

	GLuint tex;
	if (!LoadTexture(name, &tex)) {
		return -1;
	}
	return nameToId[name] = tex;
}

void TexMan::Destroy()
{
	for (NameToId::iterator it = nameToId.begin(); it != nameToId.end(); ++it)
	{
		GLuint id[1] = {it->second};
		glDeleteTextures(1, id);
	}
	nameToId.clear();
}
