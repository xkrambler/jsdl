#ifndef __SERIAL_H
#define __SERIAL_H

//#include <errno.h>
//#include <fcntl.h> 
//#include <string.h>
#include <termios.h>
#include <thread>

class Serial {

public:

	Serial(std::string deviceName){
		this->device=deviceName;
		this->bufferSize=1024;
		this->fd=0;
	}

	~Serial() {
		this->close();
	}

	int set_interface_attribs(int fd, int speed, int parity) {

		struct termios tty;
		memset (&tty, 0, sizeof tty);
		if (tcgetattr(fd, &tty) != 0) {
			printf("error %d from tcgetattr\n", errno);
			return -1;
		}

		cfsetospeed(&tty, speed);
		cfsetispeed(&tty, speed);

		tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
		// disable IGNBRK for mismatched speed tests; otherwise receive break as \000 chars
		tty.c_iflag &= ~IGNBRK;         // disable break processing
		tty.c_lflag = 0;                // no signaling chars, no echo,
		// no canonical processing
		tty.c_oflag = 0;                // no remapping, no delays
		tty.c_cc[VMIN]  = 0;            // read doesn't block
		//tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
		tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
		tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
		tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
		// enable reading
		tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
		tty.c_cflag |= parity;
		tty.c_cflag &= ~CSTOPB;
		tty.c_cflag &= ~CRTSCTS;

		if (tcsetattr(fd, TCSANOW, &tty) != 0) {
			printf("error %d from tcsetattr\n", errno);
			return -1;
		}

		return 0;
	}

	void set_blocking(int fd, bool should_block) {
		struct termios tty;
		memset(&tty, 0, sizeof tty);
		if (tcgetattr(fd, &tty) != 0) {
			printf("error %d from tggetattr\n", errno);
			return;
		}
		tty.c_cc[VMIN]  = should_block ? 1 : 0;
		tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
		if (tcsetattr(fd, TCSANOW, &tty) != 0)
			printf("error %d setting term attributes\n", errno);
	}

	bool open() {
		//int ret=system("/bin/stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parenb ignbrk -brkint -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts");
		//printf("stty_ret=%d\n", ret);
		this->err="";
		this->fd=::open(this->device.c_str(), O_RDWR | O_NOCTTY | O_SYNC); // O_NONBLOCK O_NDELAY
		if (this->fd < 0) {
			std::ostringstream ss;
			ss << "Serial: Error " << errno << " opening " << this->device << ": " << strerror(errno);
			this->err=ss.str();
			return false;
		}
		this->set_interface_attribs(this->fd, B115200, 0); // set speed to 115,200 bps, no parity
		this->set_blocking(this->fd, true);               // set no blocking
		this->readed="";
		this->readn=0;
		this->readthread=std::thread(&Serial::asyncread, this);
		return true;
	}

	void asyncread() {
		while (this->fd > 0) {
			char buf[this->bufferSize];
			int n=::read(this->fd, buf, sizeof buf);
			//printf("******Async (%d)\n", n);
			if (n>0) {
				buf[n]=0;
				this->readed=this->readed+buf;
				this->readn=this->readed.size();
			} else {
				this->readn=n;
				return;
			}
		}
	}

	int read() {
		return this->readn;
		/*this->readed="";
		char buf[this->bufferSize];
		int n=::read(this->fd, buf, sizeof buf);  // read up to 100 characters if ready to read
		if (n>=0) {
			buf[n]=0;
			this->readed=buf;
		}
		return n;*/
	}

	std::string data() {
		string r=this->readed;
		this->readed="";
		return r;
	}

	int write(string buf) {
		return ::write(this->fd, buf.c_str(), buf.size());
	}

	void close() {
		if (!this->fd) return;
		int fd=this->fd;
		this->fd=0;
		this->readthread.join();
		::close(fd);
	}

	string error() {
		return this->err;
	}

private:

	int bufferSize;
	int fd;
	int readn;
	std::string device;
	std::string readed;
	std::thread readthread;
	std::string err;

};

#endif
