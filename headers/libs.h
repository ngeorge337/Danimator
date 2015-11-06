#pragma once

#pragma warning(disable: 4800)
#pragma warning(disable: 4244)	// float to int possible loss of data
#pragma warning(disable: 4018)  // signed/unsigned mismatch

#define WIN32_LEAN_AND_MEAN


#include <new>
//#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <cstdlib>

#define NOMINMAX 
#include <errno.h>
#include <windows.h>
//#include <afxwin.h>

#include <cassert>
#include <cstdio>
#include <math.h>
#include <map>
//#include <iostream>
//#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <algorithm>
#include <list>
#include <mmsystem.h>
#include <exception>
#include <limits>
#include <thread>
#include <mutex>
#include <functional>
#include <memory>
#include <condition_variable>
#include <direct.h>
#include <sstream>

#include "dirent.h"

#include "ft2build.h"
#include FT_FREETYPE_H

//#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/wglew.h>
//#include <glut.h>
#include <GL/GL.h>
//#include <GLAux.h>
//#include <glext.h>

// SFML Stuff
//#define SFML_STATIC
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Audio.hpp>

#define wxNO_WEBVIEW_LIB
#include "wx/setup.h"
#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/glcanvas.h"
#include "wx/spinctrl.h"
#include "wx/artprov.h"
#include "wx/imagpng.h"
#include "wx/ffile.h"

#include "util.h"
#include "static.h"
#include "bimap.h"
#include "random.h"
#include "thread.h"
#include "clamped.h"
#include "locator.h"
#include "token.h"
#include "strmanip.h"
#include "waiting.h"
#include "file.h"
#include "configfile.h"
#include "console.h"
#include "ccmd.h"
#include "cvar.h"
#include "externcvar.h"
#include "camera.h"

#include "resrcman.h"
#include "texman.h"
#include "soundman.h"
#include "audio.h"

static Random random(time(NULL));

const float DOOMTICK = 1.f / 35.f;

#define TICS(n) (DOOMTICK * float(n));