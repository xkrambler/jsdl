#ifndef __COMMON_H
#define __COMMON_H

#include <v8.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

//#include <string>
#include <vector>
//#include <map>
//#include <algorithm>
//#include <fstream>
//#include <iostream>
//#include <sstream>
//#include <iterator>
////#include <sstream>
////#include <cstdlib>
////#include <libgen.h> // incluye dirname, basename, ...
#include <unistd.h> // incluye chdir, ...
//#include <time.h>
//#include <math.h>

using namespace std;

class C {
	public:
		static const char* cstr(const v8::String::Utf8Value& value);
		static string v8string(const v8::Local<v8::Value>& value);
		static unsigned long pointerToHandle(void *);
		static void* handleToPointer(const v8::Local<v8::Value>& value);
};

#endif
