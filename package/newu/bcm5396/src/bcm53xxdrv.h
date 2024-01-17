/*===========================================================================

Copyright (c) 2009, CPIT. All Rights Reserved.

FILENAME:   bcm53xxdrv.h
 
MODULE NAME:mcb driver module

DESCRIPTION:
            Source Code of bcm53xx Driver.

HISTORY:

Date        CR No           Person           Description
----------  ------------    ------------     -------------

2009/4/20                 zhaojianli        created

2011/8/5                  liangxin           modify
histrory: 

==============================================================================*/



#ifndef __BCM53XXDRV_H__
#define __BCM53XXDRV_H__
    
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

#define DEBUG_BCM
#define PORT_CONTROL_PAGE                            0x00
#define MIRROR_CPATURE_ENABLE_PAGE          0x02
#define RATE_CONTROL_PAGE                       0x41                         /*Joshua*/
#define PORT_VLAN_PAGE                            0x31
#define ARL_CONTROL_PAGE                              0x05
#define GLOBAL_MANAGEMNT_PAGE                   0x02
#define JUMBO_CONTROL_PAGE                         0x40

/*****************************错误码定义**************************/

#define BCM_ERR_BASE                                    -70000
#define BCM_ETH_SW_PORT_R_ERR                     (BCM_ERR_BASE-1)
#define BCM_ETH_SW_PORT_RX_ERR                  (BCM_ERR_BASE-2)
#define BCM_ETH_SW_PORT_TX_ERR                  (BCM_ERR_BASE-3)
#define BCM_ETH_SW_PORT_TX_RX_ERR           (BCM_ERR_BASE-4)
#define BCM_ETH_SW_PORT_ERR                     (BCM_ERR_BASE-5)
#define BCM_PORT_ENABLE_PAR_ERR             (BCM_ERR_BASE-6)
#define BCM_MIRROR_ENABLE_PAR_ERR           (BCM_ERR_BASE-7)
#define MIRROR_ENABLE_R_ERR                 (BCM_ERR_BASE-8)
#define BCM_MIRROR_ENABLE_SET_ERR           (BCM_ERR_BASE-9)
#define MIRROR_BLOCK_R_ERR                  (BCM_ERR_BASE-10)
#define BCM_MIRROR_BLOCK_PAR_ERR            (BCM_ERR_BASE-11)
#define BCM_MIRROR_BLOCK_SET_ERR            (BCM_ERR_BASE-12)
#define MIRROR_CAPTURE_R_ERR                (BCM_ERR_BASE-13)
#define BCM_MIRROR_CAPTURE_SET_ERR          (BCM_ERR_BASE-14)
#define PAR_MIRROR_CAPTURE_SET_ERR          (BCM_ERR_BASE-15)
#define BCM_MIRROR_MASK_1_SET_ERR           (BCM_ERR_BASE-16)
#define BCM_MIRROR_MASK_2_SET_ERR           (BCM_ERR_BASE-17)
#define BCM_MIRROR_MASK_3_SET_ERR           (BCM_ERR_BASE-18)
#define BCM_MIRROR_MASK_4_SET_ERR           (BCM_ERR_BASE-19)
#define MIRROR_MASK_EG_ERR                  (BCM_ERR_BASE-20)
#define PAR_MIRROR_MASK_SET_ERR             (BCM_ERR_BASE-21)
#define MIRROR_MASK_IN_ERR                  (BCM_ERR_BASE-22)
#define PAR_MIRROR_SET_ERR                  (BCM_ERR_BASE-23)
#define MIRROR_CAP_SET_ERR                  (BCM_ERR_BASE-24)
#define MIRROR_MIRROR_MASK_SET_ERR          (BCM_ERR_BASE-25)
#define BCM_PORT_RATECTRL_PAR_ERR             (BCM_ERR_BASE-26)   /*Joshua*/
#define BCM_PORT_BUCKET_PAR_ERR             (BCM_ERR_BASE-27)   /*Joshua*/
#define BCM_ARL_R_PAR_ERR                         (BCM_ERR_BASE-28)  
#define BCM_ARL_R_ENTRY0_ERR                         (BCM_ERR_BASE-29)
#define BCM_ARL_R_ENTRY1_ERR                         (BCM_ERR_BASE-30)
#define BCM_ARL_W_ENTRY0_ERR                         (BCM_ERR_BASE-31)
#define BCM_ARL_W_ENTRY1_ERR                         (BCM_ERR_BASE-32)
#define BCM_ARL_CREAT_ERR                         (BCM_ERR_BASE-33)
#define BCM_ARL_EXIT_ERR                    (BCM_ERR_BASE-34)

/*****************************错误码定义结束*********************/




#define BCM_DRV_OK                 0
#define BCM_DRV_ERR               -1

#define BBIT0               0x01
#define BBIT1               0x02
#define BBIT2               0x04
#define BBIT3               0x08
#define BBIT4               0x10
#define BBIT5               0x20
#define BBIT6               0x40
#define BBIT7               0x80
#define BBIT8               0x100
#define BBIT9               0x200
#define BBIT10              0x400
#define BBIT11              0x800
#define BBIT12              0x1000
#define BBIT13              0x2000
#define BBIT14              0x4000
#define BBIT15              0x8000
#define BBIT16              0x10000
#define BBIT17              0x20000
#define BBIT18              0x40000
#define BBIT19              0x80000
#define BBIT20              0x100000
#define BBIT21              0x200000
#define BBIT22              0x400000
#define BBIT23              0x800000
#define BBIT24              0x1000000
#define BBIT25              0x2000000
#define BBIT26              0x4000000
#define BBIT27              0x8000000
#define BBIT28              0x10000000
#define BBIT29              0x20000000
#define BBIT30              0x40000000
#define BBIT31              0x80000000


#define MIRROR_BACK_BOARD0  BBIT0
#define MIRROR_BACK_BOARD1  BBIT1
#define MIRROR_RIB          BBIT2
#define MIRROR_CPB5         BBIT3
#define MIRROR_CPB4         BBIT4
#define MIRROR_CPB3         BBIT5
#define MIRROR_CPB2         BBIT6
#define MIRROR_CPB1         BBIT7
#define MIRROR_DEBUG        BBIT8 
#define MIRROR_CPU          BBIT9


#define BCM_PORT_15       15      /*JOSHUA*/
#define   MAX_RATE_COUNT 240


#define BUCK_16K       0x00                    /*Joshua*/
#define BUCK_20K       0x01
#define BUCK_28K       0x02
#define BUCK_40K       0x03
#define BUCK_76K       0x04
#define BUCK_140K      0x05
#define BUCK_268K      0x06
#define BUCK_500K      0x07


/*----------------------------------枚举，结构体定义----------------*/

typedef enum
    {
        BCM_PORT0 = 0,
        BCM_PORT1,
        BCM_PORT2,
        BCM_PORT3,
        BCM_PORT4,
        BCM_PORT5,
        BCM_PORT6,
        BCM_PORT7,
        BCM_PORT8,
        BCM_PORT9,
        BCM_PORT10,
        BCM_PORT11,
        BCM_PORT12,
        BCM_PORT13,
        BCM_PORT14,
        BCM_PORT15
    }BcmPortEnumType;

typedef enum 
{

    BCM_ENABLE = 0,                         /* 指示使能 */
    BCM_DISABLE = 1                         /* 指示禁止 */
    
} EnableEnumType;

typedef enum 
{
    BCM_RATE_DISABLE = 0,                       /* 指示禁止 */
    BCM_RATE_ENABLE = 1                         /* 指示使能 */
    
} EnableRateEnumType;

typedef enum 
{
    BCM_RX = 0x01,                      /* 指示接口的接收方向 */
    BCM_TX = 0x02,                      /* 指示接口的发送方向 */
    BCM_RX_TX = 0x3                     /* 指示接口的接收和发送方向 */
   
} DirectTypeEnumType;


typedef enum                                 /*采用丢包方式还是返回pause帧的方式Joshua*/
{
    BCM_PAUSE_FRAME_ENABLE = 0,
    BCM_DROP_ENABLE = 1
}BcmDropMode;




typedef enum                                 /*要限速的packet的类型Joshua*/
{

    UNICAST = 0,
    MULTICAST,
    BROADCAST_S,
    BROADCAST_L,
    DEST_LK_FAILED

}BcmRateCtrlPacketType;

typedef enum                                 /*限速模式Joshua*/
{
    BCM_ABSOLUTE_RATE = 0,
    BCM_LINK_SPEED= 1
}BcmBitRateMode;


typedef struct
{
    uint8 MacAddr[6];
    uint16 VlanId;
    
}BcmARLIndex;

/*
------------------------------------------------------------------------------
*
*   Function Name: dbcm53xx_port_enable
*
*   Input(s):
*       port_num:	    指向bcm53xx的端口
*       rx_tx_type:  	指向输入输出
*        en_flag:       	使能或禁止 0(使能) 1(禁止)
*
*   Output(s):
*       <none>
*
*   Returns:
*       BCM_DRV_OK 或 <0 其它错误码
*       
*
*   Description:
*       此功能接口实现bcm53xx的端口的关闭或者使能
*
------------------------------------------------------------------------------
*/

int  dbcm53xx_port_enable(BcmPortEnumType port_num, DirectTypeEnumType rx_tx_type,EnableEnumType en_flag);

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

int  dbcm53xx_mirror_enable(EnableEnumType en_flag);
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
*       此功能接口实现bcm53xx 被mirror的端口的指定。
*
------------------------------------------------------------------------------
*/

int  dbcm53xx_mirror_block(EnableEnumType en_flag);
/*
------------------------------------------------------------------------------
*
*   Function Name: dbcm53xx_mirror_capture_set
*
*   Input(s):
*      
*    port_num:   要指定的作为mirror capture port 的端口
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

int  dbcm53xx_mirror_capture_set(BcmPortEnumType port_num);
/*
------------------------------------------------------------------------------
*
*   Function Name: dbcm53xx_mirror_mask_set
*
*   Input(s):
*      
*    port_num:   要指定的被mirror的端口，一次只能指定一个端口
*
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

int  dbcm53xx_mirror_mask_set(DirectTypeEnumType rx_tx_type, uint32 port_num);
/*
------------------------------------------------------------------------------
*
*   Function Name: dbcm53xx_port_mirror_set
*
*   Input(s):
*       rx_tx_type:  	指向输入输出
*       src_port:   指定的被mirror的端口.
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

int dbcm53xx_port_mirror_set(uint32 src_port, BcmPortEnumType dest_port, DirectTypeEnumType rx_tx_type);


extern int dbcm53xx_show_bucket(uint8  port_num );

extern int dbcm53xx_set_bucket(uint8  port_num ,uint8 bucket_size,
                                                                EnableRateEnumType en_flag,
                                                                uint32 rate_count);

extern int dbcm53xx_show_ratemode();
extern int dbcm53xx_set_ratemode(BcmBitRateMode rate_flag,BcmDropMode en_flag,
                                                                BcmRateCtrlPacketType packet_type);

extern uint16  dbcm53xx_get_packetdrop(uint8  port_num );

extern uint32  dbcm53xx_get_portvlan(uint8  port_num );

extern int  dbcm53xx_clean_portvlan(uint8  port_num );

extern int  dbcm53xx_set_portvlan(uint8  port_num ,uint8  vlan_port );
extern int  dbcm53xx_clr_portvlan(uint8  port_num ,uint8  vlan_port );


extern int  dbcm53xx_set_single_portvlan(uint8  port_num ,uint8  vlan_port );
int read_bcm53xx(unsigned char page,unsigned char offset,unsigned char byte_cnt, unsigned char *pbuffer);
int write_bcm53xx(unsigned char page,unsigned char offset,unsigned char byte_cnt,unsigned char *pBuffer);
extern int32  dbcm53xx_read_arl_table(BcmARLIndex *pARLindex);
extern int  bcm53xx_modify_ARL(BcmARLIndex *indexARL,
                                                            
                                                    uint8 staticmode,
                                                    uint8 valid,
                                                    uint8 portnum);
extern int  dbcm53xx_creat_arl(BcmARLIndex *pARLindex,
                                        uint8 staticmode,
                                        uint8 valid,
                                        uint8 portnum);
extern int dbcm53xx_agetime_set(uint32 agetime);

extern int dbcm53xx_set_jumbo(uint8  port_num1,uint8 jumbo_enable1,uint8  port_num2,
                                                                uint8 
                                                                jumbo_enable2);

    
extern int dbcm53xx_select_mib_mode(uint8 port_num,uint8 group);

extern uint32 dbcm53xx_read_MIB_value(uint8  port_num,uint8 reg);

extern int dbcm53xx_clear_mib_value(void);

extern int dbcm53xx_clear_jumbo(uint8  port_num);
int dbcm53xx_set_multicast_arl(void);
#ifdef __cplusplus
}
#endif 

#endif /* __BCM53XXDRV_H__ */


