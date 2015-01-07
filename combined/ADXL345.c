#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <curl/curl.h>
#include "ADXL345.h"

//#define NZEROS 6
//#define NPOLES 6
//#define GAIN   17.67057741
//#define GAIN   0.

//#define ADXL345_I2C_ADDR 0x53

// float xa_pre = 0;
//float xa_f_pre = 0;
// float ya_pre = 0;
// float ya_f_pre = 0;
// float za_pre = 0;
// float za_f_pre = 0;

float xa_f,ya_f,za_f,xAVG,yAVG,zAVG;
float sampleCount =  0;
char *servIP = "10.20.29.112";

//static float Xxv[NZEROS+1], Xyv[NPOLES+1];
//static float Yxv[NZEROS+1], Yyv[NPOLES+1];
//static float Zxv[NZEROS+1], Zyv[NPOLES+1];

void httpPost(char* label, float val, char *servIP)
    {
    	char tmp[200]; //= (char*)malloc(50*sizeof(char));
    	//std::cout << "fuuuu" << std::endl;
    	
    	//sprintf(tmp,"curl --data test=%f http://%s/?test=%f&test2=999",val,servIP,val);
    	sprintf(tmp,"curl --data test=%f http://%s/?%s,%f",val,servIP,label,val);
    	//printf(tmp,"curl --data test=%f http://%s/?%s,%f",val,servIP,label,val);
    	//sprintf(temp,"curl --data param1=10 http://10.20.17.167");
    	//while(1)
    	//{
		if(system(tmp)<0)
		{
			std::cout << "http Post failed" << std::endl;
		}
			//std::cout << "end of http post function" << std::endl;
		//}
		//free(tmp);
		return;
    }

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
      // readFromDevice(*fd,0x2d);
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
       char strx[50];
       char stry[50];
       char strz[50];
       sprintf(strx,"Acc%d-X",devNum);
       sprintf(stry,"Acc%d-Y",devNum);
       sprintf(strz,"Acc%d-Z",devNum);
       //char blah = "10.24.34234.23424";
       

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
             
             if((xa<0.15)&&(xa>-0.15))
             {
             	xa=0;
             }
             if((ya<0.15)&&(ya>-0.15))
             {
             	ya=0;
             }
             if((za<0.15)&&(za>-0.15))
             {
             	za=0;
             }
             if((xa-*xa_pre)<0.1 &&(xa-*xa_pre)>-0.1)
             {
             	xa = *xa_pre;
             }
             if((za-*za_pre)<0.1 &&(za-*za_pre)>-0.1)
             {
             	za = *za_pre;
             }
             if((ya-*ya_pre)<0.1 &&(ya-*ya_pre)>-0.1)
             {
             	ya = *ya_pre;
             }
             
             //DC block
             float RC = 1.0/(0.5*2*3.1415);
             float dt = 1.0/100;
             float alpha = RC/(RC+dt);
             
             xa_f = alpha*(*xa_f_pre + xa - *xa_pre);
             /*
		*xa_f_pre = xa_f;
		*xa_pre = xa;*/
             
             ya_f = alpha*(*ya_f_pre + ya - *ya_pre);
             /*
		*ya_f_pre = ya_f;
		*ya_pre = ya;*/
             
             za_f = alpha*(*za_f_pre + za - *za_pre);
             /*
		*za_f_pre = za_f;
		*za_pre = za;*/
             /*
             if((xa_f<0.15)&&(xa_f>-0.15))
             {
             	xa_f=0;
             }
             if((ya_f<0.15)&&(ya_f>-0.15))
             {
             	ya_f=0;
             }
             if((za_f<0.15)&&(za_f>-0.15))
             {
             	za_f=0;
             }
             */
             *xa_f_pre = xa_f;
             *xa_pre = xa;
             *ya_f_pre = ya_f;
             *ya_pre = ya;
             *za_f_pre = za_f;
             *za_pre = za;
         //calculates DC figure of merit    
	     xAVG = (xAVG*sampleCount + xa_f)/(sampleCount+1);
	     yAVG = (yAVG*sampleCount + ya_f)/(sampleCount+1);
	     zAVG = (zAVG*sampleCount + za_f)/(sampleCount+1);
	     sampleCount++;
	     
	     
	     httpPost(strx,xa_f, servIP);
	     //usleep(10000);
	     httpPost(stry,ya_f, servIP);
	     //usleep(10000);
	     httpPost(strz,za_f, servIP);
	     //usleep(10000);
		
	     printf("Count=%f\n",sampleCount);
	
	     printf("Accel DC, X=%f Y=%f Z=%f\n",xAVG, yAVG, zAVG);
            printf("Accel fval #%d: X=%f Y=%f Z=%f\n",devNum, xa_f, ya_f, za_f);

             //printf("Accel fval #%d, X=%f Y=%f Z=%f\n",devNum, Xyv[6], Yyv[6], Zyv[6]);
            printf("Accel #%d, X=%f Y=%f Z=%f\n",devNum, xa, ya, za);
            // printf("Accel #%d, X=%f Y=%f Z=%f\n",devNum, *xa_f_pre, *ya_f_pre, *za_f_pre);
                
          }
          
          return;
    }
