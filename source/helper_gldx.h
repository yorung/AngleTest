typedef unsigned short AFIndex;

#ifdef GL_TRUE
#define AFIndexTypeToDevice GL_UNSIGNED_SHORT
typedef GLuint AFBufObj;
#endif

#ifdef __d3d11_h__
#define AFIndexTypeToDevice DXGI_FORMAT_R16_UINT
typedef ID3D11Buffer* AFBufObj;
#endif

#ifdef USE_FAKE_SAMPLER
#define glGenSamplers(a,b)
#define glSamplerParameteri(a,b,c)
#define glBindSampler(a,b)
#define glDeleteSamplers(a,b)
#endif

#ifdef GL_TRUE
inline void afSafeDeleteBuffer(GLuint& b)
{
	if (b != 0) {
		glDeleteBuffers(1, &b);
		b = 0;
	}
}
inline void afSafeDeleteSampler(GLuint& s)
{
	if (s != 0) {
		glDeleteSamplers(1, &s);
		s = 0;
	}

}
#endif

#ifdef __d3d11_h__
inline void afSafeDeleteBuffer(AFBufObj& b)
{
	SAFE_RELEASE(b);
}
#endif

AFBufObj afCreateIndexBuffer(const AFIndex* indi, int numIndi);
AFBufObj afCreateQuadListIndexBuffer(int numQuads);
AFBufObj afCreateVertexBuffer(int size, const void* buf);
AFBufObj afCreateDynamicVertexBuffer(int size);
void afWriteBuffer(AFBufObj bo, const void* buf, int size);
void afDrawIndexedTriangleList(AFBufObj ibo, int count, int start = 0);
