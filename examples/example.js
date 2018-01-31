var app={

	"title":"V8+SDL concept app",

	"textureLoad":function(filename) {
		var surface, texture;
		if (!(surface=lib.gdi.loadImage(filename))) return false;
		texture=lib.gdi.createTextureFromSurface(app.renderer, surface);
		lib.gdi.freeSurface(surface);
		return texture;
	},

	"unload":function(){
		if (app.win) lib.gdi.destroyWindow(app.win);
		lib.exit(0);
	},

	"init":function(){
		lib.echo("v8.version="+lib.version()+"\n");
		// crear ventana y renderizador principal
		app.win=lib.gdi.window(app.title, 500, 200, 600, 300, 0);
		//lib.gdi.windowTitle(app.win, "JSDL");
		//lib.echo(lib.gdi.windowTitle(app.win));
		lib.echo("win="+app.win+"\n");
		app.renderer=lib.gdi.createRenderer(app.win, -1, 0);
		lib.echo("renderer="+app.renderer+"\n");
	}

};

// inicializar núcleo
if (lib && lib.kernel && lib.kernel.init()) {

	// inicializar aplicación
	app.init();

	// cargar datos
	var font=lib.gdi.fontOpen("font.ttf", 24);
	var background=app.textureLoad("background.png");
	var tux=app.textureLoad("tux.png");

	//var win2=lib.gdi.window("ventana2", 400, 100, 300, 200, 0);
	
	// bucle principal
	var working=true;
	var start=new Date();
	var frames=0;
	while (working) {

		frames++;
		var fps=Math.round(frames*1000/((new Date())-start));
		if (!(frames%1000)) lib.gdi.windowTitle(app.win, "pruebas setTitle: "+fps+"fps - "+frames);

		lib.gdi.renderClear(app.renderer);

		lib.gdi.renderTexture(app.renderer, background, 0, 0);

		for (var i=0;i<10;i++)
			lib.gdi.renderTexture(app.renderer, tux, Math.random()*600, Math.random()*300);

		var surface_text=lib.gdi.text(font, fps+"fps", [225,0,255,205]);
		var texture_text=lib.gdi.createTextureFromSurface(app.renderer, surface_text);
		lib.gdi.renderTexture(app.renderer, texture_text, 10, 10);
		lib.gdi.destroyTexture(texture_text);
		lib.gdi.freeSurface(surface_text);

		lib.gdi.renderPresent(app.renderer);

		var event=lib.gdi.pollEvent();
		//if (event.type) lib.echo("event="+JSON.stringify(event)+"\n");
		switch (event.type) {
		case "quit": working=false; break;
		case "keyup":
			//if (event.key.keysym.sym==100) lib.gdi.destroyWindow(win2);
			if (event.key.keysym.sym==27) working=false;
			break;
		case "mousebuttonup":
		case "mousebuttondown":
		case "mousemotion":
			break;
		default:
			if (event.type) lib.echo("event="+JSON.stringify(event)+"\n");
		}
		
		//lib.gdi.delay(fps>=120?fps/(1000/120):0);

	}

}

// terminar aplicación
app.unload();
