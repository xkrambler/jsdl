#ifndef __LIB_H
#define __LIB_H

#include "common.h"

//class LIB:public Object {
class LIB {
	public:
		static v8::Persistent<v8::Context> createShellContext();
		static v8::Handle<v8::String> readfile(const char* name);	
		static v8::Handle<v8::Value> echo(const v8::Arguments& args);
		static v8::Handle<v8::Value> read(const v8::Arguments& args);
		static v8::Handle<v8::Value> include(const v8::Arguments& args);
		static v8::Handle<v8::Value> exit(const v8::Arguments& args);
		static v8::Handle<v8::Value> version(const v8::Arguments& args);
		static v8::Handle<v8::Value> kernelInit(const v8::Arguments& args);	
		static v8::Handle<v8::Value> kernelArgs(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiWindow(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiDestroyWindow(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiSetWindowFullscreen(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiWindowTitle(const v8::Arguments& args);	
		static v8::Handle<v8::Value> gdiCreateRenderer(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiLoadImage(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiFontOpen(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiText(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiCreateTextureFromSurface(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiFreeSurface(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiDestroyTexture(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiRenderClear(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiRenderPresent(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiRenderTexture(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiSetTextureAlphaMod(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiQueryTexture(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiDelay(const v8::Arguments& args);
		static v8::Handle<v8::Value> gdiPollEvent(const v8::Arguments& args);
		static v8::Handle<v8::Value> serialInit(const v8::Arguments& args);
		static v8::Handle<v8::Value> serialOpen(const v8::Arguments& args);
		static v8::Handle<v8::Value> serialRead(const v8::Arguments& args);
		static v8::Handle<v8::Value> serialData(const v8::Arguments& args);
		static v8::Handle<v8::Value> serialWrite(const v8::Arguments& args);
		static v8::Handle<v8::Value> serialClose(const v8::Arguments& args);
		static v8::Handle<v8::Value> serialError(const v8::Arguments& args);
		static v8::Handle<v8::Value> serialDestroy(const v8::Arguments& args);
		static bool executestring(
			v8::Handle<v8::String> source,
			v8::Handle<v8::Value> name,
			bool print_result,
			bool report_exceptions
		);
		static void reportexception(v8::TryCatch* try_catch);
};

#endif
