/*==============================================================================
Copyright (c) 2009, CPIT. All Rights Reserved.

FILENAME:   bcm53xxdrv.c
 
MODULE NAME:mcb driver module

DESCRIPTION:
            Source Code of bcm53xx Driver.

HISTORY:

Date        CR No           Person           Description
----------  ------------    ------------     -------------

2009/4/20                 zhaojianli         	created
histrory: 
20010/1/7                 zhaojianli            add port mirror function
2012/1/5                  liangxin              add  ARL &MIB  function
                                                modify  port mirror funcion
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

#include "bcm53xxdrv.h"

/*******************************************************************************/


/*******************************************************************************/

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

int read_bcm53xx(unsigned char page,unsigned char offset,unsigned char byte_cnt, unsigned char *pbuffer)
{
	unsigned char wr_buf[32],rd_buf[32],*bp;
	int len,ret;
	struct spi_ioc_transfer xfer[2];
	int file;
 
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
	ret = ioctl(file, SPI_IOC_MESSAGE(2), xfer);
	
	printf("SPI int=%x\r\n",rd_buf[0]);
 
	wr_buf[0] = NWRITE;
	wr_buf[1] = SPG;
	wr_buf[2] = page;
	xfer[0].tx_buf = (unsigned long) wr_buf;
	xfer[0].len = 3;
 
	xfer[1].rx_buf = (unsigned long) rd_buf;
	xfer[1].len = 0;
	ret = ioctl(file, SPI_IOC_MESSAGE(1), xfer);
 
	wr_buf[0] = NREAD;
	wr_buf[1] = offset;
	xfer[0].tx_buf = (unsigned long) wr_buf;
	xfer[0].len = 2;
 
	xfer[1].rx_buf = (unsigned long) rd_buf;
	xfer[1].len = 1;
	ret = ioctl(file, SPI_IOC_MESSAGE(2), xfer);
 
	wr_buf[0] = NREAD;
	wr_buf[1] = STS;
	xfer[0].tx_buf = (unsigned long) wr_buf;
	xfer[0].len = 2;
 
	xfer[1].rx_buf = (unsigned long) rd_buf;
	xfer[1].len = 1;
	ret = ioctl(file, SPI_IOC_MESSAGE(2), xfer);
 
	wr_buf[0] = NREAD;
	wr_buf[1] = SIO;
	xfer[0].tx_buf = (unsigned long) wr_buf;
	xfer[0].len = 2;
 
	xfer[1].rx_buf = (unsigned long) rd_buf;
	xfer[1].len = byte_cnt;
	ret = ioctl(file, SPI_IOC_MESSAGE(2), xfer);
 
	len=ret;
	if (ret < 0) {
		perror("SPI_IOC_MESSAGE");
		return -1;
	}
 
//	printf("NREAD response(%d): ", int);
//	for (bp = rd_buf; len; len--)
//		printf("%02x ", *bp++);
//	printf("\n");
 
	bp = rd_buf;
	memcpy(pbuffer,bp,byte_cnt);

	close(file);
	return 0;
}
 
int write_bcm53xx(unsigned char page,unsigned char offset,unsigned char byte_cnt,unsigned char *pBuffer)
{
	unsigned char wr_buf[32],rd_buf[32],*bp;
	int len, ret;
	struct spi_ioc_transfer xfer[2];
	int file,i;
	unsigned int result;
 
	file = open(BCMDEV, O_RDWR);
	if (file<0) {
		printf("open 5396 error\n");
		return -1;
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
 
	wr_buf[0] = NWRITE;
	wr_buf[1] = SPG;
	wr_buf[2] = page;
	xfer[0].tx_buf = (unsigned long) wr_buf;
	xfer[0].len = 3;
 
	xfer[1].rx_buf = (unsigned long) rd_buf;
	xfer[1].len = 0;
	ret = ioctl(file, SPI_IOC_MESSAGE(1), xfer);
 
	wr_buf[0] = NWRITE;
	wr_buf[1] = offset;
	for(i=0;i<byte_cnt;i++)
	{
		wr_buf[2+i] = pBuffer[i];
	}
	xfer[0].tx_buf = (unsigned long) wr_buf;
	xfer[0].len = 2+byte_cnt;
 
	xfer[1].rx_buf = (unsigned long) rd_buf;
	xfer[1].len = 0;
	ret = ioctl(file, SPI_IOC_MESSAGE(1), xfer);

	close(file);
	return ret;
}

/*
------------------------------------------------------------------------------
*
*   Function Name: dbcm53xx_port_enable
*
*   Input(s):
*       port_num:	    指向bcm53xx的端口
*       rx_tx_type:  	指向输入输出
*	    en_flag:       	使能或禁止
*
*   Output(s):
*       <none>
*
*   Returns:
*       BCM_DRV_OK 或 <0 其它错误码
*
*   Description:
*       此功能接口实现bcm53xx的端口传输的关闭或者使能
*
------------------------------------------------------------------------------
*/

int  dbcm53xx_port_enable(BcmPortEnumType  port_num, DirectTypeEnumType rx_tx_type,EnableEnumType en_flag)
{
	uint8 write_data = 0;
	uint8 tx_disable;
	uint8 rx_disable;
	tx_disable = (uint8)BBIT1;
	rx_disable = (uint8)BBIT0;
	if( BCM_DRV_OK != read_bcm53xx(PORT_CONTROL_PAGE, port_num, 1, &write_data) )
	{   
		#ifdef DEBUG_BCM

		printf("read bcm err,read_bcm53xx(),dbcm53xx_port_enable()\r\n");
		#endif
		return BCM_ETH_SW_PORT_R_ERR;
  	}
    
    switch(rx_tx_type)
    {
        case BCM_RX:
        if(BCM_ENABLE == en_flag)
        {
        write_data &= ~rx_disable;
        }
        else if(BCM_DISABLE == en_flag)
        {
        write_data |= rx_disable;
        }
        else
        {
                #ifdef DEBUG_BCM
                    printf("\tpara err,en_flag ,function dbcm53xx_port_enable() \n ");        
                #endif
                return BCM_ETH_SW_PORT_RX_ERR;
        }
        
        break;
        case BCM_TX:
        if(BCM_ENABLE == en_flag)
        {
        write_data &= ~tx_disable;
        }
        else if(BCM_DISABLE == en_flag)
        {
        write_data |= tx_disable;
        }
        else
        {
                #ifdef DEBUG_BCM
                    printf("\tpara err,en_flag ,function dbcm53xx_port_enable() \n ");        
                #endif
                return BCM_ETH_SW_PORT_TX_ERR;
        }
        break;
        case BCM_RX_TX:
        if(BCM_ENABLE == en_flag)
        {
            write_data &= ~(tx_disable + rx_disable);
        }
        else if(BCM_DISABLE == en_flag)
        {
            write_data |= (tx_disable + rx_disable);
        }
        else
        {
#ifdef DEBUG_BCM
                    printf("\tpara err,en_flag ,function dbcm53xx_port_enable() \n ");        
                #endif
        return BCM_ETH_SW_PORT_TX_RX_ERR;
        }
        break;
        default:
#ifdef DEBUG_BCM
                printf("\tpara err,rx_tx_type ,function dbcm53xx_port_enable() \n ");        
            #endif
            return BCM_PORT_ENABLE_PAR_ERR;

        }
        if(BCM_DRV_OK != write_bcm53xx(PORT_CONTROL_PAGE, port_num, 1, &write_data))
        {   
#ifdef DEBUG_BCM
            printf("\twrite bcm err ,write_bcm53xx(),dbcm53xx_port_enable()\n ");        
        #endif
    return BCM_ETH_SW_PORT_ERR;
    }
    return BCM_DRV_OK;
}


/*
------------------------------------------------------------------------------
*
*   Function Name: bcm53xx_port_mirror_enable
*
*   Input(s):
*      
*	    en_flag:   使能或禁止 0(使能) 1(禁止)
*
*   Output(s):
*       <none>
*
*   Returns:
*       BCM_DRV_OK 或 <0 其它错误码
*       
*
*   Description:
*       此功能接口实现bcm53xx的端口port mirror 功能的关闭或者使能
*
------------------------------------------------------------------------------
*/
int  dbcm53xx_mirror_enable(EnableEnumType en_flag)
{
    uint16 read_data = 0;
    if( BCM_DRV_OK != read_bcm53xx(MIRROR_CPATURE_ENABLE_PAGE, 0x10, 2, (uint8 *)&read_data) )
        {
#ifdef DEBUG_BCM
            printf("\tread bcm err,read_bcm53xx(),dbcm53xx_mirror_enable()\n ");        
#endif
        return MIRROR_ENABLE_R_ERR;
        }
        
    if( BCM_ENABLE == en_flag )
    {
        read_data |= BBIT15;
        
    }
    else if(BCM_DISABLE == en_flag )
    {
        read_data &= ~BBIT15;
    }
    else
    {
        #ifdef DEBUG_BCM
            printf("\tpara err,en_flag ,function dbcm53xx_mirror_enable() \n ");        
        #endif
        return BCM_MIRROR_ENABLE_PAR_ERR;
    }
    if(BCM_DRV_OK != write_bcm53xx(MIRROR_CPATURE_ENABLE_PAGE, 0x10, 2, (uint8 *)&read_data))
	{
        #ifdef DEBUG_BCM
            printf("\twrite bcm err ,write_bcm53xx(),dbcm53xx_mirror_enable()\n ");        
        #endif
        return BCM_MIRROR_ENABLE_SET_ERR;
	}
    return BCM_DRV_OK;

}
/*
------------------------------------------------------------------------------
*
*   Function Name: dbcm53xx_mirror_block
*
*   Input(s):
*      
*	    en_flag:   使能或禁止 0(使能) 1(禁止)
*
*   Output(s):
*       <none>
*
*   Returns:
*       BCM_DRV_OK 或 <0 其它错误码
*       
*
*   Description:
*       此功能接口实现bcm53xx 的capture port的阻塞。
*       (当使能阻塞的时候，会导致shell断掉!!)
*
------------------------------------------------------------------------------
*/
int  dbcm53xx_mirror_block(EnableEnumType en_flag)
{
    uint16 read_data = 0;
    if( BCM_DRV_OK != read_bcm53xx(MIRROR_CPATURE_ENABLE_PAGE, 0x10, 2, (uint8 *)&read_data) )
        {
#ifdef DEBUG_BCM
            printf("\tread bcm err,read_bcm53xx(),dbcm53xx_mirror_block()\n ");        
#endif
        return MIRROR_BLOCK_R_ERR;
        }
    
    if( BCM_ENABLE == en_flag )
    {
        read_data |= BBIT14;
        
    }
    else if(BCM_DISABLE == en_flag )
    {
        read_data &= ~BBIT14;
    }
    else
    {
        #ifdef DEBUG_BCM
            printf("\tpara err,en_flag ,function dbcm53xx_mirror_block() \n ");        
        #endif
        return BCM_MIRROR_BLOCK_PAR_ERR;
    }
    if(BCM_DRV_OK != write_bcm53xx(MIRROR_CPATURE_ENABLE_PAGE, 0x10, 2, (uint8 *)&read_data))
    {
        #ifdef DEBUG_BCM
            printf("\twrite bcm err ,write_bcm53xx(),dbcm53xx_mirror_block()\n ");        
        #endif
        return BCM_MIRROR_BLOCK_SET_ERR;
    }
    return BCM_DRV_OK;

}


/*
------------------------------------------------------------------------------
*
*   Function Name: dbcm53xx_mirror_capture_set
*
*   Input(s):
*      
*	    port_num:   要指定的作为mirror capture port 的端口
*
*   Output(s):
*       <none>
*
*   Returns:
*       BCM_DRV_OK 或 <0 其它错误码
*       
*
*   Description:
*       此功能接口实现 bcm53xx 用作mirror capture port端口的指定。
*
------------------------------------------------------------------------------
*/
int  dbcm53xx_mirror_capture_set(BcmPortEnumType port_num)
{
    uint16 read_data = 0;
    if(15 < port_num)
    {
        #ifdef DEBUG_BCM
            printf("\tpara err,port_num, dbcm53xx_mirror_capture_set()\n ");        
        #endif
        return PAR_MIRROR_CAPTURE_SET_ERR;
        
        }
        
        if( BCM_DRV_OK != read_bcm53xx(MIRROR_CPATURE_ENABLE_PAGE, 0x10, 2, (uint8 *)&read_data) )
        {
#ifdef DEBUG_BCM
            printf("\tread bcm err,read_bcm53xx(),dbcm53xx_mirror_capture_set()\n ");        
#endif
        return MIRROR_CAPTURE_R_ERR;
        }
    read_data &= 0xfff0;                    /* mask ��5λ��capture port*/

    read_data |= (port_num & 0x0000000f);   /* ���ú�capture port */
    
    if(BCM_DRV_OK != write_bcm53xx(MIRROR_CPATURE_ENABLE_PAGE, 0X10, 2, (uint8 *)&read_data))
    {
        #ifdef DEBUG_BCM
            printf("\twrite bcm err ,write_bcm53xx(),dbcm53xx_mirror_capture_set()\n ");        
        #endif
        return BCM_MIRROR_CAPTURE_SET_ERR;
    }
    return BCM_DRV_OK;
    
}
/*
------------------------------------------------------------------------------
*
*   Function Name: dbcm53xx_mirror_mask_set
*
*   Input(s):
*       rx_tx_type:  	指向输入输出
*	    port_num:   要指定的被mirror的端口.
*       
*           MIRROR_BACK_BOARD0         BBIT0
            MIRROR_BACK_BOARD1         BBIT1
            MIRROR_RIB                 BBIT2
            MIRROR_CPB5                BBIT3
            MIRROR_CPB4                BBIT4
            MIRROR_CPB3                BBIT5
            MIRROR_CPB2                BBIT6
            MIRROR_CPB1                BBIT7
            MIRROR_DEBUG               BBIT8            
            MIRROR_CPU                 BBIT9            
                  
*   Output(s):
*       <none>
*
*   Returns:
*       BCM_DRV_OK 或 <0 其它错误码
*       
*
*   Description:
*       此功能接口实现bcm53xx 被mirror的端口的指定。
*
------------------------------------------------------------------------------
*/
int  dbcm53xx_mirror_mask_set(DirectTypeEnumType rx_tx_type, uint32 port_num)
{
    uint32 mirror_caputre = 0;
    uint32 ingress_data = 0;
    uint32 egress_data = 0;


    /*port_num &= 0x0000ffff;*/
   
    switch(rx_tx_type)
    {
        case BCM_RX:
            {
            ingress_data |= ((uint32)0x01) << port_num;
            /* �������*/
            if(BCM_DRV_OK != write_bcm53xx(MIRROR_CPATURE_ENABLE_PAGE, 0x1c, 
                                            4, (uint8 *)&egress_data))
        {
            #ifdef DEBUG_BCM 
                printf("\tread bcm err 1,write_bcm53xx(),dbcm53xx_mirror_mask_set()\n ");        
            #endif
            return BCM_MIRROR_MASK_1_SET_ERR;
        }
        if(BCM_DRV_OK != write_bcm53xx(MIRROR_CPATURE_ENABLE_PAGE, 0x12, 
                                        4, (uint8 *)&ingress_data))
        {
            #ifdef DEBUG_BCM
                printf("\tread bcm err 1,write_bcm53xx(),dbcm53xx_mirror_mask_set()\n ");        
            #endif
            return BCM_MIRROR_MASK_1_SET_ERR;
        }
    
            
        break;
        }
        case BCM_TX:
        {
        egress_data |= ((uint32)0x01) << port_num;

            /* �������� */
            if(BCM_DRV_OK != write_bcm53xx(MIRROR_CPATURE_ENABLE_PAGE, 0x12, 
                                            4, (uint8 *)&ingress_data))
        {
            #ifdef DEBUG_BCM
                printf("\tread bcm err 1,write_bcm53xx(),dbcm53xx_mirror_mask_set()\n ");        
            #endif
            return BCM_MIRROR_MASK_1_SET_ERR;
        }
        
        if(BCM_DRV_OK != write_bcm53xx(MIRROR_CPATURE_ENABLE_PAGE, 0x1c, 
                                        4, (uint8 *)&egress_data))
        {
            
            #ifdef DEBUG_BCM
                printf("\tread bcm err 2,write_bcm53xx(),dbcm53xx_mirror_mask_set()\n ");        
            #endif
            return BCM_MIRROR_MASK_2_SET_ERR;
        }
        }
        break;
        case BCM_RX_TX:
                {
            ingress_data |= ((uint32)0x01) << port_num;
            egress_data |= ((uint32)0x01) << port_num;
            
            if(BCM_DRV_OK != write_bcm53xx(MIRROR_CPATURE_ENABLE_PAGE, 0x12, 
                                        4, (uint8 *)&ingress_data))
        {
            #ifdef DEBUG_BCM
                printf("\tread bcm err 3,write_bcm53xx(),dbcm53xx_mirror_mask_set()\n ");        
            #endif
            return BCM_MIRROR_MASK_3_SET_ERR;
        }
        if(BCM_DRV_OK != write_bcm53xx(MIRROR_CPATURE_ENABLE_PAGE, 0x1c, 
                                        4, (uint8 *)&egress_data))
        {
            #ifdef DEBUG_BCM
                printf("\tread bcm err 4,write_bcm53xx(),dbcm53xx_mirror_mask_set()\n ");        
            #endif
            return BCM_MIRROR_MASK_4_SET_ERR;
        }
        break;
        default:
            #ifdef DEBUG_BCM
                printf("\tpara err,rx_tx_type ,function dbcm53xx_port_enable() \n ");        
            #endif
            return PAR_MIRROR_MASK_SET_ERR;
            }
    }

    return BCM_DRV_OK;
    

}
/*
------------------------------------------------------------------------------
*
*   Function Name: dbcm53xx_port_mirror_set
*
*   Input(s):
*       rx_tx_type:      指向输入输出
*        src_port:   指定的被mirror的端口.
*       
*            MIRROR_BACK_BOARD0         BBIT0
*            MIRROR_BACK_BOARD1         BBIT1
*            MIRROR_RIB                 BBIT2
*            MIRROR_CPB5                BBIT3
*            MIRROR_CPB4                BBIT4
*            MIRROR_CPB3                BBIT5
*            MIRROR_CPB2                BBIT6
*            MIRROR_CPB1                BBIT7
*            MIRROR_DEBUG               BBIT8            
*            MIRROR_CPU                 BBIT9            
*       dest_port: PORT CAPTURE .
*   Output(s):
*       <none>
*
*   Returns:
*       BCM_DRV_OK 或 <0 其它错误码
*       
*
*   Description:
*       此功能接口实现bcm53xx 的port mirror 功能。
*
------------------------------------------------------------------------------
*/
int dbcm53xx_port_mirror_set(uint32 src_port, BcmPortEnumType dest_port, DirectTypeEnumType rx_tx_type)
{
    if((15 < dest_port) || ((1 > rx_tx_type)||(3 < rx_tx_type))) 
    {
        #ifdef DEBUG_BCM
            printf("\tpara err,port_num, dbcm53xx_port_mirror_set()\n ");        
        #endif
        return PAR_MIRROR_SET_ERR;

    }
    if(BCM_DRV_OK != dbcm53xx_mirror_capture_set(dest_port))
    {
         #ifdef DEBUG_BCM
            printf("\tERR:dbcm53xx_mirror_capture_set()\n ");        
         #endif
        return MIRROR_CAP_SET_ERR;
    }
     if(BCM_DRV_OK != dbcm53xx_mirror_mask_set( rx_tx_type, src_port))
    {
         #ifdef DEBUG_BCM
            printf("\tERR:dbcm53xx_mirror_mask_set()\n ");        
         #endif
        return MIRROR_MIRROR_MASK_SET_ERR;
    }
    return BCM_DRV_OK;

}


/***********************************************************
*   Function Name: dbcm53xx_show_bucket
*
*   Input(s):
*       port_num:        指向bcm53xx的端口

*
*   Output(s):
*       <none>
*
*   Returns:
*       BCM_DRV_OK 或 <0 其它错误码
*
*   Description:
*       此功能接口实现bcm53xx的某个端口对应的bucket0的值的读取
*

************************************************************/
int dbcm53xx_show_bucket(uint8  port_num )
{

    uint32 read_data=0;
  
    uint8  buck_size =  0;

    uint8 ratecount = 0;              /*JOSHUA2011.8.19*/

    uint8 rateflag = 0;

    if(BCM_PORT15 < port_num ) 
    {
		#ifdef DEBUG_BCM
            printf("para err,port_num, dbcm53xx_show_bucket()\r\n");        
        #endif

        return BCM_PORT_BUCKET_PAR_ERR;
    }

    if( BCM_DRV_OK != read_bcm53xx(RATE_CONTROL_PAGE,
                                                        (uint8)(0x10 + 4*port_num), 
                                                        4, 
                                                        (uint8 *)&read_data) )
    {   
		#ifdef DEBUG_BCM
            printf("para err,read_bcm53xx(), dbcm53xx_show_bucket()\r\n");        
        #endif
        return BCM_ETH_SW_PORT_R_ERR;
    }


    /*�ж��Ƿ�ʹ��rate control*/
    rateflag = (uint8)(read_data >>22) & 0x01;        /*JOSHUA2011.8.19*/

    switch(rateflag)
    {
        case BCM_RATE_ENABLE:

			#ifdef DEBUG_BCM
            printf("The rate control of port %d is enabled :)\r\n");        
			#endif
            break;
        case BCM_RATE_DISABLE:

			#ifdef DEBUG_BCM
            printf("The rate control of port %d is disabled :(\r\n",
                            port_num,0,0,0,0,0);        
			#endif
            break;

        default:
          ;
    }
    
    /*��ȡratecountֵ*/
    ratecount = (uint8)(read_data & 0xff);       /*JOSHUA2011.8.19*/

    if(ratecount >=1 && ratecount <= 28 )
    {
		#ifdef DEBUG_BCM
		printf("The Suppression rate of port %d is about %d KB\r\n",
                            port_num,((ratecount*8*1024)/125),0,0,0,0);        
		#endif

    }

    else if(ratecount >= 29 && ratecount <= 127 )
    {
		#ifdef DEBUG_BCM
		printf("The Suppression rate of port %d is about %d MB\r\n",
                            port_num,(ratecount-27),0,0,0,0);        
		#endif
    }
    else
    {
		#ifdef DEBUG_BCM
		printf("The Suppression rate of port %d is about %d MB\r\n",
                            port_num,((ratecount-115)*8),0,0,0,0);        
		#endif
    }

    /*��ȡbucket�Ĵ�С*/
    buck_size = ((uint8)(read_data >>8))&0x07;

    switch (buck_size)
    {

        case BUCK_16K:
			#ifdef DEBUG_BCM
			printf("The Bucket size of port %d is %dK\r\n",
                                    port_num,16,0,0,0,0);        
			#endif
            break;

        case BUCK_20K:
			#ifdef DEBUG_BCM
			printf("The Bucket size of port %d is %dK\r\n",
                                    port_num,20,0,0,0,0);        
			#endif
            break;

        case BUCK_28K:
			#ifdef DEBUG_BCM
			printf("The Bucket size of port %d is %dK\r\n",
                                    port_num,28,0,0,0,0);        
			#endif
            break;

        case BUCK_40K:
			#ifdef DEBUG_BCM
			printf("The Bucket size of port %d is %dK\r\n",
                                    port_num,40,0,0,0,0);        
			#endif
            break;

        case BUCK_76K:
			#ifdef DEBUG_BCM
			printf("The Bucket size of port %d is %dK\r\n",
                                    port_num,76,0,0,0,0);        
			#endif
            break;

        case BUCK_140K:
			#ifdef DEBUG_BCM
			printf("The Bucket size of port %d is %dK\r\n",
                                    port_num,140,0,0,0,0);        
			#endif
            break;

        case BUCK_268K:
			#ifdef DEBUG_BCM
			printf("The Bucket size of port %d is %dK\r\n",
                                    port_num,268,0,0,0,0);        
			#endif
            break;

        case BUCK_500K:
			#ifdef DEBUG_BCM
			printf("The Bucket size of port %d is %dK\r\n",
                                    port_num,500,0,0,0,0);        
			#endif
            break;    

        default:
            #ifdef DEBUG_BCM
			printf("return err,bucket_size ,function dbcm53xx_show_bucket()\r\n",
                    0,0,0,0,0,0);        
			#endif
            return BCM_ETH_SW_PORT_R_ERR;            
        }

        return BCM_DRV_OK;
}

/***********************************************************

*   Function Name: dbcm53xx_set_bucket
*
*   Input(s):
*       port_num:        指向bcm53xx的端口
*        bucket_size:    要设置的端口所对应的bucket0的大小
*        en_flag      :   使能某个端口的rate control功能
*        rate_count :   (1~240)速率控制基数，通过公式可以计算出限速值
*   Output(s):
*       <none>
*
*   Returns:
*       BCM_DRV_OK 或 <0 其它错误码
*
*   Description:
*       此功能接口实现bcm53xx的某个端口对应的bucket0的值的设置，
*        限速功能的使能和限速值的设置。
*


************************************************************/
int dbcm53xx_set_bucket(uint8  port_num ,uint8 bucket_size,EnableRateEnumType en_flag,uint32 rate_count)
{

    uint32 write_data=0;

    if(BCM_PORT15 < port_num ) 
    {
		#ifdef DEBUG_BCM
			printf("para err,port_num, dbcm53xx_show_bucket()\r\n",
                                0,0,0,0,0,0);        
		#endif
       
        return BCM_PORT_RATECTRL_PAR_ERR;
    }

    if(BUCK_500K < bucket_size)
    {
		#ifdef DEBUG_BCM
			printf("para err,bucket_size, dbcm53xx_set_bucket()\r\n",
                                0,0,0,0,0,0);        
		#endif
        return BCM_PORT_RATECTRL_PAR_ERR;
    }

    if(MAX_RATE_COUNT < rate_count)
    {
		#ifdef DEBUG_BCM
			printf("para err,rate_count, dbcm53xx_set_bucket()\r\n",
                                0,0,0,0,0,0);        
		#endif
        return BCM_PORT_RATECTRL_PAR_ERR;
    }
    
    if( BCM_DRV_OK != read_bcm53xx(RATE_CONTROL_PAGE,
                                                    (uint8)(0x10 + 4*port_num), 
                                                    4, 
                                                    (uint8 *)&write_data) )
    {
		#ifdef DEBUG_BCM
			printf("bcm err,read_bcm53xx(), dbcm53xx_set_bucket()\r\n",
                                0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

     /*使能某个端口的rate contrl功能*/

    if( BCM_RATE_DISABLE == en_flag )
    {
        write_data &= ~BBIT22;
    }
    else if(BCM_RATE_ENABLE == en_flag )
    {
        write_data |= BBIT22;
    }
    else
    {
		#ifdef DEBUG_BCM
			printf("para err,en_flag, function dbcm53xx_set_bucket()\r\n",
                                0,0,0,0,0,0);        
		#endif
        return BCM_PORT_RATECTRL_PAR_ERR;
    }

    /*设置bucket 0 的大小，bucket1暂时不使用*/
    write_data&=0xfffff8ff;                                            /*JOSHUA2011.8.19*/
    write_data|=((uint32)bucket_size << 8);

    /*设置某个port 所对应的bucket 0 的临界速率(kBps)，bucket1暂时
        不使用*/


    if(rate_count >=1 && rate_count <= 28 )
    {
		#ifdef DEBUG_BCM
			printf("The Suppression rate of port %d is about %d KB\r\n",
                                port_num,((rate_count*8*1024)/125),0,0,0,0);        
		#endif
    }
  
    else if(rate_count >= 29 && rate_count <= 127 )
    {
		#ifdef DEBUG_BCM
			printf("The Suppression rate of port %d is about %d MB\r\n",
                                port_num,(rate_count-27),0,0,0,0);        
		#endif
    }

    else
    {
		#ifdef DEBUG_BCM
			printf("The Suppression rate of port %d is about %d MB\r\n",
                                port_num,((rate_count-115)*8),0,0,0,0);        
		#endif
    }

    write_data &= 0xffffff00;                                         /*JOSHUA2011.8.19*/
    write_data |= rate_count;

      /*完成设置操作*/
    if(BCM_DRV_OK != write_bcm53xx(RATE_CONTROL_PAGE,
                                                       (uint8)(0x10 + 4*port_num), 
                                                       4, 
                                                       (uint8 *)&write_data))
    {
        #ifdef DEBUG_BCM
			printf("write_data err,write_bcm53xx(), function dbcm53xx_set_bucket()\r\n",
                                        0,0,0,0,0,0);        
		#endif
        return BCM_PORT_RATECTRL_PAR_ERR;
    }
    return BCM_DRV_OK;
        
}


int dbcm53xx_show_ratemode()
{

     uint32 read_data=0;
     uint8 ratemode = 0;
     uint8 packetdorp = 0;
     uint8 packettype = 0;


    if( BCM_DRV_OK != read_bcm53xx(RATE_CONTROL_PAGE, 
                                                        0x00, 
                                                        4,
                                                        (uint8 *)&read_data) )
    {
		#ifdef DEBUG_BCM
			printf("bcm err,read_bcm53xx(), dbcm53xx_set_ratemode()\r\n",
                                0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;

    }

    /*查看限速模式*/
    ratemode = (uint8)(read_data >> 7) & 0x01;
    
    if(ratemode)
    {
		#ifdef DEBUG_BCM
			printf("Rate Mode: Link Speed Mode\r\n",
                                       0,0,0,0,0,0);        
		#endif
    }
    else
    {
		#ifdef DEBUG_BCM
			printf("Rate Mode: Absolute Bit Rate Mode\r\n",
                                   0,0,0,0,0,0);        
		#endif
    }

    /*查看超速后的处理方法*/
    packetdorp = (uint8)(read_data >> 6) & 0x01;

    if(packetdorp)
    {
		#ifdef DEBUG_BCM
			printf("Packet Process Mode: Drop Mode\r\n",
                                       0,0,0,0,0,0);        
		#endif
    }
    else
    {
		#ifdef DEBUG_BCM
			printf("Packet Process Mode: Return Pause Frame\r\n",
                                   0,0,0,0,0,0);        
		#endif
    }

    packettype = (uint8)(read_data & 0x3f);

    switch(packettype)
    {   
        case 0:
			#ifdef DEBUG_BCM
			printf("Packet Type: packet type has not set\r\n",
                                       0,0,0,0,0,0);        
			#endif
            break;
        case 1:
			#ifdef DEBUG_BCM
			printf("Packet Type: Unicast\r\n",
                                       0,0,0,0,0,0);        
			#endif
            break;
        case 2:
			#ifdef DEBUG_BCM
			printf("Packet Type: Multicast\r\n",
                                       0,0,0,0,0,0);        
			#endif
            break;
        case 4:
			#ifdef DEBUG_BCM
			printf("Packet Type: Broadcast_short\r\n",
                                       0,0,0,0,0,0);        
			#endif
            break;
        case 8:
			#ifdef DEBUG_BCM
			printf("Packet Type: Broadcast_long\r\n",
                                       0,0,0,0,0,0);        
			#endif
            break;
        case 32:
			#ifdef DEBUG_BCM
			printf("Packet Type: Destination lookup failed\r\n",
                                       0,0,0,0,0,0);        
			#endif
            break;
        default:
            #ifdef DEBUG_BCM
			printf("bcm err,dbcm53xx_show_ratemode()\r\n",
                                       0,0,0,0,0,0);        
			#endif
            return BCM_PORT_RATECTRL_PAR_ERR;

    }
    
    return BCM_DRV_OK;

}

/***********************************************************
*   Function Name: dbcm53xx_set_ratemode
*
*       Input(s):  
*         rate_flag:  设置限速模式
*                       0:  absolute bit rate mode
*                       1: relate to Link Speed mode
*        en_flag :        超速后，对数据包的处理模式
                        0: 返回pause 帧
                        1: 丢包
*      
*        packet_type      :   要限速的packte的类型，针对我们的需求
*                                   只需要限制单播(unicast)就行了。
*                                   UNICAST = 0
*                        MULTICAST =1
*                        BROADCAST_S =2
*                        BROADCAST_L =3
*                        DEST_LK_FAILED = 4
*        
*   Output(s):
*       <none>
*
*   Returns:
*       BCM_DRV_OK 或 <0 其它错误码
*
*   Description:
*       此功能接口实现bcm53xx的限速模式;超速后对数据包的处理模式;
         要限速的packet类型

************************************************************/
int dbcm53xx_set_ratemode(BcmBitRateMode rate_flag,
                                                                BcmDropMode en_flag,
                                                                BcmRateCtrlPacketType packet_type)
{

    uint32 write_data=0;


    if( BCM_DRV_OK != read_bcm53xx(RATE_CONTROL_PAGE, 
                                                        0x00, 
                                                        4,
                                                        (uint8 *)&write_data) )
    {   
		#ifdef DEBUG_BCM
			printf("bcm err,read_bcm53xx(), dbcm53xx_set_ratemode()\r\n",
                                0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;

    }

    /*设置限速模式，使用 absolute bit rate mode或者relate to Link Speed mode */


    if(BCM_ABSOLUTE_RATE == rate_flag)
    {
        write_data &= ~BBIT7;                   /*只使用buckte0*/
    }
    else if(BCM_LINK_SPEED == rate_flag)
    {
        write_data |= BBIT7;
    }
    else
    {
		#ifdef DEBUG_BCM
			printf("para err,rate_flag, function dbcm53xx_set_ratemode()\r\n",
                                0,0,0,0,0,0);        
		#endif
        return BCM_PORT_RATECTRL_PAR_ERR;

    }

     /*设置超速后的处理方法，Drop 或者返回pause frame*/
     
    if(BCM_PAUSE_FRAME_ENABLE == en_flag)
    {
        write_data &= ~BBIT6;                   /*只使用buckte0*/
    }
    else if(BCM_DROP_ENABLE == en_flag)
    {
        write_data |= BBIT6;
    }
    else
    {
        #ifdef DEBUG_BCM
			printf("para err,en_flag, function dbcm53xx_set_ratemode()\r\n",
                                0,0,0,0,0,0);        
		#endif
        return BCM_PORT_RATECTRL_PAR_ERR;
    }

     /*设置要限速的packet的类型*/

    /*禁用bucket1,bucket0与bucket1不能同时使用
     同时清bucket0中的packet type配置项，清完
     后再设置可以保证每次只选择一种packet type*/
    write_data &= ~0x3f3f;

    switch(packet_type)
    {
        case UNICAST:
                
            write_data |= BBIT0;
            break;

        case MULTICAST:

            write_data |= BBIT1;
            break;
        case BROADCAST_S:

           write_data |= BBIT2;
            break;
        case BROADCAST_L:

           write_data |= BBIT3;

            break;
        case DEST_LK_FAILED:

           write_data |= BBIT5;
            break;
        default:
            #ifdef DEBUG_BCM
			printf("para err,en_flag, function dbcm53xx_set_ratemode()\r\n",
                                    0,0,0,0,0,0);        
			#endif
            return BCM_PORT_RATECTRL_PAR_ERR;

    }
     
        /*完成设置操作*/
    if(BCM_DRV_OK != write_bcm53xx(RATE_CONTROL_PAGE, 
                                                        0x00, 
                                                        4,
                                                        (uint8 *)&write_data))
    {
        #ifdef DEBUG_BCM
			printf("twrite_data err,write_bcm53xx(), function dbcm53xx_set_ratemode()\r\n",
                                0,0,0,0,0,0);        
		#endif
        return BCM_PORT_RATECTRL_PAR_ERR;
    }
    return BCM_DRV_OK;

}
/***********************************************************
*   Function Name: dbcm53xx_get_packetdrop
*
*   Input(s):
*       port_num:        指向bcm53xx的端口

*   Output(s):
*       <none>
*
*   Returns:
*       超速后的丢包数量，最大为65535个
*
*   Description:
*       此功能接口实现bcm53xx的某个端口对应的超过限速设定值之后
        的丢包数
*

************************************************************/
uint16  dbcm53xx_get_packetdrop(uint8  port_num )
{
    uint16 read_data=0;
  
  
    if(BCM_PORT15 < port_num )
    {
		#ifdef DEBUG_BCM
			printf("port_num err, function dbcm53xx_get_packetdrop\r\n",
                                0,0,0,0,0,0);        
		#endif

        return BCM_ETH_SW_PORT_R_ERR;
    }

    if( BCM_DRV_OK != read_bcm53xx(RATE_CONTROL_PAGE, 
                                                        (uint8)(0x80 + 2*port_num), 
                                                        2, 
                                                        (uint8 *)&read_data) )
    {   
		#ifdef DEBUG_BCM
			printf("read bcm err, read_bcm53xx(),function dbcm53xx_get_packetdrop\r\n",
                                0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }


    return read_data;
}


/***********************************************************
*   Function Name: dbcm53xx_get_portvlan
*
*   Input(s):
*       port_num:        当前要设置的bcm53xx的主端口
*
*   Output(s):
*       <none>
*
*   Returns:
*       端口VLAN寄存器的值(16bit),每个bit位反映对应端口的
         连通情况。
*
*   Description:
*       此功能接口实现读取bcm53xx的某个端口VLAN寄存器的状态值，
*       可以反映出与当前port在同一VLAN之中的其他端口
*
*
************************************************************/
uint32  dbcm53xx_get_portvlan(uint8  port_num )
{

    uint32 read_data=0;
    uint8 count = 0;
    uint32 vlan_port=0;
    int i;
    if(BCM_PORT_15 < port_num )
    {   
		#ifdef DEBUG_BCM
			printf("port_num err,function dbcm53xx_get_portvlan()\r\n",
                                0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

   
    /*每个寄存器的低16bit对映每个port的与其他15个port的VLAN关系
      所以只需读取或设置2个字节就行了*/
    if( BCM_DRV_OK != read_bcm53xx(PORT_VLAN_PAGE,
                                                        (uint8)(0x00 + 4*port_num),
                                                        4,
                                                        (uint8 *)&read_data) )
    {   
		#ifdef DEBUG_BCM
			printf("read bcm  err,read_bcm53xx(),function dbcm53xx_get_portvlan()\r\n",
                                0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

    vlan_port = read_data;

    /*打印出与当前port在同一VLAN之中的其他端口*/
    
    if(0 != vlan_port)
    {
        for(i=0;i<(BCM_PORT_15 + 1);i++)
        {
            if(vlan_port & 0x0001)
            {
				#ifdef DEBUG_BCM
					printf("the port %d has the same VLAN with port %d!\r\n",
                                        port_num,count,0,0,0,0);        
				#endif
                vlan_port = vlan_port >> 1;
                count++;

            }
            else

            {
                vlan_port = vlan_port >> 1;
                count++;

            }
        }
    }
    else
    {
		#ifdef DEBUG_BCM
					printf("the VLAN config reg has been cleaned\r\n",
                                   0,0,0,0,0,0);        
		#endif
    }

    return read_data;


}


/***********************************************************
*   Function Name: dbcm53xx_clean_portvlan
*
*   Input(s):
*       port_num:        当前要设置的bcm53xx的主端口
*
*   Output(s):
*       <none>
*
*   Returns:
*       BCM_DRV_OK 或 <0 其它错误码
*
*   Description:
*       5396芯片在上电后默认所有端口的都是在同一个
*       VLAN内的，要想实现针对某个端口的VLAN绑定，
*        就需要先清寄存器，然后再单独设置。
*
*
************************************************************/

int  dbcm53xx_clean_portvlan(uint8  port_num )
{
   
    

    uint32 clean_reg = 0x0;
    if(BCM_PORT_15 < port_num )
    {   
		#ifdef DEBUG_BCM
			printf("port_num err,dbcm53xx_clean_portvlan()\r\n",
                                 0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }


    if( BCM_DRV_OK > write_bcm53xx(PORT_VLAN_PAGE, 
                                                        (uint8)(0x00 + 4*port_num), 
                                                         4, 
                                                        (uint8 *)&clean_reg) )
    {   
		#ifdef DEBUG_BCM
			printf("write bcm err,write_bcm53xx(),dbcm53xx_clean_portvlan()\r\n",
                                0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

    return BCM_DRV_OK;
  }




/***********************************************************
*   Function Name: dbcm53xx_set_portvlan
*
*   Input(s):
*       port_num:        当前要设置的bcm53xx的主端口
         vlan_port:       要设置的与主端口在同一vlan内的egress端口
*
*   Output(s):
*       <none>
*
*   Returns:
*       BCM_DRV_OK 或 <0 其它错误码
*
*   Description:
*    将主端口port_num与目的端口vlan_port，内置到同一VLAN之内，
*     "vlan_port" 作为"egress port"，"port_num" 作为"ingress port"；

************************************************************/
int  dbcm53xx_set_portvlan(uint8  port_num ,uint8  vlan_port )
{


    uint32 write_data=0x0;
    uint16 write_opposite_data=0;

    if(BCM_PORT_15 < port_num)
    { 
		#ifdef DEBUG_BCM
			printf("port_num err,dbcm53xx_set_portvlan()\r\n",
                                    0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

    if(BCM_PORT_15 < vlan_port )
    {   
		#ifdef DEBUG_BCM
			printf("vlan_port err,dbcm53xx_set_portvlan()\r\n",
                                            0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }
    /*每个寄存器的低16bit对映每个port的与其他15个port的VLAN关系
      所以只需读取或设置2个字节就行了*/
    if( BCM_DRV_OK != read_bcm53xx(PORT_VLAN_PAGE,(uint8)(0x00 + 4*port_num),4,(uint8 *)&write_data) )
    {   
		#ifdef DEBUG_BCM
			printf("read bcm err,read_bcm53xx(),dbcm53xx_set_portvlan()\r\n",
                                    0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }
	else
	{
		#ifdef DEBUG_BCM
		printf("read portvlan:0x%x.\r\n",write_data,0,0,0,0,0);        
		#endif
	}
     /*设置端口号为"vlan_port"("m")的端口与当前端口("port_num")在同一VLAN内，
     此时"vlan_port" 作为"egress port"，"port_num" 作为"ingress port"*/
    write_data |= (uint32)(0x0001 << vlan_port);

    if( BCM_DRV_OK > write_bcm53xx(PORT_VLAN_PAGE,(uint8)(0x00 + 4*port_num), 4,(uint8 *)&write_data) )
    {   
		#ifdef DEBUG_BCM
			printf("write bcm err,write_bcm53xx(),dbcm53xx_set_portvlan()\r\n",
                                    0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    } 

#if 0
    if( BCM_DRV_OK != read_bcm53xx(PORT_VLAN_PAGE, 
                                                        (uint8)(0x00 + 4*vlan_port), 
                                                        2, 
                                                        (uint8 *)&write_opposite_data) )
    {   
		#ifdef DEBUG_BCM
			printf("read bcm_opposite err,read_bcm53xx(),dbcm53xx_set_portvlan()",
                                    0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

   /*由于"vlan_port"与"port_num"已经在同一Vlan之内，而当"vlan_port"作
   为"ingress port"时，根据fpge的要求，"port_num"不能作为"egress port",
   所以需禁用对应的"port_num"*/
    write_opposite_data &=( ~(0x0001 << port_num));


    if( BCM_DRV_OK != write_bcm53xx(PORT_VLAN_PAGE, 
                                                        (uint8)(0x00 + 4*vlan_port), 
                                                        2, 
                                                        (uint8 *)&write_opposite_data) )
    {   
		#ifdef DEBUG_BCM
			printf("write bcm_opposite err,write_bcm53xx(),dbcm53xx_set_portvlan()",
                                    0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    } 
#endif

    return BCM_DRV_OK;
}

/***********************************************************
*   Function Name: dbcm53xx_clr_portvlan
*
*   Input(s):
*       port_num:        当前要设置的bcm53xx的主端口
         vlan_port:       要设置的与主端口在同一vlan内的egress端口
*
*   Output(s):
*       <none>
*
*   Returns:
*       BCM_DRV_OK 或 <0 其它错误码
*
*   Description:
*    将主端口port_num与目的端口vlan_port，内置到同一VLAN之内，
*     "vlan_port" 作为"egress port"，"port_num" 作为"ingress port"；

************************************************************/
int  dbcm53xx_clr_portvlan(uint8  port_num ,uint8  vlan_port )
{
    uint32 write_data=0x0;
    uint16 write_opposite_data=0;

    if(BCM_PORT_15 < port_num)
    {   
		#ifdef DEBUG_BCM
			printf("port_num err,dbcm53xx_clr_portvlan()\r\n",
                                    0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

    if(BCM_PORT_15 < vlan_port )
    {   
		#ifdef DEBUG_BCM
			printf("vlan_port err,dbcm53xx_clr_portvlan()\r\n",
                                            0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }
    /*每个寄存器的低16bit对映每个port的与其他15个port的VLAN关系
      所以只需读取或设置2个字节就行了*/
    if( BCM_DRV_OK != read_bcm53xx(PORT_VLAN_PAGE, 
                                                        (uint8)(0x00 + 4*port_num), 
                                                        4, 
                                                        (uint8 *)&write_data) )
    {   
		#ifdef DEBUG_BCM
			printf("read bcm err,read_bcm53xx(),dbcm53xx_clr_portvlan()\r\n",
                                    0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }
   
     /*设置端口号为"vlan_port"("m")的端口与当前端口("port_num")在同一VLAN内，
     此时"vlan_port" 作为"egress port"，"port_num" 作为"ingress port"*/
     
	write_data &=( ~(0x0001 << vlan_port));

    if( BCM_DRV_OK != write_bcm53xx(PORT_VLAN_PAGE,
                                                        (uint8)(0x00 + 4*port_num), 
                                                        4, 
                                                        (uint8 *)&write_data) )
     {   
		#ifdef DEBUG_BCM
			printf("write bcm err,write_bcm53xx(),dbcm53xx_clr_portvlan()\r\n",
                                    0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
     } 

    return BCM_DRV_OK;
}


/***********************************************************
*   Function Name: dbcm53xx_set_single_portvlan
*
*   Input(s):
*       port_num:        当前要设置的bcm53xx的主端口
         vlan_port:       要设置的与主端口在同一vlan内的egress端口
*
*   Output(s):
*       <none>
*
*   Returns:
*       BCM_DRV_OK 或 <0 其它错误码
*
*   Description:
*    将主端口port_num与目的端口vlan_port，内置到同一VLAN之内，
*     "vlan_port" 作为"egress port"，"port_num" 作为"ingress port"；
*      同时为了满足FPGA的需求，当目的端口"vlan_port"作为"ingress port"时
*      还要禁用主端口port_num作为"egress port"
*
*
************************************************************/
int  dbcm53xx_set_single_portvlan(uint8  port_num ,uint8  vlan_port )
{


    uint32 write_data=0;
    uint32 write_opposite_data=0;

    if(BCM_PORT_15 < port_num)
    {   
		#ifdef DEBUG_BCM
			printf("port_num err,dbcm53xx_set_portvlan()\r\n",
                                    0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

    if(BCM_PORT_15 < vlan_port )
    {   
		#ifdef DEBUG_BCM
			printf("vlan_port err,dbcm53xx_set_portvlan()\r\n",
                                            0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }
    /*每个寄存器的低16bit对映每个port的与其他15个port的VLAN关系
      但需读取或设置4个字节，否则无法对寄存器进行成功读写*/
    if( BCM_DRV_OK != read_bcm53xx(PORT_VLAN_PAGE, 
                                                        (uint8)(0x00 + 4*port_num), 
                                                        4, 
                                                        (uint8 *)&write_data) )
    {   
		#ifdef DEBUG_BCM
			printf("read bcm err,read_bcm53xx(),dbcm53xx_set_portvlan()\r\n",
                                    0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }
   
     /*设置端口号为"vlan_port"("m")的端口与当前端口("port_num")在同一VLAN内，
     此时"vlan_port" 作为"egress port"，"port_num" 作为"ingress port"*/
     
    write_data |= (uint32)(0x0001 << vlan_port);


    if( BCM_DRV_OK != write_bcm53xx(PORT_VLAN_PAGE,
                                                        (uint8)(0x00 + 4*port_num), 
                                                        4, 
                                                        (uint8 *)&write_data) )
     {   
		#ifdef DEBUG_BCM
			printf("write bcm err,write_bcm53xx(),dbcm53xx_set_portvlan()\r\n",
                                    0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
     } 


    printf("\r\nseting opposite port,please wait... \r\n");


    if( BCM_DRV_OK != read_bcm53xx(PORT_VLAN_PAGE, 
                                                        (uint8)(0x00 + 4*vlan_port), 
                                                        4, 
                                                        (uint8 *)&write_opposite_data) )
    {   
		#ifdef DEBUG_BCM
			printf("read bcm_opposite err,read_bcm53xx(),dbcm53xx_set_portvlan()\r\n",
                                    0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

   /*由于"vlan_port"与"port_num"已经在同一Vlan之内，而当"vlan_port"作
   为"ingress port"时，根据fpge的要求，"port_num"不能作为"egress port",
   所以需禁用对应的"port_num"*/
    write_opposite_data &=( ~(0x0001 << port_num));


    if( BCM_DRV_OK != write_bcm53xx(PORT_VLAN_PAGE, 
                                                        (uint8)(0x00 + 4*vlan_port), 
                                                        4, 
                                                        (uint8 *)&write_opposite_data) )
    {   
		#ifdef DEBUG_BCM
			printf("write bcm_opposite err,write_bcm53xx(),dbcm53xx_set_portvlan()\r\n",
                                    0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    } 
    printf("\r\nOK!\r\n");
    return BCM_DRV_OK;
}

/***********************************************************
EntryFlag  作为全局变量,用来指示哈希算法计算出的
数据表项是存贮在Entry0中还是存储在Entry1中。

EntryFlag = 0x0a :存储在Entry0中；
EntryFlag = 0x0b :存储在Entry1中；

************************************************************/


uint8 EntryFlag = 0x00;

/***********************************************************
*   Function Name: dbcm53xx_read_arl_table
*
*   Input(s):
*       pARLindex:  MacAddr[6]  所要读取地址表项中的Mac地址。     
 
*                        VlanId       所要读取地址表项中的VlanId.
*   Output(s):
*                       <none>
*
*   Returns:
*       地址表项中的属性值(包含此地址表项是否是
         静态地址的表项)
*
*   Description:
     读取5396片内的一条地址表项，判断是静态表项
     还是动态自学习表项，并返回表项内的相关属性。

************************************************************/
 int32  dbcm53xx_read_arl_table(BcmARLIndex *pARLindex)
{
    uint8 readstart1 = 0x01;  
    uint8 readstart2 = 0x81;  

    uint8 tablereg0[8] = {0};
    uint8 tablereg1[8] = {0};
    uint32 fwdreg0 = 0;
    uint32 fwdreg1 = 0;

    uint8 i,j;


    if(NULL == pARLindex )
    {   
		#ifdef DEBUG_BCM
			printf("ARLindex err,function dbcm53xx_read_arl_table()\r\n",
                                  0,0,0,0,0,0);        
		#endif
        return BCM_ARL_R_PAR_ERR;
    }

    /*第1步:写入查询索引值，MAC地址(6byte48bit)
    VLAN ID(1.5byte12bit) 暂时不用*/     
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, (uint8)(0x02), 6, 
                                       (uint8 *)&(pARLindex->MacAddr[0])) )
    {   
		#ifdef DEBUG_BCM
			printf("write MacAddr err,write_bcm53xx(),dbcm53xx_read_arl_table()\r\n",
                                0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

    /*第2步:指定读写模式:"1"为读，"0"为写
     开启读取地址表项功能，读取结果会放在Entry0或Entry1中*/
     if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, (uint8)(0x00),1, 
                                      (uint8 *)&readstart1))
     {   
		#ifdef DEBUG_BCM
			printf("write readstart err,write_bcm53xx(),dbcm53xx_read_arl_table()\r\n",
                       0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
     }

  
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,(uint8)(0x00),1, 
                                    (uint8 *)&readstart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart err,write_bcm53xx(),dbcm53xx_read_arl_table()\r\n",
                   0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }


  /*第3步:分别读取Entry0和Entry1的两个寄存器*/
  
  /*Entry0 :Table reg0*/
    if( BCM_DRV_OK != read_bcm53xx(ARL_CONTROL_PAGE,(uint8)(0x10),8,
                                   (uint8 *)&tablereg0[0]) )
    {   
		#ifdef DEBUG_BCM
			printf("read tablereg0[]  err,read_bcm53xx(),dbcm53xx_read_arl_table()\r\n",
                              0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }


    /*Entry0 :FWD reg0*/
    if( BCM_DRV_OK != read_bcm53xx(ARL_CONTROL_PAGE,(uint8)(0x18),4,
                                    (uint8 *)&fwdreg0) )
    {   
		#ifdef DEBUG_BCM
			printf("read fwdreg0  err,read_bcm53xx(),dbcm53xx_read_arl_table()\r\n",
                              0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }
    /*Entry1 :Table reg1*/
    if( BCM_DRV_OK != read_bcm53xx(ARL_CONTROL_PAGE,
                                                    (uint8)(0x20),
                                                    8,
                                                    (uint8 *)&tablereg1[0]) )
    {   
		#ifdef DEBUG_BCM
			printf("read tablereg1[]  err,read_bcm53xx(), dbcm53xx_read_arl_table()\r\n",
                              0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

    
    /*Entry1 :FWD reg1*/
    if( BCM_DRV_OK != read_bcm53xx(ARL_CONTROL_PAGE,
                                                    (uint8)(0x28),
                                                    4,
                                                    (uint8 *)&fwdreg1) )
    {   
		#ifdef DEBUG_BCM
			printf("read fwdreg1  err,read_bcm53xx(), dbcm53xx_read_arl_table()\r\n",
                              0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }


    /*判断有效地址表项是在Enrty0中还是在Entry1中,并进行相应处理*/

        if(0x01 == ((fwdreg0 >> 3) & (uint32)0x01))
        {
            if(0 == bcmp((int8 *)&(pARLindex->MacAddr[0]),(int8 *)&tablereg0[2],6) )
            {
				#ifdef DEBUG_BCM
					printf("ARL Table is in accord with Index value\r\n",
                                  0,0,0,0,0,0);        
				#endif
                if(0x01 ==((fwdreg0 >> 5) & (uint32)0x01))
                {
					#ifdef DEBUG_BCM
					printf("This ARL  Table is staticmode table)\r\n",
                                  0,0,0,0,0,0);        
					#endif
                }
                else
                {
					#ifdef DEBUG_BCM
					printf("This ARL  Table is Automatic table\r\n",
                                  0,0,0,0,0,0);        
					#endif
                }
                
                if(0x01 ==((fwdreg0 >> 3) & (uint32)0x01))
                {
					#ifdef DEBUG_BCM
					printf("This ARL  Table is Valid table\r\n",
                                  0,0,0,0,0,0);        
					#endif
                }
                else
                {
					#ifdef DEBUG_BCM
					printf("This ARL  Table is invalid table\r\n",
                                  0,0,0,0,0,0);        
					#endif
                }
                
                 EntryFlag = 0x0a;
                 
                 return fwdreg0;
            }
           
         }
         if(0x01 == ((fwdreg1 >> 3) & (uint32)0x01))
        {
            if(0 == bcmp((int8 *)&(pARLindex->MacAddr[0]),(int8 *)&tablereg1[2],6))
            {
				#ifdef DEBUG_BCM
					printf("ARL Table is in accord with Index value 2\r\n",
                                  0,0,0,0,0,0);        
				#endif
				
                if(0x01 ==((fwdreg1>> 5) & (uint32)0x01))
                {
					#ifdef DEBUG_BCM
					printf("This ARL  Table is staticmode table 2\r\n",
                                  0,0,0,0,0,0);        
					#endif
                }
                else
                {
					#ifdef DEBUG_BCM
					printf("This ARL  Table is Automatic table 2\r\n",
                                  0,0,0,0,0,0);        
					#endif
                }

				if(0x01 ==((fwdreg1 >> 3) & (uint32)0x01))
                {
					#ifdef DEBUG_BCM
					printf("This ARL  Table is Valid table :-)\r\n",
                                  0,0,0,0,0,0);        
					#endif
                }
                else
                {
					#ifdef DEBUG_BCM
					printf("This ARL  Table is invalid table 2\r\n",
                                  0,0,0,0,0,0);        
					#endif
                }
                EntryFlag = 0x0b;
                return fwdreg1;
            }
            else
            {
				#ifdef DEBUG_BCM
					printf("ARL Addr do not exist in ARL Table \r\n",
                                        0,0,0,0,0,0);        
				#endif
                return BCM_ARL_R_ENTRY1_ERR;
            }
    
        }
        else
        {
			#ifdef DEBUG_BCM
				printf("ARL Addr do not exist in ARL Table \r\n",
                                        0,0,0,0,0,0);        
			#endif
            return BCM_ETH_SW_PORT_R_ERR;
        }

    return BCM_DRV_OK;
}


/***********************************************************
*   Function Name: dbcm53xx_modify_arl
*   Input(s):
*       pARLindex:  MacAddr[6] : 所要读取地址表项中的Mac地址。     
*       VlanId   :  所要读取地址表项中的VlanId.
*       staticmode: 1:静态模式, 0:自动学习模式
*       valid:      1: ARL有效, 0:ARL无效
*       portnum:    0~15端口号
*       Output(s):  <none>
*   Returns:
*       BCM_DRV_OK 或 <0 其它错误码
*
*   Description:
*     读取5396片内的一条地址表项，并修改相关属性
*     注:4个参数都要有值，哪怕只修改一个属性，别的属性
*        保持不变，也要把别的参数与以前相同的值写进去。
************************************************************/
int  dbcm53xx_modify_arl(BcmARLIndex *indexARL, uint8 staticmode,
                                   uint8 valid, uint8 portnum)
{
    uint32 fwdreg  = 0;
    uint32 fwdreg0 = 0;
    uint32 fwdreg1 = 0;
    uint8 macvidreg0[8]= {0};
    uint8 macvidreg1[8]= {0};
    uint8 writestart1  = 0x00;
    uint8 writestart2  = 0x80;
    BcmARLIndex *prARLindex = NULL;

    prARLindex = indexARL;
    if(NULL == prARLindex )
    {   
		#ifdef DEBUG_BCM
			printf("indexARL is NULL, dbcm53xx_modify_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ARL_R_PAR_ERR;
    }
    if(0 > (fwdreg = dbcm53xx_read_arl_table(prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("ARL table do not exsit, cannot modify!\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ARL_R_PAR_ERR;
    }

    //if((prARLindex->MacAddr[0]) >= 0x80)
    if((prARLindex->MacAddr[0] & 0x01) == 0x01)
    {
		#ifdef DEBUG_BCM
			printf("need Multicast Group Forward Portmap\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ARL_CREAT_ERR;
    }  

    if((1 < staticmode) ||(1 < valid))
    {
		#ifdef DEBUG_BCM
			printf("staticmode or valid err,dbcm53xx_modify_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ARL_CREAT_ERR;
    }  

    if((15 < portnum))
    {
		#ifdef DEBUG_BCM
			printf("para portnum err,dbcm53xx_modify_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ARL_CREAT_ERR;
    }  

    if(0x0a == EntryFlag)
    {
        fwdreg0 = fwdreg;
        fwdreg0 &= 0xfff817;                    /*清相应配置位*/
        fwdreg0 |= (uint32)(staticmode << 5);    /*设置静态第bit 5位*/
        fwdreg0 |= (uint32)(valid  << 3);   /*设置有效位第bit 5位*/
        fwdreg0 |= (uint32)(portnum << 6);/*设置port num bit位共5位*/
        
        /*设置mac地址，共48bit*/
        memcpy((void*)&macvidreg0[2], &prARLindex->MacAddr[0],6);
        if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10,8, &macvidreg0[0] ))
        {   
			#ifdef DEBUG_BCM
			printf("write macvidreg0 err,dbcm53xx_modify_arl()\r\n",
                          0,0,0,0,0,0);        
			#endif
            return BCM_ARL_W_ENTRY0_ERR;
        }

        if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8 *)&fwdreg0 ))
        {   
			#ifdef DEBUG_BCM
			printf("write fwdreg0 err,dbcm53xx_modify_arl()\r\n",
                          0,0,0,0,0,0);        
			#endif
            return BCM_ARL_W_ENTRY0_ERR;
        }

        EntryFlag = 0x00;     /*清Entry指示标志位*/
    }
    else if(0x0b == EntryFlag)
    {
        fwdreg1 = fwdreg;
        fwdreg1 &= 0xfff817;                    /*清相应配置位*/
        fwdreg1|= (uint32)(staticmode << 5);    /*设置静态第bit 5位*/
        fwdreg1 |= (uint32)(valid  << 3);   /*设置有效位第bit 5位*/
        fwdreg1 |= (uint32)(portnum << 6);/*设置port num bit位共5位*/
        
        /*设置mac地址，共48bit*/
        memcpy((void*)&macvidreg1[2], &prARLindex->MacAddr[0],6);
        if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x20,8,&macvidreg1[0]))
        {   
			#ifdef DEBUG_BCM
			printf("write macvidreg1 err,dbcm53xx_modify_arl()\r\n",
                          0,0,0,0,0,0);        
			#endif
            return BCM_ARL_W_ENTRY1_ERR;
        }
        if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8 *)&fwdreg1))
        {   
			#ifdef DEBUG_BCM
			printf("write fwdreg1 err,dbcm53xx_modify_arl()\r\n",
                          0,0,0,0,0,0);        
			#endif
            return BCM_ARL_W_ENTRY1_ERR;
        }

        EntryFlag = 0x00;        /*清Entry指示标志位*/    
    }
    else
    {
		#ifdef DEBUG_BCM
			printf("arl addr do not exist in ARL Table\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ARL_R_PAR_ERR;
    }
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00,1,&writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

    return BCM_DRV_OK;
}

/***********************************************************
*   Function Name : dbcm53xx_creat_arl
*   Input(s):
*       pARLindex : MacAddr[6] : 所要读取地址表项中的Mac地址。     
*       VlanId    : 所要读取地址表项中的VlanId.
*       staticmode: 1:静态模式(无老化时间),  0:自动学习模式
*       valid     : 1:地址表项有效, 0: 地址表项无效
*       portnum   : 0~15
*   Output(s)     :<none>
*
*   Returns       :BCM_DRV_OK 或 <0 其它错误码
*   Description   :在交换芯片内创建一条新的地址表项
************************************************************/
 int  dbcm53xx_creat_arl(BcmARLIndex *pARLindex,
                                  uint8 staticmode,
                                  uint8 valid,
                                  uint8 portnum)
{
    int32 fwdreg =0;
    uint32 fwdreg0 = 0;
    uint8 macvidreg0[8]= {0};
    uint32 fwdreg1 = 0;
    uint8 macvidreg1[8]= {0};
    uint8 writestart1 = 0x00;
    uint8 writestart2 = 0x80;
    BcmARLIndex *prARLindex = NULL;

    prARLindex = pARLindex;
    if(NULL == prARLindex )
    {   
		#ifdef DEBUG_BCM
			printf("prARLindex err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ARL_R_PAR_ERR;
    }
     
    if(0 < (dbcm53xx_read_arl_table(prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("prARL has exist in ARL Table.\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ARL_EXIT_ERR;
    }

    //if((pARLindex->MacAddr[0]) >= 0x80)
    if((pARLindex->MacAddr[0] & 0x01) == 0x01)
    {
		#ifdef DEBUG_BCM
			printf("need Multicast Group Forward Portmap\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ARL_CREAT_ERR;
    }  
    
    if((1 < staticmode) ||(1 < valid))
    {
		#ifdef DEBUG_BCM
			printf("para staticmode or valid err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ARL_CREAT_ERR;
    }  

    if((15 < portnum))
    {
		#ifdef DEBUG_BCM
			printf("para portnum err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ARL_CREAT_ERR;
    }  
        
    fwdreg0 |= (uint32)(staticmode << 5);    /*设置静态第bit 5位*/
    fwdreg0 |= (uint32)(valid  << 3);        /*设置有效位第bit 5位*/
    fwdreg0 |= (uint32)(portnum << 6);       /*设置port num bit位共5位*/
        
    /*设置mac地址，共48bit*/
    memcpy((void*)&macvidreg0[2], &pARLindex->MacAddr[0],6);/*JOSHUA2011.10.26*/
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10,8,&macvidreg0[0]))
    {   
		#ifdef DEBUG_BCM
			printf("write macvidreg0 err,dbcm53xx_creat_arl()\r\n",
                          0,0,0,0,0,0);        
		#endif
        return BCM_ARL_W_ENTRY0_ERR;
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8 *)&fwdreg0 ))
    {   
		#ifdef DEBUG_BCM
			printf("write fwdreg0 err,dbcm53xx_creat_arl()\r\n",
                          0,0,0,0,0,0);        
		#endif
        return BCM_ARL_W_ENTRY0_ERR;
    }

    /*写入entry后，使能写入*/
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,(uint8)(0x00),1,&writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()\r\n",
                        0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1, &writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()\r\n",
                        0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }
    return BCM_DRV_OK;
}
/*************************** MULTICAST ****************************/


int dbcm53xx_set_multicast_arl(void)
{
    uint8  addr[8] = {0x00,0x00,0x01,0x00,0x5e,0x01,0x02,0x03};
    uint32 data1 = 0;
    uint8 writestart1 = 0x00;
    uint8 writestart2 = 0x80;
    uint8 data = 0;

    BcmARLIndex prARLindex = {0x01,0x00,0x5e,0x01,0x02,0x03,0x0};

    /*使能组播*/
    data = read_bcm53xx(PORT_CONTROL_PAGE,0x3b,1,&data);
    data |= 0x1;
    if( BCM_DRV_OK != write_bcm53xx(PORT_CONTROL_PAGE,0x3b,1,&data))
    { 
		#ifdef DEBUG_BCM
			printf("write PORT_CONTROL_PAGE err",0,0,0,0,0,0);        
		#endif
    }
#ifdef DRV_MCB
    /*MAC ADDR 0x01-0x00-0x5e-0x01-0x02-0x03*/
    if(0 < (dbcm53xx_read_arl_table(&prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("prIndex has exist in ARL Tables.\r\n",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err",0,0,0,0,0,0);        
		#endif
    }
    
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1);
    data1 |= 0x168; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }
    

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x20, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write macvidreg0 err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1);
    data1 |= 0x168; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write fwdreg0 err,dbcm53xx_creat_arl()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    /*MAC ADDR 0x01-0x00-0x5e-0x01-0x00-0x03*/
    addr[5] = 0x01;
    addr[6] = 0x00;
    addr[7] = 0x03;
    prARLindex.MacAddr[3]= 0x01;
    prARLindex.MacAddr[4] = 0x00;
    prARLindex.MacAddr[5] = 0x03;
    if(0 < (dbcm53xx_read_arl_table(&prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("prIndex has exist in ARL Tables,do not need create\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err\r\n",0,0,0,0,0,0);        
		#endif
    }
    
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1);
    data1 |= 0x128; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x20, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write macvidreg0 err,dbcm53xx_creat_arl\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1);
    data1 |= 0x128; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write fwdreg0 err,dbcm53xx_creat_arl",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    /*MAC ADDR 0x01-0x00-0x5e-0x01-0x02-0x00*/
    addr[5] = 0x01;
    addr[6] = 0x02;
    addr[7] = 0x00;
    prARLindex.MacAddr[3]= 0x01;
    prARLindex.MacAddr[4] = 0x02;
    prARLindex.MacAddr[5] = 0x00;
    if(0 < (dbcm53xx_read_arl_table(&prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("prIndex has exist in ARL Tables,do not need create\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err",0,0,0,0,0,0);        
		#endif
    }
    
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1);
    data1 |= 0x068; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x20, 8, addr))
    {  
		#ifdef DEBUG_BCM
			printf("write macvidreg0 err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1);
    data1 |= 0x068; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write fwdreg0 err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }

    /*MAC ADDR 0x01-0x00-0x5e-0x00-0x02-0x03*/
    addr[5] = 0x00;
    addr[6] = 0x02;
    addr[7] = 0x03;
    prARLindex.MacAddr[3]= 0x00;
    prARLindex.MacAddr[4] = 0x02;
    prARLindex.MacAddr[5] = 0x03;
    if(0 < (dbcm53xx_read_arl_table(&prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("prIndex has exist in ARL Tables,do not need create\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err\r\n",0,0,0,0,0,0);        
		#endif
    }
    
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1);
    data1 |= 0x168; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x20, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write macvidreg0 err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1);
    data1 |= 0x168; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write fwdreg0 err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    /*MAC ADDR 0x01-0x00-0x5e-0x01-0x00-0x00*/
    addr[5] = 0x01;
    addr[6] = 0x00;
    addr[7] = 0x00;
    prARLindex.MacAddr[3] = 0x01;
    prARLindex.MacAddr[4] = 0x00;
    prARLindex.MacAddr[5] = 0x00;
    if(0 < (dbcm53xx_read_arl_table(&prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("prIndex has exist in ARL TableS,do not need create\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err\r\n",0,0,0,0,0,0);        
		#endif
    }
    
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1);
    data1 |= 0x028; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x20, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write macvidreg0 err,bcm53xx_creat_ARL()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1);
    data1 |= 0x028; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write fwdreg0 err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,,dbcm53xx_read_arl_table()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }

    /*MAC ADDR 0x01-0x00-0x5e-0x00-0x02-0x00*/
    addr[5] = 0x00;
    addr[6] = 0x02;
    addr[7] = 0x00;
    prARLindex.MacAddr[3]= 0x00;
    prARLindex.MacAddr[4] = 0x02;
    prARLindex.MacAddr[5] = 0x00;
    if(0 < (dbcm53xx_read_arl_table(&prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("prIndex has exist in ARL TableS,do not need create\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err\r\n",0,0,0,0,0,0);        
		#endif
    }
    
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1);
    data1 |= 0x068; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x20, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write macvidreg0 err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1);
    data1 |= 0x068; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write fwdreg0 err,dbcm53xx_creat_arl()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    /*MAC ADDR 0x01-0x00-0x5e-0x00-0x00-0x03*/
    addr[5] = 0x00;
    addr[6] = 0x00;
    addr[7] = 0x03;
    prARLindex.MacAddr[3]= 0x00;
    prARLindex.MacAddr[4] = 0x00;
    prARLindex.MacAddr[5] = 0x03;
    if(0 < (dbcm53xx_read_arl_table(&prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("prIndex has exist in ARL TableS,do not need create\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err\r\n",0,0,0,0,0,0);        
		#endif
    }
    
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1);
    data1 |= 0x128; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x20, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write macvidreg0 err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1);
    data1 |= 0x128; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write fwdreg0 err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,,dbcm53xx_read_arl_table()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
#endif
#ifdef DRV_CPB
    /*收到的组播报文转发到5，6两个端口*/
    /*MAC ADDR 0x01-0x00-0x5e-0x01-0x02-0x03*/
    if(0 < (dbcm53xx_read_arl_table(&prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("prIndex has exist in ARL Tables.\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ARL_CREAT_ERR;
    }
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err\r\n",0,0,0,0,0,0);        
		#endif
        return BCM_ARL_W_ENTRY0_ERR;
    }
    
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1);
    data1 |= 0xc08; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
        return BCM_ARL_W_ENTRY0_ERR;
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

    
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x20, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write macvidreg0 err,bcm53xx_creat_ARL()\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ARL_W_ENTRY0_ERR;
    }
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1);
    data1 |= 0xc08; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write fwdreg0 err,bcm53xx_creat_ARL()\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ARL_W_ENTRY0_ERR;
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,,dbcm53xx_read_arl_table()\r\n",
                      0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }
    
    /*MAC ADDR 0x01-0x00-0x5e-0x01-0x00-0x03*/
    addr[5] = 0x01;
    addr[6] = 0x00;
    addr[7] = 0x03;
    prARLindex.MacAddr[3]= 0x01;
    prARLindex.MacAddr[4] = 0x00;
    prARLindex.MacAddr[5] = 0x03;
    if(0 < (dbcm53xx_read_arl_table(&prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("prIndex has exist in ARL Tables.\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err\r\n",0,0,0,0,0,0);        
		#endif
    }
    
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1);
    data1 |= 0xc08; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,\r\n",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x20, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write macvidreg0 err,dbcm53xx_creat_arl\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1);
    data1 |= 0xc08; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write fwdreg0 err,dbcm53xx_creat_arl",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    /*MAC ADDR 0x01-0x00-0x5e-0x01-0x02-0x00*/
    addr[5] = 0x01;
    addr[6] = 0x02;
    addr[7] = 0x00;
    prARLindex.MacAddr[3]= 0x01;
    prARLindex.MacAddr[4] = 0x02;
    prARLindex.MacAddr[5] = 0x00;
    if(0 < (dbcm53xx_read_arl_table(&prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("prIndex has exist in ARL Tables.\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err",0,0,0,0,0,0);        
		#endif
    }
    
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1);
    data1 |= 0xc08; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x20, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write macvidreg0 err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1);
    data1 |= 0xc08; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write fwdreg0 err,dbcm53xx_creat_arl()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    /*MAC ADDR 0x01-0x00-0x5e-0x00-0x02-0x03*/
    addr[5] = 0x00;
    addr[6] = 0x02;
    addr[7] = 0x03;
    prARLindex.MacAddr[3]= 0x00;
    prARLindex.MacAddr[4] = 0x02;
    prARLindex.MacAddr[5] = 0x03;
    if(0 < (dbcm53xx_read_arl_table(&prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("prIndex has exist in ARL Tables.\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err",0,0,0,0,0,0);        
		#endif
    }
    
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1);
    data1 |= 0xc08; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x20, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write macvidreg0 err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1);
    data1 |= 0xc08; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write fwdreg0 err,dbcm53xx_creat_arl()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    /*MAC ADDR 0x01-0x00-0x5e-0x01-0x00-0x00*/
    addr[5] = 0x01;
    addr[6] = 0x00;
    addr[7] = 0x00;
    prARLindex.MacAddr[3] = 0x01;
    prARLindex.MacAddr[4] = 0x00;
    prARLindex.MacAddr[5] = 0x00;
    if(0 < (dbcm53xx_read_arl_table(&prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("prIndex has exist in ARL Table.\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err",0,0,0,0,0,0);        
		#endif
    }
    
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1);
    data1 |= 0xc08; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x20, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write macvidreg0 err,bcm53xx_creat_ARL()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1);
    data1 |= 0xc08; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write fwdreg0 err,dbcm53xx_creat_arl()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    /*MAC ADDR 0x01-0x00-0x5e-0x00-0x02-0x00*/
    addr[5] = 0x00;
    addr[6] = 0x02;
    addr[7] = 0x00;
    prARLindex.MacAddr[3]= 0x00;
    prARLindex.MacAddr[4] = 0x02;
    prARLindex.MacAddr[5] = 0x00;
    if(0 < (dbcm53xx_read_arl_table(&prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("prIndex has exist in ARL Table.\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err",0,0,0,0,0,0);        
		#endif
    }
    
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1);
    data1 |= 0xc08; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x20, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write macvidreg0 err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1);
    data1 |= 0xc08; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write fwdreg0 err,dbcm53xx_creat_arl()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    /*MAC ADDR 0x01-0x00-0x5e-0x00-0x00-0x03*/
    addr[5] = 0x00;
    addr[6] = 0x00;
    addr[7] = 0x03;
    prARLindex.MacAddr[3]= 0x00;
    prARLindex.MacAddr[4] = 0x00;
    prARLindex.MacAddr[5] = 0x03;
    if(0 < (dbcm53xx_read_arl_table(&prARLindex)))
    {
		#ifdef DEBUG_BCM
			printf("prIndex has exist in ARL Table.\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x10, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err",0,0,0,0,0,0);        
		#endif
    }
    
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1);
    data1 |= 0xc08; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x18,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write ARL_CONTROL_PAGE err,",0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x20, 8, addr))
    {   
		#ifdef DEBUG_BCM
			printf("write macvidreg0 err,dbcm53xx_creat_arl()\r\n",
                      0,0,0,0,0,0);        
		#endif
    }
    data1 = read_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1);
    data1 |= 0xc08; 
    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x28,4,(uint8*)&data1))
    {   
		#ifdef DEBUG_BCM
			printf("write fwdreg0 err,dbcm53xx_creat_arl()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE, 0x00, 1, &writestart1))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

    if( BCM_DRV_OK != write_bcm53xx(ARL_CONTROL_PAGE,0x00,1,&writestart2))
    {   
		#ifdef DEBUG_BCM
			printf("write readstart0 err,,dbcm53xx_read_arl_table()",
                      0,0,0,0,0,0);        
		#endif
    }

#endif
    return 0;
}
/******************************************************************/

/************************************************************/


int dbcm53xx_agetime_set(uint32 agetime)
{
    uint32 write = 0;

    if(0x08 > agetime)
    {
		#ifdef DEBUG_BCM
			printf("agetime is too little for config,need 8 sec at least...\r\n",
                                  0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }
    if( BCM_DRV_OK != read_bcm53xx(GLOBAL_MANAGEMNT_PAGE,
                                                        (uint8)(0x02),
                                                        4,
                                                        (uint8 *)&write ))
        {   
			#ifdef DEBUG_BCM
			printf("read age time  err,read_bcm53xx(),function dbcm53xx_agetime_set()\r\n",
                                  0,0,0,0,0,0);        
			#endif
            return BCM_ETH_SW_PORT_R_ERR;
        }
  
    write &= 0xfff00000;   /*清除老化时间*/
    write |= agetime;
    
    if( BCM_DRV_OK != write_bcm53xx(GLOBAL_MANAGEMNT_PAGE,
                                                            (uint8)(0x02),
                                                            4,
                                                            (uint8 *)&write ))
    {   
		#ifdef DEBUG_BCM
			printf("read age time  err,read_bcm53xx(),function dbcm53xx_agetime_set()\r\n",
                              0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }


  return BCM_DRV_OK;

}

/***********************************************************
*   Function Name: dbcm53xx_set_jumbo
*
*   Input(s):
*        port_num1:  端口号0~15
*        port_num2:  端口号0~15
*        jumbo_enable1:  1 使能jumbo功能 
                        0 不使能jumbo功能(注:如果要关闭已经打开的jumbo功能，
                                            请使用bcm53xx_clear_jumbo函数)
*        jumbo_enable2:  1 使能jumbo功能 
*                        0 不使能jumbo功能(注:如果要关闭已经打开的jumbo功能，
                                            请使用bcm53xx_clear_jumbo函数)
*   Output(s):
*                       <none>
*
*   Returns:
*      OK  或  错误码
*
*   Description:
*    设置打开某个端口的jumbo功能，每调用一次可打开2个，但在关闭某个端口的
*    jumbo功能时，需调用bcm53xx_clear_jumbo()

************************************************************/

int dbcm53xx_set_jumbo(uint8  port_num1,uint8 jumbo_enable1,uint8  port_num2,
                                                                uint8 jumbo_enable2)
{
    uint32 write = 0;

    if(15 <port_num1 ||1 <jumbo_enable1 ||15 <port_num2 ||1 <jumbo_enable2)
    {
		#ifdef DEBUG_BCM
			printf("param err,portnum or jumbo_enable....\r\n",
                                  0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;


    }
    if( BCM_DRV_OK != read_bcm53xx(JUMBO_CONTROL_PAGE,
                                                        (uint8)(0x01),
                                                        4,
                                                        (uint8 *)&write ))
        {   
			#ifdef DEBUG_BCM
			printf("read jumbo contrl err,read_bcm53xx(),function dbcm53xx_set_jumbo()\r\n",
                                  0,0,0,0,0,0);        
			#endif
            return BCM_ETH_SW_PORT_R_ERR;
        }
  
    /*write &= 0xffff0000; */  /*清除上次配置*/
    write |= (uint32)(jumbo_enable1 << port_num1);
    write |= (uint32)(jumbo_enable2 << port_num2);
    
    
    if( BCM_DRV_OK != write_bcm53xx(JUMBO_CONTROL_PAGE,
                                                            (uint8)(0x01),
                                                            4,
                                                            (uint8 *)&write ))
    {   
		#ifdef DEBUG_BCM
			printf("write jumbo_enable  err,write_bcm53xx(),function dbcm53xx_set_jumbo()\r\n",
                              0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }


  return BCM_DRV_OK;

}
/***********************************************************
*   Function Name: dbcm53xx_clear_jumbo
*
*   Input(s):
*       uint8  port_num   端口号0~15
*   Output(s):
*                       <none>
*
*   Returns:
*      OK  或  错误码
*
*   Description:
*    关闭某个端口的jumbo功能

************************************************************/

int dbcm53xx_clear_jumbo(uint8  port_num)
{
    uint32 write = 0;

    if(15 <port_num)
    {
		#ifdef DEBUG_BCM
			printf("param err,portnum ....\r\n",
                                  0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }
    if( BCM_DRV_OK != read_bcm53xx(JUMBO_CONTROL_PAGE,
                                                        (uint8)(0x01),
                                                        4,
                                                        (uint8 *)&write ))
        {   
			#ifdef DEBUG_BCM
			printf("read jumbo contrl err,read_bcm53xx(),function dbcm53xx_set_jumbo()\r\n",
                                  0,0,0,0,0,0);        
			#endif
            return BCM_ETH_SW_PORT_R_ERR;
        }
  

    write &= ~((uint32)(0x01 << port_num));
    
    
    if( BCM_DRV_OK != write_bcm53xx(JUMBO_CONTROL_PAGE,
                                                            (uint8)(0x01),
                                                            4,
                                                            (uint8 *)&write ))
    {   
		#ifdef DEBUG_BCM
			printf("write jumbo_enable  err,write_bcm53xx(),function dbcm53xx_set_jumbo()\r\n",
                              0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }


  return BCM_DRV_OK;

}

/***********************************************************
*   Function Name: dbcm53xx_select_mib_mode
*
*   Input(s):
*                     port_num : 端口号0~15。     
* 
*                        group   :   0:某个端口上的两个发送统计寄存器
*                                          和两个接受统计寄存器(细节见下个函数)       
*                                       1:某个端口上的两个发送统计寄存器
*                                          和两个接受统计寄存器(细节见下个函数)   
*                                       2:某个端口上的两个发送统计寄存器
*                                          和两个接受统计寄存器(细节见下个函数)   

*   Output(s):               
*                       <none>
*
*   Returns:
                        BCM_DRV_OK 或 <0 其它错误码

*   Description:
                     交换芯片上每个端口都有4个MIB寄存器，两个
                     RX寄存器，两个TX寄存器。但是这四个reg在不同
                     的工作模式下，所体现的作用不一样。一共
                     有3种工作模式:group0/group1/group2

************************************************************/

int dbcm53xx_select_mib_mode(uint8 port_num,uint8 group)
{

    uint64 write = 0;
    uint64 read = 0;

    if(15 <port_num ||2 <group)
    {
		#ifdef DEBUG_BCM
			printf("param err,portnum or group....\r\n",
                                  0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;


    }
    if( BCM_DRV_OK != read_bcm53xx(GLOBAL_MANAGEMNT_PAGE,
                                                        (uint8)(0x04),
                                                        8,
                                                        (uint8 *)&write ))
        {   
			#ifdef DEBUG_BCM
			printf("read MIB select reg err,read_bcm53xx(),function dbcm53xx_select_mib_mode()\r\n",
                                  0,0,0,0,0,0);        
			#endif
            return BCM_ETH_SW_PORT_R_ERR;
        }

    printf("\r\nthe value of mib mode is 0x%llx\r\n",write);

    write &=~ (((uint64)0x03) << (port_num*2));   /*清除当前配置*/
    write |= (((uint64)group) << (port_num*2));


    if( BCM_DRV_OK != write_bcm53xx(GLOBAL_MANAGEMNT_PAGE,
                                                            (uint8)(0x04),
                                                            8,
                                                            (uint8 *)&write ))
    {   
		#ifdef DEBUG_BCM
			printf("write MIB select reg  err,write_bcm53xx(),function dbcm53xx_select_mib_mode()\r\n",
                              0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;
    }


     if( BCM_DRV_OK != read_bcm53xx(GLOBAL_MANAGEMNT_PAGE,
                                                        (uint8)(0x04),
                                                        8,
                                                        (uint8 *)&read ))
        {   
			#ifdef DEBUG_BCM
			printf("read MIB select reg err,read_bcm53xx(),function dbcm53xx_select_mib_mode()\r\n",
                                  0,0,0,0,0,0);        
			#endif
            return BCM_ETH_SW_PORT_R_ERR;
        }

    printf("\r\nthe value of mib new mode is 0x%llx\r\n",read);


  return BCM_DRV_OK;

}

/***********************************************************
*   Function Name: dbcm53xx_read_MIB_value
*
*   Input(s):
*                     port_num : 端口号0~15。     
* 
*                        reg   :       0: 在group0 mode下为:某个端口所TX
*                                            的正常的(less than 1522)packet数量。
*                                             在group1 mode下为:某个端口在传输
*                                            的过程中所记录的拥塞packet数量。
*                                             在group2 mode下为:某个端口在发送
*                                            过程中由于FIFO underflow而丢弃的packet。
*                                             
*                                       1: 在group0 mode下为:某个端口所TX
*                                            的正常的(less than 1522)单播packet数量。
*                                             在group1 mode下为:某个端口所TX
*                                            的正常的byte数量。
*                                             在group2 mode下为:某个端口所TX
*                                            的pause 桢数量。
*
*                                       2: 在group0 mode下为:某个端口所RX
*                                             的正常的(less than 1522)packet数量。
*                                             在group1 mode下为:某个端口所RX
*                                            的错误的FCS packet数量。
*                                            在group2 mode下为:某个端口在RX
*                                           过程中由于Buffer full而丢弃的packet
*                                       3: 在group0 mode下为:某个端口所RX
*                                             的正常的(less than 1522)单播packet数量。
*                                             在group1 mode下为:某个端口所RX
*                                            的正常的(less than 1522)packet数量。
*                                             在group2 mode下为:某个端口所RX的
*                                             正常的Jumbo(more than 1522)packet数量
*                                             
*
*   Output(s):               
*                       <none>
*
*   Returns:
                        某个MIB reg的值

*   Description:
                     交换芯片上每个端口都有4个MIB寄存器，两个
                     RX寄存器，两个TX寄存器。但是这四个reg在不同
                     的工作模式下，所体现的作用不一样。一共
                     有3种工作模式:group0/group1/group2

************************************************************/

uint32 dbcm53xx_read_MIB_value(uint8  port_num,uint8 reg)
{
    uint32 MIBvalue = 0;

    if(15 <port_num ||3 <reg)
    {
		#ifdef DEBUG_BCM
			printf("param err,portnum or reg....\r\n",
                                  0,0,0,0,0,0);        
		#endif
        return BCM_ETH_SW_PORT_R_ERR;


    }
    if( BCM_DRV_OK != read_bcm53xx((0x50+0x01*port_num),
                                                        (uint8)(0x04*reg),
                                                        4,
                                                        (uint8 *)&MIBvalue ))
	{   
		#ifdef DEBUG_BCM
		printf("read MIB value err,read_bcm53xx(),function dbcm53xx_read_MIB_value()\r\n",
							  0,0,0,0,0,0);        
		#endif
		return BCM_ETH_SW_PORT_R_ERR;
	}

    #ifdef DEBUG_BCM
	printf("The MIB reg%d 's value of port %d is %x\r\n",
                                  reg,port_num,MIBvalue,0,0,0);        
	#endif
    return MIBvalue;

}
/***********************************************************
*   Function Name: dbcm53xx_clear_mib_value
*
*   Input(s):
*                        <none>
*   Output(s):
*                       <none>
*
*   Returns:
*      OK  或  错误码
*
*   Description:
*    清所有MIB计数寄存器

************************************************************/

int dbcm53xx_clear_mib_value(void)
{
    uint8 clearMIB1 = 0x01;
    uint8 clearMIB2 = 0x00;


    if( BCM_DRV_OK != write_bcm53xx(GLOBAL_MANAGEMNT_PAGE,
                                                        (uint8)(0x00),
                                                        1,
                                                        (uint8 *)&clearMIB1))
        {   
			#ifdef DEBUG_BCM
			printf("write clearMIB value err,write_bcm53xx(),function dbcm53xx_clear_mib_value()\r\n",
                                  0,0,0,0,0,0);        
			#endif
            return BCM_ETH_SW_PORT_R_ERR;
        }
    
    if( BCM_DRV_OK != write_bcm53xx(GLOBAL_MANAGEMNT_PAGE,
                                                        (uint8)(0x00),
                                                        1,
                                                        (uint8 *)&clearMIB2 ))
	{   
		#ifdef DEBUG_BCM
		printf("write clearMIB value err,write_bcm53xx(),function dbcm53xx_clear_mib_value()\r\n",
							  0,0,0,0,0,0);        
		#endif
		return BCM_ETH_SW_PORT_R_ERR;
	}
  
	#ifdef DEBUG_BCM
	printf("The MIB reg reset OK!\r\n",
                                  0,0,0,0,0,0);        
	#endif
    return BCM_DRV_OK;

}


/* 查看 switch 各端口的ber、crc error和 */
int dbcm53xx_show_sw_ber(void)
{
    int i = 0;
    uint16 data_rev = 0;
    for(i = 0; i < 16; i++)
    {
        data_rev = 0;
        read_bcm53xx((0x10 + i), 0x2e, 2, (uint8 *)&data_rev);
		
        #ifdef DEBUG_BCM
        printf("switch5396 port%d crc error: %d,ber error: %d \r\n",i,(data_rev&0xff),(data_rev&0xff00)>>8,0,0,0);        
        #endif
    }
    usleep(2000);
    return BCM_DRV_OK;
}

