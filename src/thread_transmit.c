/*
 * thread_receive.c
 *
 *  Created on	: Jul 4, 2013
 *  Finished on	: Jul 10 2013
 *  Author		: Sotiris Lyras
 *  Version		: v04
 */

#include "libraries.h"
#include "variables.h"
#include "functions.h"




void* PackGen_Tx_Thread(void* args)
{
	packgen_t* p = (packgen_t*)args;
	uint8_t tx_buff[PGEN_ETH_FRAME];
	uint16_t indx	= 0;
	struct timeval timeStamp;
	
	unsigned long 	PacketsSent	= 0;

	memset(&tx_buff[indx],0xDE,PGEN_ETH_FRAME);
	
	//Generators Tx interface MAC address
	memcpy(&tx_buff[indx],p->p_srcmac,PGEN_ETH_MAC_LEN);
	indx+=PGEN_ETH_MAC_LEN;
	//Target's Rx interface MAC addres
	memcpy(&tx_buff[indx],p->dstmac,PGEN_ETH_MAC_LEN);
	indx+=PGEN_ETH_MAC_LEN;
	memcpy(&tx_buff[indx],p->proto,PGEN_ETH_PROTO_LEN);
	indx+=PGEN_ETH_PROTO_LEN;
	
	PP("HeaderSize [%d]",indx);
	PP("Proto [%2.2x%2.2x]",p->proto[0],p->proto[1]);
	PP("MAC:"MAC_ADDR_S,MAC_ADDR_V(p->p_srcmac));
	PP("MAC:"MAC_ADDR_S,MAC_ADDR_V(p->dstmac));
	
	
	uint8_t* timeStampNeedle = &tx_buff[indx];
	
	indx+=16;
	
	uint8_t* labelNeedle 	= &tx_buff[indx];
	
	indx+=sizeof(unsigned long);
	
	do
	{
		gettimeofday(&timeStamp,NULL);
		time2byte(&timeStamp,timeStampNeedle);
		label2packet(&PacketsSent,labelNeedle);
		send(p->tx_sock,tx_buff,p->packetSize,0);
		packet_gen_udelay(p->tx_interval);
		
		PacketsSent++;
	}while(PacketsSent<=p->packetsNum);

	pthread_exit(NULL);
	return NULL;
}






void time2byte(struct timeval *input, uint8_t* output){
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




void label2packet(unsigned long* label,uint8_t* buffer){
	
	memmove(buffer,label,sizeof(unsigned long));
	return;
}




























#if 0

void *transmitter(void *arg){
#define BUF_SIZE	(1518)
#define ETH_HEADER_LEN (14)
	int type,state;
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &type);
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&state);
	printf("\n****In Transmitter thread.****\n\n");
	ThreadArg *inTransmit=malloc(sizeof(ThreadArg));
	tra_handler tx_handler;
	mynet	*transmitter_net=(mynet*)malloc(sizeof(mynet));
	void	*buffer;
	uint8_t	*timer;
	uint8_t	*label;
	struct timeval t0,t1;
	struct timeval *timekeep;
	long elapsed;
	unsigned char *etherhead;
	memmove(inTransmit,arg,sizeof(ThreadArg));
	timekeep=malloc(sizeof(struct timeval));
	buffer=(uint8_t*)malloc(sizeof(uint8_t)*BUF_SIZE);
	tx_handler.buffer=buffer;
	tx_handler.inthread=inTransmit;
	tx_handler.transmitter=transmitter_net;
	pthread_cleanup_push(tx_cleanup_handler,(void*)&tx_handler);
	tx_handler.timer=timekeep;
	etherhead=(uint8_t*)buffer;
	transmitter_net->eh=(struct ethhdr*)etherhead;
	/*First init Network Data!*/
	netinit_transmitter(transmitter_net,inTransmit->dst_mac);
	/*Initialize packet before sending.*/
	datainit(buffer,inTransmit->size,transmitter_net->mac,inTransmit->dst_mac);
	timer=buffer+14;
	label=buffer+32;
	sent=0;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&state);
	gettimeofday(&t0, 0);
	while(sent!=NUM_OF_PACKETS){
		gettimeofday(timekeep,NULL);
		time2byte(timekeep,timer);
		label2packet(sent,label);
		sendto(transmitter_net->sock,buffer,inTransmit->size+ETH_HEADER_LEN,0,(struct sockaddr*)&transmitter_net->socket_address,sizeof(transmitter_net->socket_address));
		udelay(inTransmit->delay);
		sent++;
		pthread_testcancel();
	}
	gettimeofday(&t1, 0);
	elapsed = ((t1.tv_sec-t0.tv_sec)*1000000 + (t1.tv_usec-t0.tv_usec));
	printf("\nTransmitter Thread Exiting transmitted: %8i \n",sent);
	printf("***Transmitter Thread Exiting***\n");
	tx_result(inTransmit->path,sent,transmitter_net->sock,inTransmit->loop,inTransmit->delay,inTransmit->size,elapsed);
	/*Delay time in seconds in order to clear network buffers*/
	sleep(inTransmit->thr_interval);
	control=0;
	pthread_cleanup_pop(1);
	//Non zero argument means that the handler will be called even though
	//cancellation did not occur.
	pthread_exit(NULL);
	return NULL;
}

void tx_cleanup_handler(void *arg){
	printf("\n____Inside TX CleaUp Handler____\n");
	tra_handler *in_tx_handler=(tra_handler*)arg;
	close(in_tx_handler->transmitter->sock);
	free(in_tx_handler->buffer);
	free(in_tx_handler->transmitter);
	free(in_tx_handler->inthread);
	free(in_tx_handler->timer);

}

#endif
