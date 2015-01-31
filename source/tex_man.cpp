#include "stdafx.h"

TexMan texMan;

#ifndef _MSC_VER
static GLuint LoadTextureViaOS(const char* name)
{
	jclass myview = jniEnv->FindClass(boundJavaClass);
	jmethodID method = method = jniEnv->GetStaticMethodID(myview, "loadTexture", "(Ljava/lang/String;)I");
	if (method == 0) {
		return 0;
	}
	return jniEnv->CallStaticIntMethod(myview, method, jniEnv->NewStringUTF(name));
}
#endif

#ifdef _MSC_VER
using std::min;
using std::max;
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

static GLuint LoadTextureViaOS(const char* name)
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
	WCHAR wc[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, name, -1, wc, dimof(wc));
	Gdiplus::Bitmap* image = new Gdiplus::Bitmap(wc);

	int w = (int)image->GetWidth();
	int h = (int)image->GetHeight();
	Gdiplus::Rect rc(0, 0, w, h);

	Gdiplus::BitmapData* bitmapData = new Gdiplus::BitmapData;
	image->LockBits(&rc, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, bitmapData);

	std::vector<uint32_t> col;
	col.resize(w * h);
	for (int y = 0; y < h; y++) {
		memcpy(&col[y * w], (char*)bitmapData->Scan0 + bitmapData->Stride * y, w * 4);
		for (int x = 0; x < w; x++) {
			uint32_t& c = col[y * w + x];
			c = (c & 0xff00ff00) | ((c & 0xff) << 16) | ((c & 0xff0000) >> 16);
		}
	}
	image->UnlockBits(bitmapData);
	delete bitmapData;
	delete image;
	Gdiplus::GdiplusShutdown(gdiplusToken);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &col[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}
#endif

struct DDSHeader {
	uint32_t h3[3];
	uint32_t h, w;
	uint32_t h2[2];
	uint32_t mipCnt;
	uint32_t h13[13];
	uint32_t fourcc, bitsPerPixel, rMask, gMask, bMask, aMask;
};

static void bitScanForward(uint32_t* result, uint32_t mask)
{
	//	DWORD dwd;
	//	_BitScanForward(&dwd, mask);
	//	*result = dwd;

	for (int i = 0; i < 32; i++) {
		if (mask & (1 << i)) {
			*result = i;
			return;
		}
	}
	*result = 0;
}

static GLuint CreateTextureFromRowDDS(const void* img, int size)
{
	const DDSHeader* hdr = (DDSHeader*)img;
	int w = (int)hdr->w;
	int h = (int)hdr->h;
	const uint32_t* im = (uint32_t*)img + 128 / 4;
	std::vector<uint32_t> col;
	uint32_t rShift, gShift, bShift, aShift;
	bitScanForward(&rShift, hdr->rMask);
	bitScanForward(&gShift, hdr->gMask);
	bitScanForward(&bShift, hdr->bMask);
	bitScanForward(&aShift, hdr->aMask);
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			uint32_t c = *im++;
			col.push_back(
				((hdr->aMask & c) >> aShift << 24) +
				((hdr->bMask & c) >> bShift << 16) +
				((hdr->gMask & c) >> gShift << 8) +
				((hdr->rMask & c) >> rShift));
		}
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &col[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

static GLuint LoadDDSTexture(const char* name)
{
	int size;
	GLuint texture = 0;
	void* img = LoadFile(name, &size);
	if (!img) {
		aflog("LoadDDSTexture failed! %s", name);
		return 0;
	}
	const DDSHeader* hdr = (DDSHeader*)img;

	GLenum format;
	int blockSize = 16;
	switch (hdr->fourcc) {
	case 0x31545844: //'1TXD':
		format = 0x83F1;	// GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		blockSize = 8;
		break;
		//	case 0x33545844; //'3TXD':
		//		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		//		break;
		//	case 0x35545844; //'5TXD':
		//		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		//		break;
	default:
		texture = CreateTextureFromRowDDS(img, size);
		goto END;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	{
		int texSize = blockSize * ((hdr->w + 3) / 4) * ((hdr->h + 3) / 4);
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, hdr->w, hdr->h, 0, texSize, (char*)img + 128);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

END:
	free(img);
	return texture;
}

static GLuint LoadTexture(const char* name)
{
	int len = strlen(name);
	if (len > 4 && !stricmp(name + len - 4, ".dds")) {
		return LoadDDSTexture(name);
	} else {
		return LoadTextureViaOS(name);
	}
}

static GLuint CreateWhiteTexture()
{
	uint32_t col = 0xffffffff;
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &col);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

static GLuint CreateDynamicTexture(int w, int h)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

TexMan::TMID TexMan::CreateDynamicTexture(const char* name, int w, int h)
{
	auto it = nameToId.find(name);
	if (it != nameToId.end())
	{
		return it->second;
	}
	return nameToId[name] = ::CreateDynamicTexture(w, h);
}

TexMan::TMID TexMan::Create(const char *name)
{
	NameToId::iterator it = nameToId.find(name);
	if (it != nameToId.end())
	{
		return it->second;
	}
	return nameToId[name] = LoadTexture(name);
}

TexMan::TMID TexMan::CreateWhiteTexture()
{
	const std::string name = "$WHITE";
	NameToId::iterator it = nameToId.find(name);
	if (it != nameToId.end())
	{
		return it->second;
	}
	return nameToId[name] = ::CreateWhiteTexture();
}

void TexMan::Destroy()
{
	for (NameToId::iterator it = nameToId.begin(); it != nameToId.end(); ++it)
	{
		GLuint id[1] = { it->second };
		glDeleteTextures(1, id);
	}
	nameToId.clear();
}

void TexMan::Write(TMID id, const void* buf, int w, int h)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		0,
		0,
		w,
		h,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		buf
		);
	glBindTexture(GL_TEXTURE_2D, 0);
}
