class TexMan
{
public:
	typedef GLuint TMID;
	static const TMID INVALID_TMID = 0;
	TMID Create(const char *name);
	TMID CreateWhiteTexture();
	TMID CreateDynamicTexture(const char* name, int w, int h);
	void Destroy();
	void Write(TMID id, const void* buf, int w, int h);
private:
	typedef std::map<std::string, TMID> NameToId;
	NameToId nameToId;
};

extern TexMan texMan;
