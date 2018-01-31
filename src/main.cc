#include "kernel.h"

#ifdef COMPRESS_STARTUP_DATA_BZ2
#error Using compressed startup data is not supported for this app.
#endif


// application
int main(int argc, char* argv[]) {

	// startup script
	std::string script=getWithoutExtension(argv[0])+".js";

	// V8 command line parameters
	bool processV8settings=true;
	for (int i=1; i<argc; i++) if (strlen(argv[i])) {
		if (strcmp(argv[i], "--help")==0) {
			processV8settings=false;
			break;
		} else if (strcmp(argv[i], "--v8help")==0) {
			argv[i]=strdup("--help");
			break;
		}
	}
	if (processV8settings) v8::V8::SetFlagsFromCommandLine(&argc, argv, true);

	// internal command line parameters
	for (int i=1; i<argc; i++) if (strlen(argv[i])) {
		if (argv[i][0]=='-') {
			if (strcmp(argv[i], "-h")==0 || strcmp(argv[i], "--help")==0) {
				printf("JavaScript V8 engine with multimedia bindings.\n");
				printf("Sintax: %s [options] [script]\n", argv[0]);
				printf("  -h   Show this help\n");
				return 0;
			}
		} else {
			script=argv[i];
			break;
		}
	}

	// start kernel
	kernel.init();

	// save parámeters
	kernel.setargs(argc, argv);

	// start V8 engine
	{

		// initialize context
		v8::HandleScope handle_scope;
		v8::Persistent<v8::Context> context=LIB::createShellContext();
		if (context.IsEmpty()) {
			printf("Error creating context\n");
			return 1;
		}

		// enter context
		context->Enter();

		// script name (file name)
		v8::Handle<v8::String> script_name=v8::String::New(basename(script).c_str());

		// load script source
		v8::Handle<v8::String> source=LIB::readfile(script.c_str());
		if (source.IsEmpty()) {
			printf("Error loading '%s'\n", script.c_str());
			return 1;
		}

		// set working directory to the script base
		if (dirname(script)!="") chdir(dirname(script).c_str());

		// execute script
		if (!LIB::executestring(source, script_name, false, true)) {
			printf("Error executing '%s'\n", script.c_str());
			return 1;
		}

		// exit context and free memory
		context->Exit();
		context.Dispose();

	}

	// unload kernel
	kernel.unload();

	// free V8 memory
	v8::V8::Dispose();

	// everything went fine
	return kernel.result();

}
