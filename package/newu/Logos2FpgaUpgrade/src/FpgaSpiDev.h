/*===========================================================================

Copyright (c) 2029, CPIT. All Rights Reserved.

FILENAME:   FpgaSpiDev.h
 
MODULE NAME:mcb driver module

DESCRIPTION:
            Source Code of fpga_spi Driver.

HISTORY:

Date        CR No           Person           Description
----------  ------------    ------------     -------------

2024/04/10                 zhaobaoxing        created

==============================================================================*/



#ifndef __FpgaSpiDev_H__
#define __FpgaSpiDev_H__
    
#ifdef __cplusplus
    extern "C" {
#endif



#include <stdio.h>

typedef unsigned char 	uint8;
typedef signed char		int8;
typedef unsigned short	uint16;
typedef signed short	int16;
typedef unsigned int	uint32;
typedef signed int		int32;
typedef unsigned long   uint64;
typedef signed long     int64;

#define BLOCK_LEN   1024


#define FPGA_SPI_DONE         "/sys/class/gpio/gpio97/value"
#define FPGA_SPI_DONE_H       "1"
#define FPGA_SPI_DONE_L       "0"
#define FPGA_SPI_INT          "/sys/class/gpio/gpio98/value"
#define FPGA_SPI_INT_H        "1"
#define FPGA_SPI_INT_L        "0"
#define FPGA_SPI_RESET        "/sys/class/gpio/gpio99/value"
#define FPGA_SPI_RESET_H      "1"
#define FPGA_SPI_RESET_L      "0"
#define FPGA_SPI_MOSI        "/sys/class/gpio/gpio113/value"
#define FPGA_SPI_MOSI_H      "1"
#define FPGA_SPI_MOSI_L      "0"
#define FPGA_SPI_CLK        "/sys/class/gpio/gpio115/value"
#define FPGA_SPI_CLK_H      "1"
#define FPGA_SPI_CLK_L      "0"


int period_fpga_reset(void);
int period_fpga_upgrade(char *pPath);

#ifdef __cplusplus
}
#endif 

#endif /* __FpgaSpiDev_H__ */


