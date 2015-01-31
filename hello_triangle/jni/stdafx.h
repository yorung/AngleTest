#include <Windows.h>
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
#include <stdint.h>
#include <ctype.h>

#define USE_FAKE_SAMPLER
#include "af_math.h"
#include "helper.h"
#include "helper_gldx.h"
#include "helper_text.h"
#include "tex_man.h"
#include "shader_man.h"
#include "matrix_man.h"
#include "water_surface.h"
#include "app.h"
