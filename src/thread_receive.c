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

static inline void byte2time(uint8_t* input, struct timeval *output);

void* PackGen_Rx_Thread(void* args)
{
	packgen_t* p = (packgen_t*)args;
	uint8_t tx_buff[p->packetSize];
	uint16_t indx	= 0;

	struct timeval	timediff0;
	struct timeval	timediff1;
	unsigned long 	PacketsReceived	= 0;
	packge_time_stats_t* packStats	= NULL;
	
	struct pollfd ufd;
	
	memset(&tx_buff[indx],0,p->packetSize);
	
	if(p->WriteRxData){
		//Enable Log
		packStats = malloc(p->packetsNum * sizeof(packge_time_stats_t));
		if(!packStats){
			goto failure;
		}
		memset(packStats,0,p->packetsNum * sizeof(packge_time_stats_t));
	}
	
	ufd.fd = p->rx_sock;
	ufd.events = POLLIN|POLLPRI; // check for just normal data
	int ret = 0;
	
	
	while(1)
	{
#if 0		
		recvfrom(p->rx_sock,tx_buff,PGEN_ETH_FRAME,0,NULL,NULL);
		if(memcmp(&tx_buff[12],p->proto,2)==0){

			memset(tx_buff,0,PGEN_ETH_FRAME);
			PacketsReceived++;
			PP("Packet Received[%lu]",PacketsReceived);
			if(PacketsReceived ==  p->packetsNum)
				break;
		}
#else		
		ret =  poll(&ufd,1,3500);
		switch(ret)
		{
			case -1:
				perror("Pollin Error");
				PP("Error");
				goto failure;
				break;
			case 0:
				PP("Timeout, so far received[%lu]",PacketsReceived);
				goto normalOut;
				break;
			default:
				if (ufd.revents & POLLIN) 
				{
					//recvfrom(p->rx_sock,tx_buff,PGEN_ETH_FRAME,0,NULL,NULL);
					recv(p->rx_sock, tx_buff,p->packetSize, 0); // receive normal data
					gettimeofday(&timediff0,NULL);
					if(memcmp(&tx_buff[12],p->proto,2)==0)
					{
						if(p->WriteRxData==1)
						{
							byte2time(&tx_buff[14],&timediff1);
							memmove(&packStats[PacketsReceived].ID,&tx_buff[30],sizeof(unsigned long));
							
							// Latency
							packStats[PacketsReceived].LATENCY = 
								(((timediff0.tv_sec-timediff1.tv_sec)*1000000)+(timediff0.tv_usec-timediff1.tv_usec));
							
							// Interarrival Delay
							packStats[PacketsReceived].INTERARRIVAL_TIME = 
								((timediff0.tv_sec*1000000)+timediff0.tv_usec);
							
							timediff1.tv_sec	= 0;
							timediff1.tv_usec	= 0;
						}
						
						PacketsReceived++;
						
						if(PacketsReceived == p->packetsNum){
							goto normalOut;
						}
						
						//PP("Packet Received[%lu]",PacketsReceived);
					}
				}
			break;
		}
#endif	
		
	}
	
normalOut:	
	
	if(p->WriteRxData){
		packet_gen_write_rx_result(p,PacketsReceived,packStats,PGEN_ETH_FRAME);
	}else{
		P_INFO("\nResults From Channel Tester.(Receive)");
		P_INFO("Write to file          :%s.",p->WriteRxData==1?"Yes":"No");
		P_INFO("Frame Size             :%u.",p->packetSize);
		P_INFO("Packets Received       :%lu.",PacketsReceived);
		P_INFO("Packets Sent           :%lu.",p->packetsNum);
		P_INFO("Tx interval            :%lu.",p->tx_interval);
		P_INFO("Protocol               :0x%2.2X%2.2X.",p->proto[0],p->proto[1]);
		P_INFO("Tx dev                 :%s.",p->tx_dev);
		P_INFO("Tx dev MAC             :"MAC_ADDR_S".",MAC_ADDR_V(p->p_srcmac));
		P_INFO("Rx dev                 :%s.",p->rx_dev);
		P_INFO("Rx dev MAC             :"MAC_ADDR_S".",MAC_ADDR_V(p->p_dstmac));
		P_INFO("Target's Rx dev MAC    :"MAC_ADDR_S".",MAC_ADDR_V(p->dstmac));
		P_INFO("Target's Tx dev MAC    :"MAC_ADDR_S".",MAC_ADDR_V(p->srcmac));
	}

failure:
	if(packStats){
		free(packStats);
	}
	
	
	pthread_exit(NULL);
	return NULL;
}



static inline void byte2time(uint8_t* input, struct timeval *output)
{
	/*Reversing the time2byte in order to calculate travel time.*/

	output->tv_sec	=	*(input);
	output->tv_sec	=	(output->tv_sec<<4)	|*(input+1);
	output->tv_sec	=	(output->tv_sec<<4)	|*(input+2);
	output->tv_sec	=	(output->tv_sec<<4)	|*(input+3);
	output->tv_sec	=	(output->tv_sec<<4)	|*(input+4);
	output->tv_sec	=	(output->tv_sec<<4)	|*(input+5);
	output->tv_sec	=	(output->tv_sec<<4)	|*(input+6);
	output->tv_sec	=	(output->tv_sec<<4)	|*(input+7);
	output->tv_usec	=	(output->tv_usec<<4)|*(input+8);
	output->tv_usec	=	(output->tv_usec<<4)|*(input+9);
	output->tv_usec	=	(output->tv_usec<<4)|*(input+10);
	output->tv_usec	=	(output->tv_usec<<4)|*(input+11);
	output->tv_usec	=	(output->tv_usec<<4)|*(input+12);
	output->tv_usec	=	(output->tv_usec<<4)|*(input+13);
	output->tv_usec	=	(output->tv_usec<<4)|*(input+14);
	output->tv_usec	=	(output->tv_usec<<4)|*(input+15);
	
	return;
}
