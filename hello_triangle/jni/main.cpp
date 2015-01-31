#include "stdafx.h"
#include "SampleApplication.h"

static void CreateRipple(float x, float y)
{
	Mat matP, matV;
	matrixMan.Get(MatrixMan::PROJ, matP);
	matrixMan.Get(MatrixMan::VIEW, matV);
	Vec3 r = transform(Vec3(x, y, 0), inv(matV * matP));
	waterSurface.CreateRipple(Vec2(r.x, r.y));
}

const int SCR_W = 600;
const int SCR_H = 600;
class MyAngleApp : public SampleApplication
{
public:
	MyAngleApp::MyAngleApp() : SampleApplication("TestAngle", SCR_W, SCR_H)
	{
	}
	virtual bool initialize()
	{
		GoMyDir();
		SetCurrentDirectoryA("..\\assets");
		app.Create();
		return true;
	}
	virtual void destroy()
	{
		app.Destroy();
	}
	virtual void draw()
    {
		static bool last;
		bool current = !!(GetKeyState(VK_LBUTTON) & 0x80);
		bool edge = current && !last;
		last = current;

		RECT rc;
		GetClientRect(getWindow()->getNativeWindow(), &rc);
		int w = rc.right - rc.left;
		int h = rc.bottom - rc.top;

		if (edge) {
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(GetForegroundWindow(), &pt);
			CreateRipple((float)pt.x / w * 2 - 1, (float)pt.y / h * -2 + 1);
		}
		app.Update(w, h, 0);
		app.Draw();
	}
};

int main(int argc, char **argv)
{
    MyAngleApp angleApp;
    return angleApp.run();
}
