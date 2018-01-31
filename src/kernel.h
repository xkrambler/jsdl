#ifndef __KERNEL_H
#define __KERNEL_H

#include "common.h"
#include "files.h"
#include "lib.h"
#include "serial.h"

// control de gráficos
class GDI {
public:

	GDI(){
		this->error="";
	}

	// inicializar
	bool init(){
		// inicializar librerías
		if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
			printf("Failed to initialize SDL: %s\n", SDL_GetError());
			return this->err("Failed SDL");
		}
		if (TTF_Init() == -1) {
			printf("Failed to initialize TTF: %s\n", SDL_GetError());
			return this->err("Failed TTF");
		}
		SDL_ShowCursor(SDL_DISABLE);
		// todo OK
		return true;
	}

	// terminar
	bool unload(){
		SDL_Quit();
		return true;
	}

	string err(){
		return this->error;
	}

	bool err(string error){
		this->error=error;
		return false;
	}

	string error;

};


// núcleo
class Kernel {
public:

	GDI gdi;
	string error;
	string version;

	Kernel(){
		this->version="0.1";
		this->error="";
		this->result_code=1;
	}

	~Kernel(){
		//delete this->gdi;
	}

	bool init(){
		//printf("kernel=%s\n", this->version.c_str());
		if (!this->gdi.init()) this->err(this->gdi.err());
		return true;
	}

	int argc() {
		return this->arg_c;
	}

	char* argv(int num) {
		if (num>=this->arg_c) return NULL;
		return this->arg_v[num];
	}

	void setargs(int _argc, char* _argv[]) {
		this->arg_c=_argc;
		this->arg_v=_argv;
	}

	void unload() {
		this->gdi.unload();
		fflush(stdout);
		fflush(stderr);
	}

	int result() {
		return this->result_code;
	}

	void result(int result_code) {
		this->result_code=result_code;
	}

	int err(){
		printf("%s\n", this->error.c_str());
		return 1;
	}

	bool err(string error){
		this->error=error;
		return false;
	}

private:
	int result_code;
	int arg_c;
	char** arg_v;

};

// necesitamos conocer el kernel
extern Kernel kernel;

#endif
