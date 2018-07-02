// ultrasonic_lib.c
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEV_NAME "simple_ultrasonic_dev" // ultrasonic 커널모듈 이름

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1

#define SIMPLE_ULTRASONIC_IOCTL_NUM 'z'
#define GET_ULTRASONIC_DISTANCE _IOWR(SIMPLE_ULTRASONIC_IOCTL_NUM, IOCTL_NUM1, unsigned long *)

long int get_ultrasonic_distance()
{
    long int ultrasonic_distance;

    int dev = open("/dev/simple_ultrasonic_dev", O_RDWR); 

    ioctl(dev, GET_ULTRASONIC_DISTANCE, &ultrasonic_distance);

    close(dev);

    return ultrasonic_distance;
}

int main(int argc, char* argv[])
{
    long int ultrasonic_distance = get_ultrasonic_distance();

    printf("get_ultrasonic_distance : %ld \n", ultrasonic_distance);

    return 0;
}
