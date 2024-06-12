/*==============================================================================
Copyright (c) 2029, CPIT. All Rights Reserved.

FILENAME:   FpgaSpiDev.c
 
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
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <linux/spi/spidev.h>
#include <stdint.h>

#include <errno.h>

#include "FpgaSpiDev.h"


#define BCMDEV "/dev/spidev1.0"
 
#define NREAD	(0x60)
#define NWRITE	(0x61)
 
#define SIO (0xF0)
#define STS (0xFE)
#define SPG (0xFF)
 
#define SPIF	(0x80)
#define RACK	(0x20)
#define RXRDY	(0x02)
#define TXRDY	(0x01)

int fpga_spi_reset,fpga_spi_done,fpga_spi_int,fpga_spi_mosi,fpga_spi_clk;

int read_fpga_spi(unsigned char *pbuffer,unsigned char byte_cnt)
{
	unsigned char wr_buf[32],rd_buf[32],*bp;
	int ret;
	struct spi_ioc_transfer xfer[2];
	int file;
 
	file = open(BCMDEV, O_RDWR);
	if (file<0) {
		printf("open fpga upgrade error\n");
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
	ret = ioctl(file, SPI_IOC_MESSAGE(2), xfer);
	
	printf("SPI int=%x\r\n",rd_buf[0]);

	if (ret < 0) {
		perror("SPI_IOC_MESSAGE");
		return -1;
	}

	memcpy(pbuffer,rd_buf,byte_cnt);

	close(file);
	return 0;
}
 
int write_fpga_spi(unsigned char *pbuffer,unsigned int byte_cnt)
{
	unsigned char wr_buf[1024],rd_buf[32],*bp;
	int len, ret;
	struct spi_ioc_transfer xfer[2];
	int file,i;
	unsigned int result;
 

	if (byte_cnt>sizeof(wr_buf))
	{
		printf("byte_cnt bigger error\n");
	}
	
	file = open(BCMDEV, O_RDWR);
	if (file<0) {
		printf("open fpga spi error\n");
		return -1;
	}

	memcpy(wr_buf, pbuffer, byte_cnt);
	memset(rd_buf, 0, sizeof(rd_buf));
	memset(xfer,0,sizeof(xfer));

	xfer[0].tx_buf = (unsigned long) wr_buf;
	xfer[0].len = byte_cnt;
 
	xfer[1].rx_buf = (unsigned long) rd_buf;
	xfer[1].len = 1;
	ret = ioctl(file, SPI_IOC_MESSAGE(2), xfer);
 
	close(file);
	return ret;
}

/*
 * @description		: period_io_spi_func
 * @param - dataBuf	: dataBuf 数据首地址
 * @param - dataLength	: 数据长度
 * @return 			: 0 成功;其他 失败
 */
static void period_io_spi_func(uint8_t *dataBuf,uint16_t dataLength)
{
	//printf("io-spi len:%d\r\n",dataLength);
	for (uint16_t i = 0; i < dataLength; i++)
	{
		write(fpga_spi_clk,FPGA_SPI_CLK_L,sizeof(FPGA_SPI_CLK_L));
		write(fpga_spi_mosi,FPGA_SPI_MOSI_L,sizeof(FPGA_SPI_MOSI_L));
		for (int8_t j = 7; j >= 0; --j)
		{
			if (dataBuf[i] & (1 << j))
			{
				write(fpga_spi_mosi,FPGA_SPI_MOSI_H,sizeof(FPGA_SPI_MOSI_H));
			}
			else
			{
				write(fpga_spi_mosi,FPGA_SPI_MOSI_L,sizeof(FPGA_SPI_MOSI_L));
			}

			write(fpga_spi_clk,FPGA_SPI_CLK_L,sizeof(FPGA_SPI_CLK_L));
			write(fpga_spi_clk,FPGA_SPI_CLK_H,sizeof(FPGA_SPI_CLK_H));
		}
		
	}	
}

int setup_fpga_spi_dev(void)
{
	//DONE
	system("echo 97 > /sys/class/gpio/export");
	system("echo in > /sys/class/gpio/gpio97/direction");
	//INT
	system("echo 98 > /sys/class/gpio/export");
	system("echo in  > /sys/class/gpio/gpio98/direction");
	//RESET
	system("echo 99 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio99/direction");
	//Dout
	system("echo 113 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio113/direction");
	//Clk
	system("echo 115 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio115/direction");
	
	fpga_spi_reset = open(FPGA_SPI_RESET, O_RDWR);
	if(fpga_spi_reset == -1)
	{
        printf("ERR: Radio hard cs pin value open error.\n");
        return EXIT_FAILURE;
	}

    fpga_spi_done = open(FPGA_SPI_DONE, O_RDONLY);
	if(fpga_spi_done == -1)
	{
        printf("ERR: Radio hard done pin value open error.\n");
        return EXIT_FAILURE;
	}

    fpga_spi_int = open(FPGA_SPI_INT, O_RDONLY);
	if(fpga_spi_int == -1)
	{
        printf("ERR: Radio hard int pin value open error.\n");
        return EXIT_FAILURE;
	}

	fpga_spi_mosi = open(FPGA_SPI_MOSI, O_RDWR);
	if(fpga_spi_mosi == -1)
	{
        printf("ERR: Radio hard mosi pin value open error.\n");
        return EXIT_FAILURE;
	}

	fpga_spi_clk = open(FPGA_SPI_CLK, O_RDWR);
	if(fpga_spi_clk == -1)
	{
        printf("ERR: Radio hard clk pin value open error.\n");
        return EXIT_FAILURE;
	}
	return 0;
}

int period_fpga_reset(void)
{
	write(fpga_spi_reset,FPGA_SPI_RESET_L,sizeof(FPGA_SPI_RESET_L));
	sleep(1);
	write(fpga_spi_reset,FPGA_SPI_RESET_H,sizeof(FPGA_SPI_RESET_H));

	return 0;
}

//获取文件大小
unsigned long get_file_size(const char *filename)  
{  
    struct stat buf;  
    if(stat(filename, &buf)<0)  
    {  
        return 0;  
    }  
    return (unsigned long)buf.st_size;  
} 

int period_fpga_upgrade(char *pPath)
{
	FILE *fpga_fd = NULL;
	uint16_t block_total_num=0;
	unsigned long file_size = 0;
	uint8_t tmp_buf[BLOCK_LEN]={0x00};
	int read_len = 0;
	int ret = 0;

	file_size = get_file_size(pPath);
	block_total_num = (file_size%BLOCK_LEN)==0 ? (file_size/BLOCK_LEN):(file_size/BLOCK_LEN+1);

	fpga_fd = fopen(pPath, "rb+");
	if(fpga_fd < 0) {
		printf("can't open file fpga bin!\r\n");
	}

	printf("file_size:%d,block_total_num:%d!\r\n",file_size,block_total_num);

	write(fpga_spi_reset, FPGA_SPI_RESET_L, sizeof(FPGA_SPI_RESET_L));
	usleep(1000);
	write(fpga_spi_reset, FPGA_SPI_RESET_H, sizeof(FPGA_SPI_RESET_H));
	usleep(10000);
	printf("fpga update flag1:%d,%d\r\n",atoi(tmp_buf),read_len);

	while (atoi(tmp_buf) != 1)
	{
		read_len=read(fpga_spi_int,tmp_buf,3);
		usleep(500);
	}

	printf("fpga update flag2:%d,%d\r\n",atoi(tmp_buf),read_len);	
	
	for (size_t i = 0; i < block_total_num; i++)
	{
		if(i==(block_total_num-1))
		{
			ret = fread(tmp_buf,1, file_size%BLOCK_LEN,fpga_fd);
			
			//write_fpga_spi(tmp_buf,file_size%BLOCK_LEN);
			period_io_spi_func(tmp_buf,file_size%BLOCK_LEN);
		}
		else
		{
			ret = fread(tmp_buf,1, BLOCK_LEN,fpga_fd);
			
			//write_fpga_spi(tmp_buf,BLOCK_LEN);
			period_io_spi_func(tmp_buf,BLOCK_LEN);
		}
		//usleep(1);			
	}

	//发送100个无效包I
	tmp_buf[0] = 0x00;
	tmp_buf[1] = 0x00;
	tmp_buf[2] = 0x00;
	tmp_buf[3] = 0xa0;
	for (size_t i = 0; i < 150; i++)
	{
		//write_fpga_spi(tmp_buf,4);
		period_io_spi_func(tmp_buf,4);
	}
		
	memset(tmp_buf,0,3);
	while (atoi(tmp_buf) != 1)
	{
		read(fpga_spi_done,tmp_buf,3);
		usleep(5000);
	}
	printf("fpga update flag3:%d\r\n",atoi(tmp_buf));
	
	//write_fpga_spi(tmp_buf,20);
	period_io_spi_func(tmp_buf,20);

	fclose(fpga_fd);
	return 0;
}