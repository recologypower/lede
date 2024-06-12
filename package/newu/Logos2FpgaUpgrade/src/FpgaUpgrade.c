/*==============================================================================
Copyright (c) 2029, CPIT. All Rights Reserved.

FILENAME:   FpgaUpgrade.c
 
MODULE NAME:mcb driver module

DESCRIPTION:
            Source Code of fpga_spi Driver.

HISTORY:

Date        CR No           Person           Description
----------  ------------    ------------     -------------

2024/4/10                 zhaobaoxing         	created

==============================================================================*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <linux/spi/spidev.h>
#include <stdint.h>

#include <errno.h>

#include "FpgaSpiDev.h"

 
int main(int argc,char** argv)
{
	if (argc < 3)
	{
		printf("Please enter the correct parameters\r\n");
		return -1;
	}

	setup_fpga_spi_dev();
	
	switch (atoi(argv[1]))
	{
	case 0://复位FPGA
		period_fpga_reset();
		break;
	case 1://升级FPGA
		period_fpga_upgrade(argv[2]);
		break;
	
	default:
		printf("para error\r\n");
		break;
	}
	return 0;
}