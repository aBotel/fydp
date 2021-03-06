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


//static float Xxv[NZEROS+1], Xyv[NPOLES+1];
//static float Yxv[NZEROS+1], Yyv[NPOLES+1];
//static float Zxv[NZEROS+1], Zyv[NPOLES+1];

void httpPost(float val, char *servIP)
    {
    	char tmp[50]; //= (char*)malloc(50*sizeof(char));
    	std::cout << "fuuuu" << std::endl;
    	sprintf(tmp,"curl --data test=%f http://%s/?test=%f",val,servIP,val);
    	//sprintf(temp,"curl --data param1=10 http://10.20.17.167");
		if(system(tmp)<0)
		{
			std::cout << "http Post failed" << std::endl;
		}
		std::cout << "fuuuu22222" << std::endl;
		std::cout << "fuuuu22222" << std::endl;
		std::cout << "fuuuu22222" << std::endl;
		std::cout << "fuuuu22222" << std::endl;
		//free(tmp);
		//return;
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
       //char blah = "10.24.34234.23424";
       char *servIP = "10.20.194.199";

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
             
             if((xa<0.08)&&(xa>-0.08))
             {
             	xa=0;
             }
             if((ya<0.08)&&(ya>-0.08))
             {
             	ya=0;
             }
             if((za<0.08)&&(za>-0.08))
             {
             	za=0;
             }
             if((xa-*xa_pre)<0.05 &&(xa-*xa_pre)>-0.05)
             {
             	xa = *xa_pre;
             }
             if((za-*za_pre)<0.05 &&(za-*za_pre)>-0.05)
             {
             	za = *za_pre;
             }
             if((ya-*ya_pre)<0.05 &&(ya-*ya_pre)>-0.05)
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
             
             if((xa_f<0.1)&&(xa_f>-0.1))
             {
             	xa_f=0;
             }
             if((ya_f<0.1)&&(ya_f>-0.1))
             {
             	ya_f=0;
             }
             if((za_f<0.1)&&(za_f>-0.1))
             {
             	za_f=0;
             }
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
	     
	     
	     httpPost(xa_f, servIP);
	     //httpPost(ya_f, servIP);
	     //httpPost(za_f, servIP);

/*
			//---------------------------------------------------------------------------------------------------
		    Xxv[0] = Xxv[1]; Xxv[1] = Xxv[2]; Xxv[2] = Xxv[3]; Xxv[3] = Xxv[4]; Xxv[4] = Xxv[5]; Xxv[5] = Xxv[6]; 
			Xxv[6] = xa / GAIN;
			Xyv[0] = Xyv[1]; Xyv[1] = Xyv[2]; Xyv[2] = Xyv[3]; Xyv[3] = Xyv[4]; Xyv[4] = Xyv[5]; Xyv[5] = Xyv[6]; 
			Xyv[6] =   (Xxv[0] + Xxv[6]) - 6 * (Xxv[1] + Xxv[5]) + 15 * (Xxv[2] + Xxv[4])
				         - 20 * Xxv[3]
				         + ( -0.9989894086 * Xyv[0]) + (  5.9885625009 * Xyv[1])
				         + (-14.9643603700 * Xyv[2]) + ( 19.9515941410 * Xyv[3])
				         + (-14.9694101360 * Xyv[4]) + (  5.992 * Xyv[5]);
			//*xa_pre = Xyv[6];
			//
			Yxv[0] = Yxv[1]; Yxv[1] = Yxv[2]; Yxv[2] = Yxv[3]; Yxv[3] = Yxv[4]; Yxv[4] = Yxv[5]; Yxv[5] = Yxv[6]; 
			Yxv[6] = ya / GAIN;
			Yyv[0] = Yyv[1]; Yyv[1] = Yyv[2]; Yyv[2] = Yyv[3]; Yyv[3] = Yyv[4]; Yyv[4] = Yyv[5]; Yyv[5] = Yyv[6]; 
			Yyv[6] =   (Yxv[0] + Yxv[6]) - 6 * (Yxv[1] + Yxv[5]) + 15 * (Yxv[2] + Yxv[4])
				         - 20 * Yxv[3]
				         + ( -0.9997469782 * Yyv[0]) + (  5.9983352695 * Yyv[1])
				         + (-14.9958712950 * Yyv[2]) + ( 19.9950720260 * Yyv[3])
				         + (-14.9971363540 * Yyv[4]) + (  5.9993473319 * Yyv[5]);
			//*ya_pre = Yyv[6];
			//
			Zxv[0] = Zxv[1]; Zxv[1] = Zxv[2]; Zxv[2] = Zxv[3]; Zxv[3] = Zxv[4]; Zxv[4] = Zxv[5]; Zxv[5] = Zxv[6]; 
			Zxv[6] = za / GAIN;
			Zyv[0] = Zyv[1]; Zyv[1] = Zyv[2]; Zyv[2] = Zyv[3]; Zyv[3] = Zyv[4]; Zyv[4] = Zyv[5]; Zyv[5] = Zyv[6]; 
			Zyv[6] =   (Zxv[0] + Zxv[6]) - 6 * (Zxv[1] + Zxv[5]) + 15 * (Zxv[2] + Zxv[4])
				         - 20 * Zxv[3]
				         + ( -0.9997469782 * Zyv[0]) + (  5.9983352695 * Zyv[1])
				         + (-14.9958712950 * Zyv[2]) + ( 19.9950720260 * Zyv[3])
				         + (-14.9971363540 * Zyv[4]) + (  5.9993473319 * Zyv[5]);
			//*za_pre = Zyv[6];
	
			
*/
/*
Xxv[0] = Xxv[1]; Xxv[1] = Xxv[2]; Xxv[2] = Xxv[3]; Xxv[3] = Xxv[4]; Xxv[4] = Xxv[5]; Xxv[5] = Xxv[6];
 Xxv[6] = xa*1000 / GAIN;
 Xyv[0] = Xyv[1]; Xyv[1] = Xyv[2]; Xyv[2] = Xyv[3]; Xyv[3] = Xyv[4]; Xyv[4] = Xyv[5]; Xyv[5] = Xyv[6];
 Xyv[6] = (Xxv[6] - Xxv[0]) + 3 * (Xxv[2] - Xxv[4]) + ( -0.1211839985 * Xyv[0]) + ( 0.9702069152 * Xyv[1]) + ( -3.1780877771 * Xyv[2]) + ( 5.8755616446 * Xyv[3]) + ( -6.4891985319 * Xyv[4]) + ( 3.9426271869 * Xyv[5]);
 *xa_pre = Xyv[6];

Yxv[0] = Yxv[1]; Yxv[1] = Yxv[2]; Yxv[2] = Yxv[3]; Yxv[3] = Yxv[4]; Yxv[4] = Yxv[5]; Yxv[5] = Yxv[6];
 Yxv[6] = ya*1000 / GAIN;
 Yyv[0] = Yyv[1]; Yyv[1] = Yyv[2]; Yyv[2] = Yyv[3]; Yyv[3] = Yyv[4]; Yyv[4] = Yyv[5]; Yyv[5] = Yyv[6];
 Yyv[6] = (Yxv[6] - Yxv[0]) + 3 * (Yxv[2] - Yxv[4]) + ( -0.1211839985 * Yyv[0]) + ( 0.9702069152 * Yyv[1]) + ( -3.1780877771 * Yyv[2]) + ( 5.8755616446 * Yyv[3]) + ( -6.4891985319 * Yyv[4]) + ( 3.9426271869 * Yyv[5]);
 *ya_pre = Yyv[6];

Zxv[0] = Zxv[1]; Zxv[1] = Zxv[2]; Zxv[2] = Zxv[3]; Zxv[3] = Zxv[4]; Zxv[4] = Zxv[5]; Zxv[5] = Zxv[6];
 Zxv[6] = za*1000 / GAIN;
 Zyv[0] = Zyv[1]; Zyv[1] = Zyv[2]; Zyv[2] = Zyv[3]; Zyv[3] = Zyv[4]; Zyv[4] = Zyv[5]; Zyv[5] = Zyv[6];
 Zyv[6] = (Zxv[6] - Zxv[0]) + 3 * (Zxv[2] - Zxv[4]) + ( -0.1211839985 * Zyv[0]) + ( 0.9702069152 * Zyv[1]) + ( -3.1780877771 * Zyv[2]) + ( 5.8755616446 * Zyv[3]) + ( -6.4891985319 * Zyv[4]) + ( 3.9426271869 * Zyv[5]);
 *za_pre = Zyv[6];	*/		
		
	
	//if(sampleCount == 20000){
	printf("Count=%f\n",sampleCount);
	//}	
	    printf("Accel DC, X=%f Y=%f Z=%f\n",xAVG, yAVG, zAVG);
            printf("Accel fval #%d: X=%f Y=%f Z=%f\n",devNum, xa_f, ya_f, za_f);

             //printf("Accel fval #%d, X=%f Y=%f Z=%f\n",devNum, Xyv[6], Yyv[6], Zyv[6]);
            printf("Accel #%d, X=%f Y=%f Z=%f\n",devNum, xa, ya, za);
            // printf("Accel #%d, X=%f Y=%f Z=%f\n",devNum, *xa_f_pre, *ya_f_pre, *za_f_pre);
            
            
             
          }
          
          return;
    }
