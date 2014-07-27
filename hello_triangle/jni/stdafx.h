#ifdef _MSC_VER
#include <Windows.h>
#else
#include <jni.h>
#endif

#include <gles2/gl2.h>
#include <gles2/gl2ext.h>

#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <deque>

#include <math.h>
#include <assert.h>
#include <time.h>

#include "af_math.h"
#include "helper.h"
#include "tex_man.h"
#include "shader_man.h"
#include "matrix_man.h"
#include "water_surface.h"

//#define nullptr NULL;

//extern JNIEnv* jniEnv;
//extern const char* boundJavaClass;

