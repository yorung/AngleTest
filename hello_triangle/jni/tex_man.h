class TexMan
{
public:
	typedef GLuint TMID;
	TMID Create(const char *name);
	void Destroy();
private:
	typedef std::map<std::string, TMID> NameToId;
	NameToId nameToId;
};

extern TexMan texMan;
