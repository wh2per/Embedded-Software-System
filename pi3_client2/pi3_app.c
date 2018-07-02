#include <stdio.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "pi3.h"

int pi3_turnOn(){
	int dev;
	int cmd;
	unsigned long value=0;

	dev = open("/dev/pi3_dev", O_RDWR);
	
	ioctl(dev, PI3_LEDON, &value);
	
	close(dev);

	return 0;
}

int pi3_turnOff(){
	int dev;
	int cmd;
	unsigned long value=0;

	dev = open("/dev/pi3_dev", O_RDWR);
	
	ioctl(dev, PI3_LEDOFF, &value);
	
	close(dev);

	return 0;
}

int pi3_doorOpen(){
	int dev;
	int cmd;
	unsigned long value=0;

	dev = open("/dev/pi3_dev", O_RDWR);
	
	ioctl(dev, PI3_DOOROPEN, &value);
	
	close(dev);

	return 0;
}

int pi3_doorClose(){
	int dev;
	int cmd;
	unsigned long value=0;

	dev = open("/dev/pi3_dev", O_RDWR);
	
	ioctl(dev, PI3_DOORCLOSE, &value);
	
	close(dev);

	return 0;
}

int pi3_speakerOn(){
	int dev;
	int cmd;
	unsigned long value=0;

	dev = open("/dev/pi3_dev", O_RDWR);
	
	ioctl(dev, PI3_SPEAKER, &value);
	
	close(dev);

	return 0;
}

int pi3_doorSpeakerOn(){
	int dev;
	int cmd;
	unsigned long value=0;

	dev = open("/dev/pi3_dev", O_RDWR);
	
	ioctl(dev, PI3_DOORSPEAKER, &value);
	
	close(dev);

	return 0;
}


int main(void){
	pid_t pid;
	pid_t end;
	int status;

	pi3_turnOn();
	//pi3_speakerOn();
	//pi3_doorClose();
	//pi3_doorOpen();

	pi3_doorSpeakerOn();
	pi3_turnOff();
	return 0;
}