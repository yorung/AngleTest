enum ShaderFormat {
	SF_R32_FLOAT,
	SF_R32G32_FLOAT,
	SF_R32G32B32_FLOAT,
	SF_R32G32B32A32_FLOAT,
	SF_R8_UNORM,
	SF_R8G8_UNORM,
	SF_R8G8B8_UNORM,
	SF_R8G8B8A8_UNORM,
	SF_R8_UINT,
	SF_R8G8_UINT,
	SF_R8G8B8_UINT,
	SF_R8G8B8A8_UINT,
};

struct InputElement {
	int inputSlot;
	const char* name;
	ShaderFormat format;
	int offset;
};

class CInputElement : public InputElement {
public:
	CInputElement(int inputSlot, const char* name, ShaderFormat format, int offset) {
		this->name = name;
		this->format = format;
		this->inputSlot = inputSlot;
		this->offset = offset;
	}
};

class ShaderMan
{
public:
	typedef GLuint SMID;
	static const SMID INVALID_SMID = 0;
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
	void SetVertexBuffers(SMID id, int numBuffers, GLuint const *vertexBufferIds, const GLsizei* strides);
};

extern ShaderMan shaderMan;

