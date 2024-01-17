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

#include "bcm53xxdrv.h"

 
int main(int argc,char** argv)
{
	if (argc < 3)
	{
		printf("Please enter the correct parameters\r\n");
		return -1;
	}
	
	switch (atoi(argv[1]))
	{
	case 0://获取端口vlan
		dbcm53xx_get_portvlan(atoi(argv[2]));
		break;
	case 1://清除端口vlan
		dbcm53xx_clean_portvlan(atoi(argv[2]));
		break;
	case 2://设置端口vlan
		dbcm53xx_set_portvlan(atoi(argv[2]),atoi(argv[3]));
		break;
	default:
		break;
	}
	return 0;
}