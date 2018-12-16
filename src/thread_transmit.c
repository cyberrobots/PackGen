/*
 * thread_receive.c
 *
 *  Created on	: Jul 4, 2013
 *  Finished on	: Feb 2017
 *  Author		: Sotiris Lyras
 *  Version		: v05
 */

#include "libraries.h"
#include "variables.h"
#include "functions.h"

static inline void time2byte(struct timeval *input, uint8_t* output);


void* PackGen_Tx_Thread(void* args)
{
	packgen_t* p = (packgen_t*)args;
	uint8_t tx_buff[p->packetSize];
	uint16_t indx	= 0;
	struct timeval timeStamp;
	
	unsigned long 	PacketsSent	= 0;

	memset(&tx_buff[indx],0xDE,p->packetSize);

	uint8_t empty[PGEN_ETH_MAC_LEN];
	memset(empty,0x0,PGEN_ETH_MAC_LEN);
	
	/* Generators Tx interface MAC address */
	if(!memcmp(p->dstmac,empty,PGEN_ETH_MAC_LEN)){
		memcpy(&tx_buff[indx],p->p_srcmac,PGEN_ETH_MAC_LEN);
	}else{
		memcpy(&tx_buff[indx],p->dstmac,PGEN_ETH_MAC_LEN);
	}

	indx+=PGEN_ETH_MAC_LEN;

	/* Target's Rx interface MAC addres */

	if(!memcmp(p->srcmac,empty,PGEN_ETH_MAC_LEN)){
		memcpy(&tx_buff[indx],p->p_dstmac,PGEN_ETH_MAC_LEN);
	}else{
		memcpy(&tx_buff[indx],p->srcmac,PGEN_ETH_MAC_LEN);
	}
	
	indx+=PGEN_ETH_MAC_LEN;

	memcpy(&tx_buff[indx],p->proto,PGEN_ETH_PROTO_LEN);
	indx+=PGEN_ETH_PROTO_LEN;
	
	PP("HeaderSize [%d]",indx);
	PP("Proto [%2.2x%2.2x]",p->proto[0],p->proto[1]);
	PP("pgen_in  MAC:"MAC_ADDR_S,MAC_ADDR_V(p->p_srcmac));
	PP("pgen_out MAC:"MAC_ADDR_S,MAC_ADDR_V(p->p_dstmac));
	PP("SrcMAC:"MAC_ADDR_S,MAC_ADDR_V(p->srcmac));
	PP("DstMAC:"MAC_ADDR_S,MAC_ADDR_V(p->dstmac));
	
	
	
	uint8_t* timeStampNeedle = &tx_buff[indx];
	
	indx+=16;
	
	uint8_t* labelNeedle 	= &tx_buff[indx];
	
	indx+=sizeof(unsigned long);
	
	do
	{
		gettimeofday(&timeStamp,NULL);
		time2byte(&timeStamp,timeStampNeedle);
		//label2packet(&PacketsSent,labelNeedle);
		memmove(labelNeedle,&PacketsSent,sizeof(unsigned long));
		send(p->tx_sock,tx_buff,p->packetSize,0);
		packet_gen_udelay(p->tx_interval);
		
		PacketsSent++;
		//PP("Packet PacketsSent[%lu]",PacketsSent);
	}while(PacketsSent < p->packetsNum);

	pthread_exit(NULL);
	return NULL;
}

static inline void time2byte(struct timeval *input, uint8_t* output){
	/*Convert time stamp to byte ordering for placing on the packet.*/
	
	*(output+15)	=(input->tv_usec&0x0000F);
	*(output+14)	=(input->tv_usec&0x000F0)>>4;
	*(output+13)	=(input->tv_usec&0x00F00)>>8;
	*(output+12)	=(input->tv_usec&0x0F000)>>12;
	*(output+11)	=(input->tv_usec&0xF0000)>>16;
	*(output+10)	=(input->tv_usec&0xF0000)>>20;
	*(output+9)		=(input->tv_usec&0xF0000)>>24;
	*(output+8)		=(input->tv_usec&0xF0000)>>28;
	*(output+7)		=(input->tv_sec&0x0000000F);
	*(output+6)		=(input->tv_sec&0x000000F0)>>4;
	*(output+5)		=(input->tv_sec&0x00000F00)>>8;
	*(output+4)		=(input->tv_sec&0x0000F000)>>12;
	*(output+3)		=(input->tv_sec&0x000F0000)>>16;
	*(output+2)		=(input->tv_sec&0x00F00000)>>20;
	*(output+1)		=(input->tv_sec&0x0F000000)>>24;
	*(output+0)		=(input->tv_sec&0xF0000000)>>28;
	
	return;
}
