#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include "ADXL345.h"

//#define ADXL345_I2C_ADDR 0x53

// float xa_pre = 0;
//float xa_f_pre = 0;
// float ya_pre = 0;
// float ya_f_pre = 0;
// float za_pre = 0;
// float za_f_pre = 0;
float xa_f,ya_f,za_f;
void selectDevice(int fd, int addr, char * name)
    {
       if (ioctl(fd, I2C_SLAVE, addr) < 0)
       {
          fprintf(stderr, "%s not present\n", name);
       }
    }

void writeToDevice(int fd, int reg, int val)
    {
       char buf[2];
       buf[0]=reg; buf[1]=val;
       if (write(fd, buf, 2) != 2)
       {
          fprintf(stderr, "Can't write to device\n");
       }
    }
void writeToDeviceForRead(int fd, int reg)
    {
       char buf[1];
       buf[0]=reg;
       if (write(fd, buf, 1) != 1)
       {
          fprintf(stderr, "Can't write to device\n");
       }
    }

void readFromDevice(int fd, int reg)
    {
	writeToDeviceForRead(fd,reg);
       char buf[1];
	printf("Reading from reg %i",(uint)reg);
       buf[0]=reg+27; 
       if (read(fd, buf, 1) != 1)
       {
          fprintf(stderr, "Can't write to device\n");
       }else{
	
	//printf("asdlfkjasldfjlksd\n");
	//printf("Read %i from buf\n",(uint)buf[0]);
	
	}
    }

void adxl_init(int *fd, int addr) //pointer??
{
	/* initialise ADXL345 */
	if ((*fd = open("/dev/i2c-1", O_RDWR)) < 0)
       {
          // Open port for reading and writing

          fprintf(stderr, "Failed to open i2c bus\n");

          exit(1);
       }

       selectDevice(*fd, addr, "ADXL345");

       //writeToDevice(fd, 0x2d, 0);
       //writeToDevice(fd, 0x2d, 16);
       writeToDevice(*fd, 0x2d, 56);

       writeToDevice(*fd, 0x2c, 11);
       //writeToDevice(fd, 0x31, 0);
       writeToDevice(*fd, 0x31, 11);
       readFromDevice(*fd,0x2d);
       return;
}

void adxl_read(int fd, int addr, int devNum, float *xa_f_pre, float *xa_pre, float *ya_f_pre, float *ya_pre , float *za_f_pre, float *za_pre)
    {
       unsigned int range;
       int count, b;
       short x, y, z;
       float xa, ya, za;
       //int fd;
       unsigned char buf[16];
       

          /* select ADXL345 */

          selectDevice(fd, addr, "ADXL345");

          buf[0] = 0x32;
       
          if ((write(fd, buf, 1)) != 1)
          {
             // Send the register to read from

             fprintf(stderr, "Error writing to i2c slave\n");
          }
       
          if (read(fd, buf, 6) != 6)
          {
             //  X, Y, Z accelerations

             fprintf(stderr, "Unable to read from ADXL345\n");
          }
          else
          {
             //printf("Accel #%d, X=%f Y=%f Z=%f\n",devNum, *xa_f_pre, *ya_f_pre, *za_f_pre);

             x = buf[1]<<8| buf[0];
             y = buf[3]<<8| buf[2];
             z = buf[5]<<8| buf[4];
             xa = (90.0 / 2350) * (float) x;
             ya = (90.0 / 2350) * (float) y;
             za = (90.0 / 2350) * (float) z;
             
             //DC block
             float RC = 1.0/(0.3*2*3.1415);
             float dt = 1.0/200;
             float alpha = RC/(RC+dt);
             
             xa_f = alpha*(*xa_f_pre + xa - *xa_pre);
             *xa_f_pre = xa_f;
	         *xa_pre = xa;
             
             ya_f = alpha*(*ya_f_pre + ya - *ya_pre);
             *ya_f_pre = ya_f;
	         *ya_pre = ya;
             
             za_f = alpha*(*za_f_pre + za - *za_pre);
             *za_f_pre = za_f;
             *za_pre = za;
             

             printf("Accel fval #%d, X=%f Y=%f Z=%f\n",devNum, xa_f, ya_f, za_f);
             printf("Accel #%d, X=%f Y=%f Z=%f\n",devNum, xa, ya, za);
            // printf("Accel #%d, X=%f Y=%f Z=%f\n",devNum, *xa_f_pre, *ya_f_pre, *za_f_pre);
             
          }
          return;
    }
