#ifndef ADXL345_H
	#define ADXL345_H

#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string>
#include <iostream>
 

    	void selectDevice(int fd, int addr, char * name);
	void writeToDevice(int fd, int reg, int val);
	void writeToDeviceForRead(int fd, int reg);
        void readFromDevice(int fd, int reg);
	void adxl_init(int *fd, int addr);
	void adxl_read(int fd, int addr,int devNum);

#endif
