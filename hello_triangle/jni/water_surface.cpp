#include "stdafx.h"

WaterSurface waterSurface;

struct TexFiles
{
	const char *name;
	bool clamp;
};

TexFiles texFiles[] = {
	{ "rose.jpg", true },
	{ "autumn.jpg", true },
	{ "pangyo.jpg", true },
	{ "timeline.png", false },
	{ "delaymap.png", true },
	{ "sphere.jpg", true },
};

TexMan::TMID texId[dimof(texFiles)];


const int tileMax = 50;
const int vertMax = tileMax + 1;
const float pitch = 2.0f / tileMax;
const float repeat = 2;
const float halflife = 1.5f;
//const float heightUnit = 0.00375f;
const float heightUnit = 0.02f;
const float rippleSpeed = 0.7f;
const float loopTime = 20.0;

static Vec3 MakePos(int x, int z, float hmap[vertMax][vertMax])
{
	float height = hmap[std::max(0,std::min(tileMax, x))][std::max(0,std::min(tileMax, z))];
	return Vec3(((float)x - tileMax / 2) * pitch, height, ((float)z - tileMax / 2) * pitch);
}

void WaterSurface::CreateRipple(Vec2 pos)
{
	WaterRipple r;
	r.generatedTime = elapsedTime;
	r.centerPos = pos;
	ripples[ripplesNext++] = r;
	ripplesNext %= dimof(ripples);
}

void WaterSurface::UpdateVert(std::vector<WaterVert>& vert)
{
	struct RandWave {
		float degreePerSec;
		float xShift;
		float xMul;
		float timeMul;
		float strength;
	} static randWave[10];
	static bool t;
	if (!t) {
		t = true;
		for (int i = 0; i < dimof(randWave); i++) {
			RandWave& r = randWave[i];
			r.degreePerSec = Random() * 15 - 7.5;
			r.xShift = Random();
			r.xMul = 2 + pow(2, 1 + 3 * Random());
			r.timeMul = 2 + pow(2, 0.5 + 2 * Random());
			r.strength = 0.005f * pow(2, 0.1 + 0.3 * Random());
		}
	}
	float hmap[vertMax][vertMax];
	memset(hmap, 0, sizeof(hmap));
	for (int z = 0; z <= tileMax; z++) {
		for (int x = 0; x <= tileMax; x++) {
			Vec2 pos = Vec2((float)x, (float)z) / tileMax * 2 - Vec2(1, 1);
			for (int i = 0; i < dimof(ripples); i++) {
				const WaterRipple& r = ripples[i];
				float lifeTime = (float)(elapsedTime - r.generatedTime);
				float timeAfterArrived = lifeTime - length(r.centerPos - pos) / rippleSpeed;
				float h = timeAfterArrived > 0 ? (float)sin(timeAfterArrived * (M_PI * 2) * repeat) * heightUnit : 0;
				h *= std::min(1.0f, powf(0.5f, lifeTime / halflife));

				hmap[x][z] += h;
			}
#if 0
			for (int i = 0; i < dimof(randWave); i++) {
				const RandWave& r = randWave[i];
				float rad = (elapsedTime * r.degreePerSec) * XM_2PI / 180;
				Vec2 posRot = pos * cosf(rad) + Vec2(-pos.y, pos.x) * sinf(rad);
				hmap[x][z] += sinf(r.xShift + posRot.x * r.xMul + elapsedTime * r.timeMul) * r.strength;
			}
#endif
		}
	}

	for (int z = 0; z <= tileMax; z++) {
		for (int x = 0; x <= tileMax; x++) {
			WaterVert v;
			v.pos = MakePos(x, z, hmap);
			Vec3 v1 = MakePos(x, z - 1, hmap);
			Vec3 v2 = MakePos(x - 1, z, hmap);
			Vec3 v3 = MakePos(x + 1, z + 1, hmap);
			v.normal = cross(v2 - v1, v3 - v2);
			vert.push_back(v);
		}
	}
}

WaterSurface::WaterSurface()
{
	ripplesNext = 0;
}

WaterSurface::~WaterSurface()
{
	Destroy();
}

void WaterSurface::Destroy()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

void WaterSurface::Init()
{
	Destroy();

	lastTime = GetTime();

	std::vector<short> indi;
	std::vector<WaterVert> vert;
	UpdateVert(vert);

	for (int z = 0; z < tileMax; z++) {
		if (z != 0) {
			indi.push_back(z * vertMax);
		}
		indi.push_back(z * vertMax);
		for (int x = 0; x < tileMax; x++) {
			indi.push_back((z + 1) * vertMax + x);
			indi.push_back(z * vertMax + x + 1);
		}
		indi.push_back((z + 1) * vertMax + vertMax - 1);
		if (z != tileMax - 1) {
			indi.push_back((z + 1) * vertMax + vertMax - 1);
		}
	}

	lines = indi.size() / 2;
	nIndi = indi.size();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(WaterVert), &vert[0], GL_DYNAMIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ARRAY_BUFFER, ibo);
	glBufferData(GL_ARRAY_BUFFER, indi.size() * sizeof(short), &indi[0], GL_STATIC_DRAW);

	static const InputElement elements[] = {
		{ 0, "vPosition", SF_R32G32B32_FLOAT, 0 },
		{ 0, "vNormal", SF_R32G32B32_FLOAT, 12 },
	};
//	texId = texMan.Create("sphere.jpg");
	shaderId = shaderMan.Create("water", elements, dimof(elements));

	glActiveTexture(GL_TEXTURE0);
	for (int i = 0; i < dimof(texFiles); i++) {
		texId[i] = texMan.Create(texFiles[i].name);
	}
}

void WaterSurface::Update()
{
	double now = GetTime();
	elapsedTime += now - lastTime;
	lastTime = now;

	std::vector<WaterVert> vert;
	UpdateVert(vert);

	glBufferSubData(GL_ARRAY_BUFFER, 0, vert.size() * sizeof(WaterVert), &vert[0]);

	if (0) {
//	if (elapsedTime >= nextTime) {
		nextTime = elapsedTime + 0.5 + Random() * 2;
		CreateRipple(Vec2(Random(), Random()) * 4 - Vec2(2, 2));
	}
}

void WaterSurface::Draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

//	float rad = sinf(time * M_PI) * 0.5f * M_PI;
//	vertOld[0].normal = Vec3(0, 0, 1) * cosf(rad) + Vec3(1, 0, 0) * sinf(rad);
//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertOld), vertOld);

//	glDrawArrays(GL_TRIANGLE_STRIP, 0, dimof(vert));
	glDrawElements(GL_TRIANGLE_STRIP, nIndi, GL_UNSIGNED_SHORT, 0);
//	glDisableVertexAttribArray(mPositionHandle);

	Update();

	shaderMan.Apply(shaderId);

	glUniform1i(glGetUniformLocation(shaderId, "sampler0"), 0);
	glUniform1i(glGetUniformLocation(shaderId, "sampler1"), 1);
	glUniform1i(glGetUniformLocation(shaderId, "sampler2"), 2);
	glUniform1i(glGetUniformLocation(shaderId, "sampler3"), 3);
	glUniform1i(glGetUniformLocation(shaderId, "sampler4"), 4);
	glUniform1i(glGetUniformLocation(shaderId, "sampler5"), 5);
	double dummy;
	glUniform1f(glGetUniformLocation(shaderId, "time"), (float)modf(elapsedTime * (1.0f / loopTime), &dummy) * loopTime);

//	Mat m = scale(1.5f);
//	Mat m = q2m(Quat(Vec3(1,0,0), M_PI / 180 * time * 60));
	Mat matW = q2m(Quat(Vec3(1,0,0), (float)M_PI / 180 * -90));
	Mat matP, matV;
	matrixMan.Get(MatrixMan::PROJ, matP);
	matrixMan.Get(MatrixMan::VIEW, matV);
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "matW"), 1, GL_FALSE, &matW.m[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "matV"), 1, GL_FALSE, &matV.m[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "matP"), 1, GL_FALSE, &matP.m[0][0]);

	for (int i = 0; i < dimof(texFiles); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texId[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		if (texFiles[i].clamp) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
	}
}
