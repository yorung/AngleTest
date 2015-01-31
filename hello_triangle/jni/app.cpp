#include "stdafx.h"

App app;

void App::Draw()
{
	glClearColor(1, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	waterSurface.Draw();
}

void App::Create()
{
	glClearColor(0.0f, 0.2f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	waterSurface.Init();
}

void App::CreateRipple(float x, float y)
{
	Mat matP, matV;
	matrixMan.Get(MatrixMan::PROJ, matP);
	matrixMan.Get(MatrixMan::VIEW, matV);
	Vec3 r = transform(Vec3(x, y, 0), inv(matV * matP));
	waterSurface.CreateRipple(Vec2(r.x, r.y));
}

void App::Destroy()
{
	texMan.Destroy();
	shaderMan.Destroy();
	waterSurface.Destroy();
}

void App::Update(int w, int h, float offset)
{
	float aspect = (float)h / w;
	glViewport(0, 0, w, h);
	if (aspect < 1) {
		matrixMan.Set(MatrixMan::VIEW, fastInv(translate(0, 0.5f * (1 - aspect), 0)));
		matrixMan.Set(MatrixMan::PROJ, Mat(
			1, 0, 0, 0,
			0, 1 / aspect, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1));
	} else {
		matrixMan.Set(MatrixMan::VIEW, fastInv(translate(offset * (1 - 1 / aspect), 0, 0)));
		matrixMan.Set(MatrixMan::PROJ, Mat(
			aspect, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1));
	}
	waterSurface.Update(w, h);
}
