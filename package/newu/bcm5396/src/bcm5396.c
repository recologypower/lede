/******************************************************************************
 *	bcm5396 - bcm5396.c
 *	test program for the tuxbox-framebuffer device
 *	tests all GTX/eNX supported modes
 *                                                                            
 *	(c) 2003 Carsten Juttner (carjay@gmx.net)
 *
 * 	This program is free software; you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	The Free Software Foundation; either version 2 of the License, or
 * 	(at your option) any later version.
 *
 * 	This program is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * 	You should have received a copy of the GNU General Public License
 * 	along with this program; if not, write to the Free Software
 * 	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *  									      
 ******************************************************************************
 * $Id: bcm5396.c,v 1.5 2005/01/14 23:14:41 carjay Exp $
 ******************************************************************************/

// TODO: - should restore the colour map and mode to what it was before
//	 - is colour map handled correctly?

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

#define BCMDEV "/dev/spidev3.0"
 
#define NREAD	(0x60)
#define NWRITE	(0x61)
 
#define SIO (0xF0)
#define STS (0xFE)
#define SPG (0xFF)
 
#define SPIF	(0x80)
#define RACK	(0x20)
#define RXRDY	(0x02)
#define TXRDY	(0x01)
 
unsigned int readBCM5396Reg(unsigned char page,unsigned char offset,unsigned char regType)
{
	unsigned char wr_buf[32],rd_buf[32],*bp;
	int len, status;
	struct spi_ioc_transfer xfer[2];
	int file;
	unsigned int result;
 
	file = open(BCMDEV, O_RDWR);
	if (file<0) {
		printf("open 5396 error\n");
		return 1;
	}
	memset(wr_buf, 0, sizeof(wr_buf));
	memset(rd_buf, 0, sizeof(rd_buf));
	memset(xfer,0,sizeof(xfer));
 
	wr_buf[0] = NREAD;
	wr_buf[1] = STS;
	xfer[0].tx_buf = (unsigned long) wr_buf;
	xfer[0].len = 2;
 
	xfer[1].rx_buf = (unsigned long) rd_buf;
	xfer[1].len = 1;
	status = ioctl(file, SPI_IOC_MESSAGE(2), xfer);
 
	wr_buf[0] = NWRITE;
	wr_buf[1] = SPG;
	wr_buf[2] = page;
	xfer[0].tx_buf = (unsigned long) wr_buf;
	xfer[0].len = 3;
 
	xfer[1].rx_buf = (unsigned long) rd_buf;
	xfer[1].len = 0;
	status = ioctl(file, SPI_IOC_MESSAGE(1), xfer);
 
	wr_buf[0] = NREAD;
	wr_buf[1] = offset;
	xfer[0].tx_buf = (unsigned long) wr_buf;
	xfer[0].len = 2;
 
	xfer[1].rx_buf = (unsigned long) rd_buf;
	xfer[1].len = 1;
	status = ioctl(file, SPI_IOC_MESSAGE(2), xfer);
 
	wr_buf[0] = NREAD;
	wr_buf[1] = STS;
	xfer[0].tx_buf = (unsigned long) wr_buf;
	xfer[0].len = 2;
 
	xfer[1].rx_buf = (unsigned long) rd_buf;
	xfer[1].len = 1;
	status = ioctl(file, SPI_IOC_MESSAGE(2), xfer);
 
	wr_buf[0] = NREAD;
	wr_buf[1] = SIO;
	xfer[0].tx_buf = (unsigned long) wr_buf;
	xfer[0].len = 2;
 
	xfer[1].rx_buf = (unsigned long) rd_buf;
	xfer[1].len = regType/8;
	status = ioctl(file, SPI_IOC_MESSAGE(2), xfer);
 
	len=status;
	if (status < 0) {
		perror("SPI_IOC_MESSAGE");
		return -1;
	}
 
//	printf("NREAD response(%d): ", status);
//	for (bp = rd_buf; len; len--)
//		printf("%02x ", *bp++);
//	printf("\n");
 
	bp = rd_buf;
	memcpy(&result,bp,regType/8);
	printf("read result is 0x%x\n",result);
	close(file);
	return result;
}
 
 
unsigned int writeBCM5396Reg(unsigned char page,unsigned char offset,unsigned char *pBuffer,unsigned char regType)
{
	unsigned char wr_buf[32],rd_buf[32],*bp;
	int len, status;
	struct spi_ioc_transfer xfer[2];
	int file,i;
	unsigned int result;
 
	file = open(BCMDEV, O_RDWR);
	if (file<0) {
		printf("open 5396 error\n");
		return 1;
	}
	memset(wr_buf, 0, sizeof(wr_buf));
	memset(rd_buf, 0, sizeof(rd_buf));
	memset(xfer,0,sizeof(xfer));
 
	wr_buf[0] = NREAD;
	wr_buf[1] = STS;
	xfer[0].tx_buf = (unsigned long) wr_buf;
	xfer[0].len = 2;
 
	xfer[1].rx_buf = (unsigned long) rd_buf;
	xfer[1].len = 1;
	status = ioctl(file, SPI_IOC_MESSAGE(2), xfer);
 
	wr_buf[0] = NWRITE;
	wr_buf[1] = SPG;
	wr_buf[2] = page;
	xfer[0].tx_buf = (unsigned long) wr_buf;
	xfer[0].len = 3;
 
	xfer[1].rx_buf = (unsigned long) rd_buf;
	xfer[1].len = 0;
	status = ioctl(file, SPI_IOC_MESSAGE(1), xfer);
 
	wr_buf[0] = NWRITE;
	wr_buf[1] = offset;
	for(i=0;i<regType/8;i++)
	{
		wr_buf[2+i] = pBuffer[i];
	}
	xfer[0].tx_buf = (unsigned long) wr_buf;
	xfer[0].len = 2+(regType/8);;
 
	xfer[1].rx_buf = (unsigned long) rd_buf;
	xfer[1].len = 0;
	status = ioctl(file, SPI_IOC_MESSAGE(1), xfer);
	close(file);
	return status;
}
 
int main()
{
	unsigned int temp;
	unsigned char buf[10];
	int res=0;
 
	buf[0]=0x00;
	buf[1]=0x0f;
	buf[2]=0x00;
 
	temp=readBCM5396Reg(2,0x30,8);
	printf("page 0x2 offset 0x30 is 0x%x\n",temp);
 
	temp=readBCM5396Reg(0x31,0x00,32);
	printf("before write page 0x31 offset 0x0 is 0x%x\n",temp);
 
	res=writeBCM5396Reg(0x31, 0x00, buf,32);//Port 0 - Slot 10
	if(res<0)
	{
		printf("write bcm5396 error\n");
	}
 
	temp=readBCM5396Reg(0x31,0x00,32);
	printf("after write page 0x31 offset 0x0 is 0x%x\n",temp);
	return 0;
}