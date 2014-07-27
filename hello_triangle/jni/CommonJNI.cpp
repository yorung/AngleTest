#include "stdafx.h"

//JNIEnv* jniEnv;
//const char* boundJavaClass = "common/pinotnoir/livewallpaper/CommonGLDrawer";
#define JNIEXPORT
#define JNICALL

extern "C" {

//JNIEXPORT void JNICALL Java_common_pinotnoir_livewallpaper_CommonJNI_init(JNIEnv* env, jobject obj)
void init()
{
//	jniEnv = env;
	glClearColor(0.0f, 0.2f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	waterSurface.Init();
//	jniEnv = nullptr;
}

//JNIEXPORT void JNICALL Java_common_pinotnoir_livewallpaper_CommonJNI_createRipple(JNIEnv* env, jobject obj, jfloat x, jfloat y)
void createRipple(float x, float y)
{
	Mat matP, matV;
	matrixMan.Get(MatrixMan::PROJ, matP);
	matrixMan.Get(MatrixMan::VIEW, matV);
	Vec3 r = transform(Vec3(x, y, 0), inv(matV * matP));
	waterSurface.CreateRipple(Vec2(r.x, r.y));
}

//JNIEXPORT void JNICALL Java_common_pinotnoir_livewallpaper_CommonJNI_destroy(JNIEnv* env, jobject obj)
void destroy()
{
//	jniEnv = env;
	texMan.Destroy();
	shaderMan.Destroy();
	waterSurface.Destroy();
//	jniEnv = nullptr;
}

//JNIEXPORT void JNICALL Java_common_pinotnoir_livewallpaper_CommonJNI_update(JNIEnv* env, jobject obj, jfloat aspect, jfloat offset)
void update(float aspect, float offset)
{
//	jniEnv = env;

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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	waterSurface.Draw();
//	jniEnv = nullptr;
}

}


#include "SampleApplication.h"
class MyApp : public SampleApplication
{
public:
	MyApp::MyApp() : SampleApplication("Bingmu", 720, 720)
	{
	}
	virtual bool initialize()
	{
		::init();
		return true;
	}
	virtual void destroy()
	{
		::destroy();
	}
	virtual void draw()
    {
		update(1, 0);
	}
};

int main(int argc, char **argv)
{
    MyApp app;
    return app.run();
}
