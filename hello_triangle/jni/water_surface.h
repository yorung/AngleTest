struct WaterVert
{
	Vec3 pos;
	Vec3 normal;
};

struct WaterRipple
{
	WaterRipple()
	{
		generatedTime = -10000;
	}
	double generatedTime;
	Vec2 centerPos;
};

class WaterSurface
{
	ShaderMan::SMID shaderId;
	int lines;
	void UpdateVert(std::vector<WaterVert>& vert);
	void Update();
	WaterRipple ripples[2];
	int ripplesNext;
	double elapsedTime;
	double lastTime;
	double nextTime;
	GLuint vbo, ibo;
	int nIndi;
public:
	WaterSurface();
	~WaterSurface();
	void Destroy();
	void Init();
	void Draw();
	void CreateRipple(Vec2 pos);
};

extern WaterSurface waterSurface;
