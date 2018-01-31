#include "lib.h"
#include "kernel.h"

/*

	// clase de acceso al puerto serie
	class AccordSerial {

		protected $o;
		protected $h;

		// constructor y parámetros
		function __construct($o) {
			$this->o=$o;
		}

		// abrir puerto
		function open() {
			passthru("stty -F ".$this->o["dev"]." ".$this->o["bauds"]." cs".$this->o["bits"]." -cstopb -parenb ignbrk -brkint -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts");
			if (!$this->h=fopen($this->o["dev"], "rw+b")) return false;
			stream_set_blocking($this->h, false);
			return true;
		}

		// leer del puerto
		function read() {
			if (feof($this->h)) return false;
			return fread($this->h, 1024);
		}

		// escribir datos
		function write($d) {
			if (feof($this->h)) return false;
			return fwrite($this->h, $d);
		}

		// cerrar puerto
		function close() {
			fclose($this->h);
		}

	}

*/

// creates a new execution environment containing the built-in functions
v8::Persistent<v8::Context> LIB::createShellContext() {

	// global object
	v8::Handle<v8::ObjectTemplate> global=v8::ObjectTemplate::New();

	// lib object
	v8::Handle<v8::ObjectTemplate> lib=v8::ObjectTemplate::New();

	// lib object functions (lib.echo, lib.read, ...)
	lib->Set(v8::String::New("version"), v8::FunctionTemplate::New(LIB::version));
	lib->Set(v8::String::New("echo"), v8::FunctionTemplate::New(LIB::echo));
	lib->Set(v8::String::New("read"), v8::FunctionTemplate::New(LIB::read));
	lib->Set(v8::String::New("include"), v8::FunctionTemplate::New(LIB::include));
	lib->Set(v8::String::New("exit"), v8::FunctionTemplate::New(LIB::exit));

	// lib.kernel object
	v8::Handle<v8::ObjectTemplate> k=v8::ObjectTemplate::New();
	k->Set(v8::String::New("init"), v8::FunctionTemplate::New(LIB::kernelInit));
	k->Set(v8::String::New("args"), v8::FunctionTemplate::New(LIB::kernelArgs));
	lib->Set(v8::String::New("kernel"), k);

	// lib.serial object
	v8::Handle<v8::ObjectTemplate> serial=v8::ObjectTemplate::New();
	serial->Set(v8::String::New("init"), v8::FunctionTemplate::New(LIB::serialInit));
	serial->Set(v8::String::New("open"), v8::FunctionTemplate::New(LIB::serialOpen));
	serial->Set(v8::String::New("read"), v8::FunctionTemplate::New(LIB::serialRead));
	serial->Set(v8::String::New("data"), v8::FunctionTemplate::New(LIB::serialData));
	serial->Set(v8::String::New("write"), v8::FunctionTemplate::New(LIB::serialWrite));
	serial->Set(v8::String::New("close"), v8::FunctionTemplate::New(LIB::serialClose));
	serial->Set(v8::String::New("error"), v8::FunctionTemplate::New(LIB::serialError));
	serial->Set(v8::String::New("destroy"), v8::FunctionTemplate::New(LIB::serialDestroy));
	lib->Set(v8::String::New("serial"), serial);

	// lib.gdi object
	v8::Handle<v8::ObjectTemplate> gdi=v8::ObjectTemplate::New();
	gdi->Set(v8::String::New("window"), v8::FunctionTemplate::New(LIB::gdiWindow));
	gdi->Set(v8::String::New("destroyWindow"), v8::FunctionTemplate::New(LIB::gdiDestroyWindow));
	gdi->Set(v8::String::New("setWindowFullscreen"), v8::FunctionTemplate::New(LIB::gdiSetWindowFullscreen));
	gdi->Set(v8::String::New("windowTitle"), v8::FunctionTemplate::New(LIB::gdiWindowTitle));
	gdi->Set(v8::String::New("createRenderer"), v8::FunctionTemplate::New(LIB::gdiCreateRenderer));
	gdi->Set(v8::String::New("loadImage"), v8::FunctionTemplate::New(LIB::gdiLoadImage));
	gdi->Set(v8::String::New("fontOpen"), v8::FunctionTemplate::New(LIB::gdiFontOpen));
	gdi->Set(v8::String::New("text"), v8::FunctionTemplate::New(LIB::gdiText));
	gdi->Set(v8::String::New("createTextureFromSurface"), v8::FunctionTemplate::New(LIB::gdiCreateTextureFromSurface));
	gdi->Set(v8::String::New("freeSurface"), v8::FunctionTemplate::New(LIB::gdiFreeSurface));
	gdi->Set(v8::String::New("destroyTexture"), v8::FunctionTemplate::New(LIB::gdiDestroyTexture));
	gdi->Set(v8::String::New("renderClear"), v8::FunctionTemplate::New(LIB::gdiRenderClear));
	gdi->Set(v8::String::New("renderTexture"), v8::FunctionTemplate::New(LIB::gdiRenderTexture));
	gdi->Set(v8::String::New("queryTexture"), v8::FunctionTemplate::New(LIB::gdiQueryTexture));
	gdi->Set(v8::String::New("setTextureAlphaMod"), v8::FunctionTemplate::New(LIB::gdiSetTextureAlphaMod));
	gdi->Set(v8::String::New("renderPresent"), v8::FunctionTemplate::New(LIB::gdiRenderPresent));
	gdi->Set(v8::String::New("pollEvent"), v8::FunctionTemplate::New(LIB::gdiPollEvent));
	gdi->Set(v8::String::New("delay"), v8::FunctionTemplate::New(LIB::gdiDelay));
	lib->Set(v8::String::New("gdi"), gdi);

	// lib to global
	global->Set(v8::String::New("lib"), lib);

	// return global context
	return v8::Context::New(NULL, global);

}

// initialize all
v8::Handle<v8::Value> LIB::kernelInit(const v8::Arguments& args) {
	return v8::Boolean::New(kernel.init());
}

// return arguments
v8::Handle<v8::Value> LIB::kernelArgs(const v8::Arguments& args) {
	v8::Handle<v8::Array> o=v8::Array::New(kernel.argc());
	for (int i=0;i<kernel.argc();i++)
		o->Set(v8::Integer::New(i), v8::String::New(kernel.argv(i)));
	return o;
}

// create window
v8::Handle<v8::Value> LIB::gdiWindow(const v8::Arguments& args) {
	string title="";
	int x=0, y=0, w=640, h=480, flags=0;
	if (args.Length()>0) title=C::v8string(args[0]);
	if (args.Length()>1) x=args[1]->IntegerValue();
	if (args.Length()>2) y=args[2]->IntegerValue();
	if (args.Length()>3) w=args[3]->IntegerValue();
	if (args.Length()>4) h=args[4]->IntegerValue();
	if (args.Length()>5) flags=args[5]->IntegerValue();
	SDL_Window *window=SDL_CreateWindow(title.c_str(), x, y, w, h, flags);		
	return v8::Number::New(C::pointerToHandle(window));
}

// serial create connection
v8::Handle<v8::Value> LIB::serialInit(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	Serial* serial=new Serial(C::v8string(args[0]));
	return v8::Number::New(C::pointerToHandle(serial));
}

// serial open
v8::Handle<v8::Value> LIB::serialOpen(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	Serial* serial=(Serial*) C::handleToPointer(args[0]);
	if (!serial->open()) return v8::Boolean::New(false);
	return v8::Boolean::New(true);
}

// serial read data
v8::Handle<v8::Value> LIB::serialRead(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	Serial* serial=(Serial*) C::handleToPointer(args[0]);
	int n=serial->read();
	return v8::Number::New(n);
}

// serial get data readed
v8::Handle<v8::Value> LIB::serialData(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	Serial* serial=(Serial*) C::handleToPointer(args[0]);
	string s=serial->data();
	return v8::String::New(s.c_str());
}

// serial write data
v8::Handle<v8::Value> LIB::serialWrite(const v8::Arguments& args) {
	if (args.Length()<2) return v8::Boolean::New(false);
	Serial* serial=(Serial*) C::handleToPointer(args[0]);
	int n=serial->write(C::v8string(args[1]).c_str());
	return v8::Number::New(n);
}

// serial close connection
v8::Handle<v8::Value> LIB::serialClose(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	Serial* serial=(Serial*) C::handleToPointer(args[0]);
	serial->close();
	//delete serial;
	return v8::Boolean::New(true);
}

// serial get last error
v8::Handle<v8::Value> LIB::serialError(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	Serial* serial=(Serial*) C::handleToPointer(args[0]);
	string s=serial->error();
	return v8::String::New(s.c_str());
}

// serial destroy
v8::Handle<v8::Value> LIB::serialDestroy(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	Serial* serial=(Serial*) C::handleToPointer(args[0]);
	serial->close();
	delete serial;
	return v8::Boolean::New(true);
}

// destroy window
v8::Handle<v8::Value> LIB::gdiDestroyWindow(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	SDL_Window* window=(SDL_Window*) C::handleToPointer(args[0]);
	SDL_DestroyWindow(window);
	//delete window;
	return v8::Boolean::New(true);;
}

// set window title
v8::Handle<v8::Value> LIB::gdiWindowTitle(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	if (args.Length()<2) return v8::String::New(SDL_GetWindowTitle((SDL_Window*) C::handleToPointer(args[0])));
	SDL_SetWindowTitle((SDL_Window*) C::handleToPointer(args[0]), C::v8string(args[1]).c_str());
	return v8::Boolean::New(true);
}

// set window fullscreen
v8::Handle<v8::Value> LIB::gdiSetWindowFullscreen(const v8::Arguments& args) {
	if (args.Length()<2) return v8::Boolean::New(false);
	int mode=args[1]->IntegerValue();
	if (mode==1) mode=SDL_WINDOW_FULLSCREEN;
	else if (mode==2) mode=SDL_WINDOW_FULLSCREEN_DESKTOP;
	SDL_SetWindowFullscreen((SDL_Window*) C::handleToPointer(args[0]), mode);
	return v8::Boolean::New(true);
}

// create renderer
v8::Handle<v8::Value> LIB::gdiCreateRenderer(const v8::Arguments& args) {
	if (args.Length()<3) return v8::Boolean::New(false);
	SDL_Window* window=(SDL_Window*) C::handleToPointer(args[0]);
	SDL_Renderer *renderer=SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	return v8::Number::New(C::pointerToHandle(renderer));
}

// load image
v8::Handle<v8::Value> LIB::gdiLoadImage(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	SDL_Surface* image=IMG_Load(C::v8string(args[0]).c_str());
	return v8::Number::New(C::pointerToHandle(image));
}

// open font TTF
v8::Handle<v8::Value> LIB::gdiFontOpen(const v8::Arguments& args) {
	if (args.Length()<2) return v8::Boolean::New(false);
	TTF_Font* font=TTF_OpenFont(C::v8string(args[0]).c_str(), args[1]->IntegerValue()); // fontfile, fontsize
	return v8::Number::New(C::pointerToHandle(font));
}

// render text
v8::Handle<v8::Value> LIB::gdiText(const v8::Arguments& args) {
	if (args.Length()<2) return v8::Boolean::New(false);
	Uint8 r=255, g=255, b=255, a=255;
	if (args[2]->IsArray()) {
		v8::Handle<v8::Array> arr=v8::Handle<v8::Array>::Cast(args[2]);
		if (arr->Length()>2) {
			r=arr->Get(0)->ToObject()->Uint32Value();
			g=arr->Get(1)->ToObject()->Uint32Value();
			b=arr->Get(2)->ToObject()->Uint32Value();
		} else if (arr->Length()>3) {
			a=arr->Get(2)->ToObject()->Uint32Value();
		}
	}
	SDL_Color    color  ={r, g, b, a};
	SDL_Surface* surface=TTF_RenderUTF8_Blended((TTF_Font*) C::handleToPointer(args[0]), C::v8string(args[1]).c_str(), color);
	return v8::Number::New(C::pointerToHandle(surface));
}

// CreateTextureFromSurface
v8::Handle<v8::Value> LIB::gdiCreateTextureFromSurface(const v8::Arguments& args) {
	if (args.Length()<2) return v8::Boolean::New(false);
	SDL_Renderer* renderer=(SDL_Renderer*) C::handleToPointer(args[0]);
	SDL_Surface*  surface =(SDL_Surface*) C::handleToPointer(args[1]);
	SDL_Texture*  texture =SDL_CreateTextureFromSurface(renderer, surface);
	return v8::Number::New(C::pointerToHandle(texture));
}

// SDL_FreeSurface
v8::Handle<v8::Value> LIB::gdiFreeSurface(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	SDL_FreeSurface((SDL_Surface*) C::handleToPointer(args[0]));
	return v8::Boolean::New(true);
}

// SDL_DestroyTexture
v8::Handle<v8::Value> LIB::gdiDestroyTexture(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	SDL_DestroyTexture((SDL_Texture*) C::handleToPointer(args[0]));
	return v8::Boolean::New(true);
}

// renderClear
v8::Handle<v8::Value> LIB::gdiRenderClear(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	SDL_RenderClear((SDL_Renderer*) C::handleToPointer(args[0]));
	return v8::Boolean::New(true);
}
	
// renderTexture
v8::Handle<v8::Value> LIB::gdiRenderTexture(const v8::Arguments& args) {
	if (args.Length()<2) return v8::Boolean::New(false);
	SDL_Renderer* renderer=(SDL_Renderer*) C::handleToPointer(args[0]);
	SDL_Texture*  texture =(SDL_Texture*)  C::handleToPointer(args[1]);
	SDL_Rect rect;
	rect.x=(args.Length()>2?args[2]->IntegerValue():0);
	rect.y=(args.Length()>3?args[3]->IntegerValue():0);
	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	if (args.Length()>4) {
		int align=args[4]->IntegerValue();
		if (align%3==1) rect.x-=rect.w/2;
		if (align%3==2) rect.x-=rect.w;
		if (align/3==1) rect.y-=rect.h/2;
		if (align/3==2) rect.y-=rect.h;
	}
	SDL_RenderCopy(renderer, texture, NULL, &rect);
	return v8::Boolean::New(true);
}

// queryTexture
v8::Handle<v8::Value> LIB::gdiQueryTexture(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	SDL_Texture* texture =(SDL_Texture*) C::handleToPointer(args[0]);
	SDL_Rect rect;
	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	v8::Handle<v8::Object> o=v8::Object::New();
	o->Set(v8::String::New("w"), v8::Number::New(rect.w));
	o->Set(v8::String::New("h"), v8::Number::New(rect.h));
	return o;
}

// setTextureAlphaMod
v8::Handle<v8::Value> LIB::gdiSetTextureAlphaMod(const v8::Arguments& args) {
	if (args.Length()<2) return v8::Boolean::New(false);
	SDL_Texture* texture =(SDL_Texture*) C::handleToPointer(args[0]);
	Uint8 alpha=args[1]->IntegerValue();
	if (alpha<255) SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	else SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
	int ret=SDL_SetTextureAlphaMod(texture, alpha);
	return v8::Boolean::New(ret==0);
}

// renderPresent
v8::Handle<v8::Value> LIB::gdiRenderPresent(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	SDL_Renderer* renderer=(SDL_Renderer*) C::handleToPointer(args[0]);
	SDL_RenderPresent(renderer);
	return v8::Boolean::New(true);
}

// delay
v8::Handle<v8::Value> LIB::gdiDelay(const v8::Arguments& args) {
	if (args.Length()<1) return v8::Boolean::New(false);
	unsigned int delay=args[0]->IntegerValue();
	SDL_Delay(delay);
	return v8::Boolean::New(true);
}

// poll event
v8::Handle<v8::Value> LIB::gdiPollEvent(const v8::Arguments& args) {
	SDL_Event event;
	SDL_PollEvent(&event);

	v8::Handle<v8::Object> e=v8::Object::New();
	switch (event.type) {
	case SDL_QUIT:
		e->Set(v8::String::New("type"), v8::String::New("quit"));
		break;

	case SDL_WINDOWEVENT:
		{
			e->Set(v8::String::New("type"), v8::String::New("windowevent"));
			v8::Handle<v8::Object> window=v8::Object::New();
			window->Set(v8::String::New("windowID"), v8::Number::New(event.window.windowID));
			switch (event.window.event) {
			case SDL_WINDOWEVENT_NONE: window->Set(v8::String::New("event"), v8::String::New("none")); break;
			case SDL_WINDOWEVENT_SHOWN: window->Set(v8::String::New("event"), v8::String::New("shown")); break;
			case SDL_WINDOWEVENT_HIDDEN: window->Set(v8::String::New("event"), v8::String::New("hidden")); break;
			case SDL_WINDOWEVENT_EXPOSED: window->Set(v8::String::New("event"), v8::String::New("exposed")); break;
			case SDL_WINDOWEVENT_MOVED:
				window->Set(v8::String::New("event"), v8::String::New("moved"));
				window->Set(v8::String::New("x"), v8::Number::New(event.window.data1));
				window->Set(v8::String::New("y"), v8::Number::New(event.window.data2));
				break;
			case SDL_WINDOWEVENT_RESIZED:
				window->Set(v8::String::New("event"), v8::String::New("resized"));
				window->Set(v8::String::New("w"), v8::Number::New(event.window.data1));
				window->Set(v8::String::New("h"), v8::Number::New(event.window.data2));
				break;
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				window->Set(v8::String::New("event"), v8::String::New("size_changed"));
				window->Set(v8::String::New("x"), v8::Number::New(event.window.data1));
				window->Set(v8::String::New("y"), v8::Number::New(event.window.data2));
				break;
			case SDL_WINDOWEVENT_MINIMIZED: window->Set(v8::String::New("event"), v8::String::New("minimized")); break;
			case SDL_WINDOWEVENT_MAXIMIZED: window->Set(v8::String::New("event"), v8::String::New("maximized")); break;
			case SDL_WINDOWEVENT_RESTORED: window->Set(v8::String::New("event"), v8::String::New("restored")); break;
			case SDL_WINDOWEVENT_ENTER: window->Set(v8::String::New("event"), v8::String::New("enter")); break;
			case SDL_WINDOWEVENT_LEAVE: window->Set(v8::String::New("event"), v8::String::New("leave")); break;
			case SDL_WINDOWEVENT_FOCUS_GAINED: window->Set(v8::String::New("event"), v8::String::New("focus_gained")); break;
			case SDL_WINDOWEVENT_FOCUS_LOST: window->Set(v8::String::New("event"), v8::String::New("focus_lost")); break;
			case SDL_WINDOWEVENT_CLOSE: window->Set(v8::String::New("event"), v8::String::New("close")); break;
			//case SDL_WINDOWEVENT_TAKE_FOCUS: window->Set(v8::String::New("event"), v8::String::New("take_focus")); break;
			//case SDL_WINDOWEVENT_HIT_TEST: window->Set(v8::String::New("event"), v8::String::New("hit_test")); break;
			default: window->Set(v8::String::New("event"), v8::Number::New(event.window.event)); break;
 			}
			e->Set(v8::String::New("window"), window);
		}
		break;

	case SDL_KEYUP:
	case SDL_KEYDOWN:
		{
			e->Set(v8::String::New("type"), v8::String::New(event.type==SDL_KEYUP?"keyup":"keydown"));
			v8::Handle<v8::Object> key=v8::Object::New();
			v8::Handle<v8::Object> keysym=v8::Object::New();
			// sym
			switch (event.key.keysym.sym) {
			case SDLK_CAPSLOCK: keysym->Set(v8::String::New("sym"), v8::String::New("capslock")); break;
			case SDLK_F1: keysym->Set(v8::String::New("sym"), v8::String::New("f1")); break;
			case SDLK_F2: keysym->Set(v8::String::New("sym"), v8::String::New("f2")); break;
			case SDLK_F3: keysym->Set(v8::String::New("sym"), v8::String::New("f3")); break;
			case SDLK_F4: keysym->Set(v8::String::New("sym"), v8::String::New("f4")); break;
			case SDLK_F5: keysym->Set(v8::String::New("sym"), v8::String::New("f5")); break;
			case SDLK_F6: keysym->Set(v8::String::New("sym"), v8::String::New("f6")); break;
			case SDLK_F7: keysym->Set(v8::String::New("sym"), v8::String::New("f7")); break;
			case SDLK_F8: keysym->Set(v8::String::New("sym"), v8::String::New("f8")); break;
			case SDLK_F9: keysym->Set(v8::String::New("sym"), v8::String::New("f9")); break;
			case SDLK_F10: keysym->Set(v8::String::New("sym"), v8::String::New("f10")); break;
			case SDLK_F11: keysym->Set(v8::String::New("sym"), v8::String::New("f11")); break;
			case SDLK_F12: keysym->Set(v8::String::New("sym"), v8::String::New("f12")); break;
			case SDLK_PRINTSCREEN: keysym->Set(v8::String::New("sym"), v8::String::New("printscreen")); break;
			case SDLK_SCROLLLOCK: keysym->Set(v8::String::New("sym"), v8::String::New("scrolllock")); break;
			case SDLK_PAUSE: keysym->Set(v8::String::New("sym"), v8::String::New("pause")); break;
			case SDLK_INSERT: keysym->Set(v8::String::New("sym"), v8::String::New("insert")); break;
			case SDLK_HOME: keysym->Set(v8::String::New("sym"), v8::String::New("home")); break;
			case SDLK_PAGEUP: keysym->Set(v8::String::New("sym"), v8::String::New("pageup")); break;
			case SDLK_DELETE: keysym->Set(v8::String::New("sym"), v8::String::New("delete")); break;
			case SDLK_END: keysym->Set(v8::String::New("sym"), v8::String::New("end")); break;
			case SDLK_PAGEDOWN: keysym->Set(v8::String::New("sym"), v8::String::New("pagedown")); break;
			case SDLK_RIGHT: keysym->Set(v8::String::New("sym"), v8::String::New("right")); break;
			case SDLK_LEFT: keysym->Set(v8::String::New("sym"), v8::String::New("left")); break;
			case SDLK_DOWN: keysym->Set(v8::String::New("sym"), v8::String::New("down")); break;
			case SDLK_UP: keysym->Set(v8::String::New("sym"), v8::String::New("up")); break;
			case SDLK_NUMLOCKCLEAR: keysym->Set(v8::String::New("sym"), v8::String::New("numlockclear")); break;
			case SDLK_KP_DIVIDE: keysym->Set(v8::String::New("sym"), v8::String::New("kp_divide")); break;
			case SDLK_KP_MULTIPLY: keysym->Set(v8::String::New("sym"), v8::String::New("kp_multiply")); break;
			case SDLK_KP_MINUS: keysym->Set(v8::String::New("sym"), v8::String::New("kp_minus")); break;
			case SDLK_KP_PLUS: keysym->Set(v8::String::New("sym"), v8::String::New("kp_plus")); break;
			case SDLK_KP_ENTER: keysym->Set(v8::String::New("sym"), v8::String::New("kp_enter")); break;
			case SDLK_KP_1: keysym->Set(v8::String::New("sym"), v8::String::New("kp_1")); break;
			case SDLK_KP_2: keysym->Set(v8::String::New("sym"), v8::String::New("kp_2")); break;
			case SDLK_KP_3: keysym->Set(v8::String::New("sym"), v8::String::New("kp_3")); break;
			case SDLK_KP_4: keysym->Set(v8::String::New("sym"), v8::String::New("kp_4")); break;
			case SDLK_KP_5: keysym->Set(v8::String::New("sym"), v8::String::New("kp_5")); break;
			case SDLK_KP_6: keysym->Set(v8::String::New("sym"), v8::String::New("kp_6")); break;
			case SDLK_KP_7: keysym->Set(v8::String::New("sym"), v8::String::New("kp_7")); break;
			case SDLK_KP_8: keysym->Set(v8::String::New("sym"), v8::String::New("kp_8")); break;
			case SDLK_KP_9: keysym->Set(v8::String::New("sym"), v8::String::New("kp_9")); break;
			case SDLK_KP_0: keysym->Set(v8::String::New("sym"), v8::String::New("kp_0")); break;
			case SDLK_KP_PERIOD: keysym->Set(v8::String::New("sym"), v8::String::New("kp_period")); break;
			case SDLK_APPLICATION: keysym->Set(v8::String::New("sym"), v8::String::New("application")); break;
			case SDLK_POWER: keysym->Set(v8::String::New("sym"), v8::String::New("power")); break;
			case SDLK_KP_EQUALS: keysym->Set(v8::String::New("sym"), v8::String::New("kp_equals")); break;
			case SDLK_F13: keysym->Set(v8::String::New("sym"), v8::String::New("f13")); break;
			case SDLK_F14: keysym->Set(v8::String::New("sym"), v8::String::New("f14")); break;
			case SDLK_F15: keysym->Set(v8::String::New("sym"), v8::String::New("f15")); break;
			case SDLK_F16: keysym->Set(v8::String::New("sym"), v8::String::New("f16")); break;
			case SDLK_F17: keysym->Set(v8::String::New("sym"), v8::String::New("f17")); break;
			case SDLK_F18: keysym->Set(v8::String::New("sym"), v8::String::New("f18")); break;
			case SDLK_F19: keysym->Set(v8::String::New("sym"), v8::String::New("f19")); break;
			case SDLK_F20: keysym->Set(v8::String::New("sym"), v8::String::New("f20")); break;
			case SDLK_F21: keysym->Set(v8::String::New("sym"), v8::String::New("f21")); break;
			case SDLK_F22: keysym->Set(v8::String::New("sym"), v8::String::New("f22")); break;
			case SDLK_F23: keysym->Set(v8::String::New("sym"), v8::String::New("f23")); break;
			case SDLK_F24: keysym->Set(v8::String::New("sym"), v8::String::New("f24")); break;
			case SDLK_EXECUTE: keysym->Set(v8::String::New("sym"), v8::String::New("execute")); break;
			case SDLK_HELP: keysym->Set(v8::String::New("sym"), v8::String::New("help")); break;
			case SDLK_MENU: keysym->Set(v8::String::New("sym"), v8::String::New("menu")); break;
			case SDLK_SELECT: keysym->Set(v8::String::New("sym"), v8::String::New("select")); break;
			case SDLK_STOP: keysym->Set(v8::String::New("sym"), v8::String::New("stop")); break;
			case SDLK_AGAIN: keysym->Set(v8::String::New("sym"), v8::String::New("again")); break;
			case SDLK_UNDO: keysym->Set(v8::String::New("sym"), v8::String::New("undo")); break;
			case SDLK_CUT: keysym->Set(v8::String::New("sym"), v8::String::New("cut")); break;
			case SDLK_COPY: keysym->Set(v8::String::New("sym"), v8::String::New("copy")); break;
			case SDLK_PASTE: keysym->Set(v8::String::New("sym"), v8::String::New("paste")); break;
			case SDLK_FIND: keysym->Set(v8::String::New("sym"), v8::String::New("find")); break;
			case SDLK_MUTE: keysym->Set(v8::String::New("sym"), v8::String::New("mute")); break;
			case SDLK_VOLUMEUP: keysym->Set(v8::String::New("sym"), v8::String::New("volumeup")); break;
			case SDLK_VOLUMEDOWN: keysym->Set(v8::String::New("sym"), v8::String::New("volumedown")); break;
			case SDLK_KP_COMMA: keysym->Set(v8::String::New("sym"), v8::String::New("kp_comma")); break;
			case SDLK_KP_EQUALSAS400: keysym->Set(v8::String::New("sym"), v8::String::New("kp_equalsas400")); break;
			case SDLK_ALTERASE: keysym->Set(v8::String::New("sym"), v8::String::New("alterase")); break;
			case SDLK_SYSREQ: keysym->Set(v8::String::New("sym"), v8::String::New("sysreq")); break;
			case SDLK_CANCEL: keysym->Set(v8::String::New("sym"), v8::String::New("cancel")); break;
			case SDLK_CLEAR: keysym->Set(v8::String::New("sym"), v8::String::New("clear")); break;
			case SDLK_PRIOR: keysym->Set(v8::String::New("sym"), v8::String::New("prior")); break;
			case SDLK_RETURN2: keysym->Set(v8::String::New("sym"), v8::String::New("return2")); break;
			case SDLK_SEPARATOR: keysym->Set(v8::String::New("sym"), v8::String::New("separator")); break;
			case SDLK_OUT: keysym->Set(v8::String::New("sym"), v8::String::New("out")); break;
			case SDLK_OPER: keysym->Set(v8::String::New("sym"), v8::String::New("oper")); break;
			case SDLK_CLEARAGAIN: keysym->Set(v8::String::New("sym"), v8::String::New("clearagain")); break;
			case SDLK_CRSEL: keysym->Set(v8::String::New("sym"), v8::String::New("crsel")); break;
			case SDLK_EXSEL: keysym->Set(v8::String::New("sym"), v8::String::New("exsel")); break;
			case SDLK_KP_00: keysym->Set(v8::String::New("sym"), v8::String::New("kp_00")); break;
			case SDLK_KP_000: keysym->Set(v8::String::New("sym"), v8::String::New("kp_000")); break;
			case SDLK_THOUSANDSSEPARATOR: keysym->Set(v8::String::New("sym"), v8::String::New("thousandsseparator")); break;
			case SDLK_DECIMALSEPARATOR: keysym->Set(v8::String::New("sym"), v8::String::New("decimalseparator")); break;
			case SDLK_CURRENCYUNIT: keysym->Set(v8::String::New("sym"), v8::String::New("currencyunit")); break;
			case SDLK_CURRENCYSUBUNIT: keysym->Set(v8::String::New("sym"), v8::String::New("currencysubunit")); break;
			case SDLK_KP_LEFTPAREN: keysym->Set(v8::String::New("sym"), v8::String::New("kp_leftparen")); break;
			case SDLK_KP_RIGHTPAREN: keysym->Set(v8::String::New("sym"), v8::String::New("kp_rightparen")); break;
			case SDLK_KP_LEFTBRACE: keysym->Set(v8::String::New("sym"), v8::String::New("kp_leftbrace")); break;
			case SDLK_KP_RIGHTBRACE: keysym->Set(v8::String::New("sym"), v8::String::New("kp_rightbrace")); break;
			case SDLK_KP_TAB: keysym->Set(v8::String::New("sym"), v8::String::New("kp_tab")); break;
			case SDLK_KP_BACKSPACE: keysym->Set(v8::String::New("sym"), v8::String::New("kp_backspace")); break;
			case SDLK_KP_A: keysym->Set(v8::String::New("sym"), v8::String::New("kp_a")); break;
			case SDLK_KP_B: keysym->Set(v8::String::New("sym"), v8::String::New("kp_b")); break;
			case SDLK_KP_C: keysym->Set(v8::String::New("sym"), v8::String::New("kp_c")); break;
			case SDLK_KP_D: keysym->Set(v8::String::New("sym"), v8::String::New("kp_d")); break;
			case SDLK_KP_E: keysym->Set(v8::String::New("sym"), v8::String::New("kp_e")); break;
			case SDLK_KP_F: keysym->Set(v8::String::New("sym"), v8::String::New("kp_f")); break;
			case SDLK_KP_XOR: keysym->Set(v8::String::New("sym"), v8::String::New("kp_xor")); break;
			case SDLK_KP_POWER: keysym->Set(v8::String::New("sym"), v8::String::New("kp_power")); break;
			case SDLK_KP_PERCENT: keysym->Set(v8::String::New("sym"), v8::String::New("kp_percent")); break;
			case SDLK_KP_LESS: keysym->Set(v8::String::New("sym"), v8::String::New("kp_less")); break;
			case SDLK_KP_GREATER: keysym->Set(v8::String::New("sym"), v8::String::New("kp_greater")); break;
			case SDLK_KP_AMPERSAND: keysym->Set(v8::String::New("sym"), v8::String::New("kp_ampersand")); break;
			case SDLK_KP_DBLAMPERSAND: keysym->Set(v8::String::New("sym"), v8::String::New("kp_dblampersand")); break;
			case SDLK_KP_VERTICALBAR: keysym->Set(v8::String::New("sym"), v8::String::New("kp_verticalbar")); break;
			case SDLK_KP_DBLVERTICALBAR: keysym->Set(v8::String::New("sym"), v8::String::New("kp_dblverticalbar")); break;
			case SDLK_KP_COLON: keysym->Set(v8::String::New("sym"), v8::String::New("kp_colon")); break;
			case SDLK_KP_HASH: keysym->Set(v8::String::New("sym"), v8::String::New("kp_hash")); break;
			case SDLK_KP_SPACE: keysym->Set(v8::String::New("sym"), v8::String::New("kp_space")); break;
			case SDLK_KP_AT: keysym->Set(v8::String::New("sym"), v8::String::New("kp_at")); break;
			case SDLK_KP_EXCLAM: keysym->Set(v8::String::New("sym"), v8::String::New("kp_exclam")); break;
			case SDLK_KP_MEMSTORE: keysym->Set(v8::String::New("sym"), v8::String::New("kp_memstore")); break;
			case SDLK_KP_MEMRECALL: keysym->Set(v8::String::New("sym"), v8::String::New("kp_memrecall")); break;
			case SDLK_KP_MEMCLEAR: keysym->Set(v8::String::New("sym"), v8::String::New("kp_memclear")); break;
			case SDLK_KP_MEMADD: keysym->Set(v8::String::New("sym"), v8::String::New("kp_memadd")); break;
			case SDLK_KP_MEMSUBTRACT: keysym->Set(v8::String::New("sym"), v8::String::New("kp_memsubtract")); break;
			case SDLK_KP_MEMMULTIPLY: keysym->Set(v8::String::New("sym"), v8::String::New("kp_memmultiply")); break;
			case SDLK_KP_MEMDIVIDE: keysym->Set(v8::String::New("sym"), v8::String::New("kp_memdivide")); break;
			case SDLK_KP_PLUSMINUS: keysym->Set(v8::String::New("sym"), v8::String::New("kp_plusminus")); break;
			case SDLK_KP_CLEAR: keysym->Set(v8::String::New("sym"), v8::String::New("kp_clear")); break;
			case SDLK_KP_CLEARENTRY: keysym->Set(v8::String::New("sym"), v8::String::New("kp_clearentry")); break;
			case SDLK_KP_BINARY: keysym->Set(v8::String::New("sym"), v8::String::New("kp_binary")); break;
			case SDLK_KP_OCTAL: keysym->Set(v8::String::New("sym"), v8::String::New("kp_octal")); break;
			case SDLK_KP_DECIMAL: keysym->Set(v8::String::New("sym"), v8::String::New("kp_decimal")); break;
			case SDLK_KP_HEXADECIMAL: keysym->Set(v8::String::New("sym"), v8::String::New("kp_hexadecimal")); break;
			case SDLK_LCTRL: keysym->Set(v8::String::New("sym"), v8::String::New("lctrl")); break;
			case SDLK_LSHIFT: keysym->Set(v8::String::New("sym"), v8::String::New("lshift")); break;
			case SDLK_LALT: keysym->Set(v8::String::New("sym"), v8::String::New("lalt")); break;
			case SDLK_LGUI: keysym->Set(v8::String::New("sym"), v8::String::New("lgui")); break;
			case SDLK_RCTRL: keysym->Set(v8::String::New("sym"), v8::String::New("rctrl")); break;
			case SDLK_RSHIFT: keysym->Set(v8::String::New("sym"), v8::String::New("rshift")); break;
			case SDLK_RALT: keysym->Set(v8::String::New("sym"), v8::String::New("ralt")); break;
			case SDLK_RGUI: keysym->Set(v8::String::New("sym"), v8::String::New("rgui")); break;
			case SDLK_MODE: keysym->Set(v8::String::New("sym"), v8::String::New("mode")); break;
			case SDLK_AUDIONEXT: keysym->Set(v8::String::New("sym"), v8::String::New("audionext")); break;
			case SDLK_AUDIOPREV: keysym->Set(v8::String::New("sym"), v8::String::New("audioprev")); break;
			case SDLK_AUDIOSTOP: keysym->Set(v8::String::New("sym"), v8::String::New("audiostop")); break;
			case SDLK_AUDIOPLAY: keysym->Set(v8::String::New("sym"), v8::String::New("audioplay")); break;
			case SDLK_AUDIOMUTE: keysym->Set(v8::String::New("sym"), v8::String::New("audiomute")); break;
			case SDLK_MEDIASELECT: keysym->Set(v8::String::New("sym"), v8::String::New("mediaselect")); break;
			case SDLK_WWW: keysym->Set(v8::String::New("sym"), v8::String::New("www")); break;
			case SDLK_MAIL: keysym->Set(v8::String::New("sym"), v8::String::New("mail")); break;
			case SDLK_CALCULATOR: keysym->Set(v8::String::New("sym"), v8::String::New("calculator")); break;
			case SDLK_COMPUTER: keysym->Set(v8::String::New("sym"), v8::String::New("computer")); break;
			case SDLK_AC_SEARCH: keysym->Set(v8::String::New("sym"), v8::String::New("ac_search")); break;
			case SDLK_AC_HOME: keysym->Set(v8::String::New("sym"), v8::String::New("ac_home")); break;
			case SDLK_AC_BACK: keysym->Set(v8::String::New("sym"), v8::String::New("ac_back")); break;
			case SDLK_AC_FORWARD: keysym->Set(v8::String::New("sym"), v8::String::New("ac_forward")); break;
			case SDLK_AC_STOP: keysym->Set(v8::String::New("sym"), v8::String::New("ac_stop")); break;
			case SDLK_AC_REFRESH: keysym->Set(v8::String::New("sym"), v8::String::New("ac_refresh")); break;
			case SDLK_AC_BOOKMARKS: keysym->Set(v8::String::New("sym"), v8::String::New("ac_bookmarks")); break;
			case SDLK_BRIGHTNESSDOWN: keysym->Set(v8::String::New("sym"), v8::String::New("brightnessdown")); break;
			case SDLK_BRIGHTNESSUP: keysym->Set(v8::String::New("sym"), v8::String::New("brightnessup")); break;
			case SDLK_DISPLAYSWITCH: keysym->Set(v8::String::New("sym"), v8::String::New("displayswitch")); break;
			case SDLK_KBDILLUMTOGGLE: keysym->Set(v8::String::New("sym"), v8::String::New("kbdillumtoggle")); break;
			case SDLK_KBDILLUMDOWN: keysym->Set(v8::String::New("sym"), v8::String::New("kbdillumdown")); break;
			case SDLK_KBDILLUMUP: keysym->Set(v8::String::New("sym"), v8::String::New("kbdillumup")); break;
			case SDLK_EJECT: keysym->Set(v8::String::New("sym"), v8::String::New("eject")); break;
			case SDLK_SLEEP: keysym->Set(v8::String::New("sym"), v8::String::New("sleep")); break;
			default: keysym->Set(v8::String::New("sym"), v8::Number::New(event.key.keysym.sym));
			}
			// mod
			int modc=0;
			v8::Handle<v8::Array> mod=v8::Array::New();
			if ((event.key.keysym.mod & KMOD_LSHIFT)==KMOD_LSHIFT)     mod->Set(v8::Integer::New(modc++), v8::String::New("lshift"));
			if ((event.key.keysym.mod & KMOD_RSHIFT)==KMOD_RSHIFT)     mod->Set(v8::Integer::New(modc++), v8::String::New("rshift"));
			if ((event.key.keysym.mod & KMOD_LCTRL)==KMOD_LCTRL)       mod->Set(v8::Integer::New(modc++), v8::String::New("lctrl"));
			if ((event.key.keysym.mod & KMOD_RCTRL)==KMOD_RCTRL)       mod->Set(v8::Integer::New(modc++), v8::String::New("rctrl"));
			if ((event.key.keysym.mod & KMOD_LALT)==KMOD_LALT)         mod->Set(v8::Integer::New(modc++), v8::String::New("lalt"));
			if ((event.key.keysym.mod & KMOD_RALT)==KMOD_RALT)         mod->Set(v8::Integer::New(modc++), v8::String::New("ralt"));
			if ((event.key.keysym.mod & KMOD_LGUI)==KMOD_LGUI)         mod->Set(v8::Integer::New(modc++), v8::String::New("lgui"));
			if ((event.key.keysym.mod & KMOD_RGUI)==KMOD_RGUI)         mod->Set(v8::Integer::New(modc++), v8::String::New("rgui"));
			if ((event.key.keysym.mod & KMOD_NUM)==KMOD_NUM)           mod->Set(v8::Integer::New(modc++), v8::String::New("num"));
			if ((event.key.keysym.mod & KMOD_CAPS)==KMOD_CAPS)         mod->Set(v8::Integer::New(modc++), v8::String::New("caps"));
			if ((event.key.keysym.mod & KMOD_MODE)==KMOD_MODE)         mod->Set(v8::Integer::New(modc++), v8::String::New("mode"));
			if ((event.key.keysym.mod & KMOD_RESERVED)==KMOD_RESERVED) mod->Set(v8::Integer::New(modc++), v8::String::New("reserved"));
			keysym->Set(v8::String::New("mod"), mod);
			key->Set(v8::String::New("keysym"), keysym);
			e->Set(v8::String::New("key"), key);
		}
		break;

	case SDL_MOUSEWHEEL:
		{
			e->Set(v8::String::New("type"), v8::String::New("mousewheel"));
			v8::Handle<v8::Object> wheel=v8::Object::New();
			wheel->Set(v8::String::New("timestamp"), v8::Number::New(event.wheel.timestamp));
			wheel->Set(v8::String::New("windowID"), v8::Number::New(event.wheel.windowID));
			wheel->Set(v8::String::New("x"), v8::Number::New(event.wheel.x));
			wheel->Set(v8::String::New("y"), v8::Number::New(event.wheel.y));
			wheel->Set(v8::String::New("which"), v8::Number::New(event.wheel.which));
#ifdef SDL_MOUSEWHEEL_NORMAL
			wheel->Set(v8::String::New("direction"), v8::Number::New(event.wheel.direction));
#endif
			e->Set(v8::String::New("wheel"), wheel);
		}
		break;

	case SDL_MOUSEBUTTONUP:
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEMOTION:
		{
			switch (event.type) {
			case SDL_MOUSEBUTTONUP:   e->Set(v8::String::New("type"), v8::String::New("mousebuttonup"));   break;
			case SDL_MOUSEBUTTONDOWN: e->Set(v8::String::New("type"), v8::String::New("mousebuttondown")); break;
			case SDL_MOUSEMOTION:     e->Set(v8::String::New("type"), v8::String::New("mousemotion"));     break;
			}
			v8::Handle<v8::Object> motion=v8::Object::New();
			motion->Set(v8::String::New("state"), v8::Number::New(event.motion.state));
			motion->Set(v8::String::New("x"), v8::Number::New(event.motion.x));
			motion->Set(v8::String::New("y"), v8::Number::New(event.motion.y));
			e->Set(v8::String::New("motion"), motion);
			v8::Handle<v8::Object> button=v8::Object::New();
			button->Set(v8::String::New("state"), v8::Number::New(event.button.state));
			button->Set(v8::String::New("button"), v8::Number::New(event.button.button));
			button->Set(v8::String::New("x"), v8::Number::New(event.button.x));
			button->Set(v8::String::New("y"), v8::Number::New(event.button.y));
			e->Set(v8::String::New("button"), button);
		}
		break;

	//case SDL_AUDIODEVICEADDED: // only SDL2
	//case SDL_AUDIODEVICEREMOVED: // only SDL2
	case SDL_CONTROLLERAXISMOTION:
	case SDL_CONTROLLERBUTTONDOWN:
	case SDL_CONTROLLERBUTTONUP:
	case SDL_CONTROLLERDEVICEADDED:
	case SDL_CONTROLLERDEVICEREMOVED:
	case SDL_CONTROLLERDEVICEREMAPPED:
	case SDL_DOLLARGESTURE:
	case SDL_DOLLARRECORD:
	case SDL_DROPFILE:
	//case SDL_DROPTEXT:
	//case SDL_DROPBEGIN:
	//case SDL_DROPCOMPLETE:
	case SDL_FINGERMOTION:
	case SDL_FINGERDOWN:
	case SDL_FINGERUP:
	case SDL_JOYAXISMOTION:
	case SDL_JOYBALLMOTION:
	case SDL_JOYHATMOTION:
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
	case SDL_JOYDEVICEADDED:
	case SDL_JOYDEVICEREMOVED:
	case SDL_MULTIGESTURE:
	case SDL_SYSWMEVENT:
	case SDL_TEXTEDITING:
	case SDL_TEXTINPUT:
	case SDL_USEREVENT:
		e->Set(v8::String::New("type"), v8::Number::New(event.type));
		//e->Set(v8::String::New("type"), v8::Boolean::New(false));
		break;
	}

	return e;
}

// the callback that is invoked by v8 whenever the JavaScript 'echo'
// function is called.  Prints its arguments on stdout separated by
// spaces and ending with a newline.
v8::Handle<v8::Value> LIB::echo(const v8::Arguments& args) {
	bool first=true;
	for (int i=0; i<args.Length(); i++) {
		//v8::HandleScope handle_scope;
		if (first) first=false;
		else printf(" ");
		v8::String::Utf8Value str(args[i]);
		const char* cstr=C::cstr(str);
		printf("%s", cstr);
	}
	fflush(stdout); //************** quizá eliminar en el futuro
	return v8::Undefined();
}

// the callback that is invoked by v8 whenever the JavaScript 'read'
// function is called.  This function loads the content of the file named in
// the argument into a JavaScript string.
v8::Handle<v8::Value> LIB::read(const v8::Arguments& args) {
	if (args.Length() != 1) return v8::ThrowException(v8::String::New("Bad parameters"));
	v8::String::Utf8Value file(args[0]);
	if (*file == NULL) return v8::ThrowException(v8::String::New("Error loading file"));
	v8::Handle<v8::String> source=LIB::readfile(*file);
	if (source.IsEmpty()) return v8::ThrowException(v8::String::New("Error loading file"));
	return source;
}

// the callback that is invoked by v8 whenever the JavaScript 'include'
// function is called.  Loads, compiles and executes its argument JavaScript file.
v8::Handle<v8::Value> LIB::include(const v8::Arguments& args) {
	for (int i=0; i < args.Length(); i++) {
		v8::HandleScope handle_scope;
		v8::String::Utf8Value file(args[i]);
		if (*file == NULL) return v8::ThrowException(v8::String::New("Error loading file"));
		v8::Handle<v8::String> source=LIB::readfile(*file);
		if (source.IsEmpty()) return v8::ThrowException(v8::String::New("Error loading file"));
		if (!LIB::executestring(source, v8::String::New(*file), false, false)) return v8::ThrowException(v8::String::New("Error executing file"));
	}
	return v8::Undefined();
}

// the callback that is invoked by v8 whenever the JavaScript 'exit' function is called. Quits.
v8::Handle<v8::Value> LIB::exit(const v8::Arguments& args) {
	if (args.Length()>0) kernel.result(args[0]->IntegerValue());
	v8::V8::TerminateExecution();
	return v8::Undefined();
}

// returns de V8 version
v8::Handle<v8::Value> LIB::version(const v8::Arguments& args) {
	return v8::String::New(v8::V8::GetVersion());
}

// reads a file into a v8 string
v8::Handle<v8::String> LIB::readfile(const char* name) {
	FILE* file=fopen(name, "rb");
	if (file == NULL) return v8::Handle<v8::String>();
	fseek(file, 0, SEEK_END);
	int size=ftell(file);
	rewind(file);
	char* chars=new char[size + 1];
	chars[size]='\0';
	for (int i=0; i < size;) {
		int read=static_cast<int>(fread(&chars[i], 1, size - i, file));
		i += read;
	}
	fclose(file);
	v8::Handle<v8::String> result=v8::String::New(chars, size);
	delete[] chars;
	return result;
}

// Executes a string within the current v8 context.
bool LIB::executestring(
	v8::Handle<v8::String> source,
	v8::Handle<v8::Value> name,
	bool print_result,
	bool report_exceptions
) {
	v8::HandleScope handle_scope;
	v8::TryCatch try_catch;
	v8::Handle<v8::Script> script=v8::Script::Compile(source, name);
	if (script.IsEmpty()) {
		// Print errors that happened during compilation.
		if (report_exceptions) LIB::reportexception(&try_catch);
		return false;
	} else {
		v8::Handle<v8::Value> result=script->Run();
		if (result.IsEmpty()) {
			if (!kernel.result()) return true;
			assert(try_catch.HasCaught());
			// Print errors that happened during execution.
			if (report_exceptions) LIB::reportexception(&try_catch);
			return false;
		} else {
			assert(!try_catch.HasCaught());
			if (print_result && !result->IsUndefined()) {
				// If all went well and the result wasn't undefined then print the returned value
				v8::String::Utf8Value str(result);
				const char* cstr=C::cstr(str);
				printf("%s\n", cstr);
			}
			return true;
		}
	}
}

// lanzar excepción
void LIB::reportexception(v8::TryCatch* try_catch) {
	v8::HandleScope handle_scope;
	v8::String::Utf8Value exception(try_catch->Exception());
	const char* exception_string=C::cstr(exception);
	v8::Handle<v8::Message> message=try_catch->Message();
	if (message.IsEmpty()) {
		// V8 didn't provide any extra information about this error; just print the exception
		printf("%s\n", exception_string);
	} else {
		// print (filename):(line number): (message)
		v8::String::Utf8Value filename(message->GetScriptResourceName());
		const char* filename_string=C::cstr(filename);
		int linenum=message->GetLineNumber();
		printf("[V8] %s:%i\n", filename_string, linenum); // , exception_string
		// print line of source code.
		v8::String::Utf8Value sourceline(message->GetSourceLine());
		const char* sourceline_string=C::cstr(sourceline);
		printf("%s\n", sourceline_string);
		// print wavy underline (GetUnderline is deprecated)
		int start=message->GetStartColumn();
		for (int i=0; i<start; i++)
			printf(sourceline_string[i]==9?"\t":" ");
		int end=message->GetEndColumn();
		for (int i=start; i<end; i++)
			printf("^");
		printf("\n");
		v8::String::Utf8Value stack_trace(try_catch->StackTrace());
		if (stack_trace.length() > 0) {
			const char* stack_trace_string=C::cstr(stack_trace);
			printf("%s\n", stack_trace_string);
		}
	}
}
