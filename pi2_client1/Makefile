obj-m := ultrasonic.o

KERNELDIR := ~/linux-rpi/
ARM := ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $(ARM) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $(ARM) clean
	
