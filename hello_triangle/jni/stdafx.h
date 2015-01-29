#ifdef _MSC_VER
#include <Windows.h>
#define JNIEXPORT
#define JNICALL
typedef void JNIEnv;
typedef void* jobject;
typedef float jfloat;
#else
#include <jni.h>
#define nullptr NULL;
extern JNIEnv* jniEnv;
extern const char* boundJavaClass;
#endif

#include <gles2/gl2.h>
#include <gles2/gl2ext.h>

#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <deque>

#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>
#include <time.h>

#include "af_math.h"
#include "helper.h"
#include "helper_text.h"
#include "tex_man.h"
#include "shader_man.h"
#include "matrix_man.h"
#include "water_surface.h"
