struct InputElement {
	const char* name;
	int size;
	int offset;
};

class ShaderMan
{
public:
	typedef GLuint SMID;
private:
	struct Effect
	{
		SMID program;
		const InputElement *elements;
		int numElements;
	};
	typedef std::map<std::string, SMID> NameToId;
	NameToId nameToId;
	typedef std::map<SMID, Effect> Effects;
	Effects effects;
public:
	SMID Create(const char *name, const InputElement elements[], int numElements);
	void Destroy();
	void Reload();
	void Apply(SMID id);
};

extern ShaderMan shaderMan;

