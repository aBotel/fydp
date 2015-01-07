CC=g++
CPPFLAGS=-c -Wall
CFLAGS=-c -Wall -g
CSOURCES=ADXL345.c
COBJS = ADXL345.o
CPPSOURCES= mcp3008Spi.cpp mcp3008SpiTest.cpp
CPPOBJS= mcp3008Spi.o
HEADERS=mcp3008Spi.h
OUT_EXE= ./test_out

all: test  

test: ADXL345.o mcp3008Spi.o mcp3008SpiTest.o 
	$(CC) -o out_exe ADXL345.o mcp3008Spi.o mcp3008SpiTest.o -lcurl

ADXL345.o: ADXL345.c
	$(CC) $(CFLAGS) ADXL345.c

mcp3008Spi.o: mcp3008Spi.cpp
	$(CC) $(CPPFLAGS) mcp3008Spi.cpp

mcp3008SpiTest.o: mcp3008SpiTest.cpp
	$(CC) $(CFLAGS) mcp3008SpiTest.cpp

clean: 
	rm -rf *.o


