/***********************************************************************
 * mcp3008SpiTest.cpp. Sample program that tests the mcp3008Spi class.
 * an mcp3008Spi class object (a2d) is created. the a2d object is instantiated
 * using the overloaded constructor. which opens the spidev0.0 device with 
 * SPI_MODE_0 (MODE 0) (defined in linux/spi/spidev.h), speed = 1MHz &
 * bitsPerWord=8.
 * 
 * call the spiWriteRead function on the a2d object 20 times. Each time make sure
 * that conversion is configured for single ended conversion on CH0
 * i.e. transmit ->  byte1 = 0b00000001 (start bit)
 *                   byte2 = 0b1000000  (SGL/DIF = 1, D2=D1=D0=0)
 *                   byte3 = 0b00000000  (Don't care)
 *      receive  ->  byte1 = junk
 *                   byte2 = junk + b8 + b9
 *                   byte3 = b7 - b0
 *     
 * after conversion must merge data[1] and data[2] to get final result 
 * 
 * 
 * 
 * *********************************************************************/
#include "mcp3008Spi.h"
#include "ADXL345.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
 
using namespace std;
 
int main(void)
{
    mcp3008Spi a2d1("/dev/spidev0.0", SPI_MODE_0, 1000000, 8);
    //mcp3008Spi *ptr = &a2d;
    int foo = 20;
    int *fd = &foo;
    int adxl1_addr = 0x53;
 
    adxl_init(fd,adxl1_addr);
    float xa_f_pre = 2.7;
    float xa_pre= 2.7;
    float ya_f_pre = -8.46;
    float ya_pre = -8.46;
    float za_f_pre = -4.21;
    float za_pre = -4.21;
    
    while(1)
    {
		//a2d scan
		a2d1.mcp3008_Scan(1);
		
		// NEW CODE?
		adxl_read(*fd,adxl1_addr,1,&xa_f_pre, &xa_pre, &ya_f_pre, &ya_pre, &za_f_pre, &za_pre );
		//sleep(1);
		//usleep(500);
		cout << endl;
	
    }

    return 0;
}
