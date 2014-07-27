#include "stdafx.h"

TexMan texMan;

#if 0
#include "shader_utils.h"
#include "path_utils.h"
#include "tga_utils.h"
static bool LoadTextureTGA(const char* name, GLuint* tex)
{
    TGAImage img;
    if (!LoadTGAImageFromFile(GetExecutableDirectory() + "/../../assets/" + name, &img))
    {
        return false;
    }

	*tex = LoadTextureFromTGAImage(img);
	return *tex != 0;
}
#endif

#ifdef _MSC_VER
#include "shader_utils.h"
#include "path_utils.h"
using std::min;
using std::max;
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
static bool LoadTexture(const char* name, GLuint* tex)
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	std::string path = GetExecutableDirectory() + "/../../assets/" + name;
	WCHAR wc[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, path.c_str(), -1, wc, dimof(wc));
	Gdiplus::Bitmap* image = new Gdiplus::Bitmap(wc);


	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	std::vector<DWORD> col;
	for(int y = 0; y < (int)image->GetHeight(); y++) {
		for(int x = 0; x < (int)image->GetWidth(); x++) {
			Gdiplus::Color c;
            image->GetPixel(x, y, &c);
			col.push_back((c.GetA() << 24) + (c.GetB() << 16) + (c.GetG() << 8) + c.GetR());
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->GetWidth(), image->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, &col[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	delete image;
	Gdiplus::GdiplusShutdown(gdiplusToken);
	*tex = texture;
	return *tex != 0;
}
#endif

#ifndef _MSC_VER
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
