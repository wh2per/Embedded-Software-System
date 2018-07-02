#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <string.h>


unsigned int get_value(unsigned int adcChannel){
	int dev = 0;
	unsigned char buff[3];
	int adcValue = 0;
	
	dev = open("/dev/spidev0.0", O_RDWR);
	if (dev < 0)
	{
		printf("Open failed");
		return -1;
	}

	struct spi_ioc_transfer tfUnit;
	memset (&tfUnit, 0, sizeof(tfUnit));

	adcChannel &= 1;

	buff[0] = 0x06 | ((adcChannel & 0x07) >> 2);
	buff[1] = ((adcChannel & 0x07) << 6);
	buff[2] = 0x00;

	tfUnit.tx_buf = (unsigned long)buff;
	tfUnit.rx_buf = (unsigned long)buff;
	tfUnit.len = 3;
	tfUnit.delay_usecs = 0;
	tfUnit.speed_hz = 2000000;
	tfUnit.bits_per_word = 8;

	adcValue = ioctl(dev, SPI_IOC_MESSAGE(1), &tfUnit);
	
	buff[1] = 0x0f & buff[1];
	adcValue = (buff[1] << 8) | buff[2];

	close(dev);

	if(adcValue < 1000)
	{
		return adcValue;
	}

	return 0;
}	
