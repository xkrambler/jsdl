// startup
if (lib) {

	var device="/dev/tty2"; // /dev/ttyUSB0
	var serial;
	if (serial=lib.serial.init(device)) {
		lib.echo("Connecting to "+device+"\n");
		if (lib.serial.open(serial)) {
			lib.echo("Connected successfully, waiting for data.\n");
			if (!lib.serial.write(serial, "PRUEBAAAAAAAAAAAA")) lib.echo("Cannot write!\n");
			while (true) {
				var n=lib.serial.read(serial);
				if (n<0) break; // connection closed
				lib.echo("SERIAL("+n+")["+lib.serial.data(serial)+"]\n");
				if (n==0) lib.gdi.delay(10);
			}
			lib.serial.close(serial);
			lib.echo("Connection closed\n");
		} else {
			lib.echo("Cannot open device.\n");
		}
	} else {
		lib.echo("Cannot initialize device.");
	}

	// finish
	lib.exit(0);

}
