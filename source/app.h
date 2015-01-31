class App {
public:
	void Create();
	void Update(int w, int h, float offset);
	void Draw();
	void CreateRipple(float x, float y);
	void Destroy();
};

extern App app;
