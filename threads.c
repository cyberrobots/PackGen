/*
 * threads.c
 *
 *  Created on: Jul 5, 2013
 *      Author: cuda
 */
#include "libraries.h"
#include "variables.h"
#include "functions.h"

#define DEBUG

void *transmitter(void *arg){
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
	netinit_transmitter(transmitter_net);
	datainit(buffer,inTransmit->size);
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

void *receiver(void *arg_rec){
	printf("****In Receiver thread.****\n");
	int type,state;
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &type);
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&state);
	RcvArg *inrcv	=(RcvArg*)malloc(sizeof(RcvArg));
	mynet *receiver	=(mynet*)malloc(sizeof(mynet));
	void *inbuffer	=(uint8_t*)malloc(BUF_SIZE*sizeof(uint8_t));
	struct timeval *timediff0;
	struct timeval *timediff1;
	uint8_t *timer; //Points the packets time stamp.
	uint8_t *label; //Points the packets label id stamp.
	rec_handler rx_handler;
	/*Time structures*/
	timediff0=malloc(sizeof(struct timeval)); // The present time.
	timediff0->tv_sec=0;
	timediff0->tv_usec=0;
	timediff1=malloc(sizeof(struct timeval)); // Packets departure time.
	timediff1->tv_sec=0;
	timediff1->tv_usec=0;
	unsigned char *recetherhead;
	recetherhead=(uint8_t*)inbuffer;
	timer=recetherhead+14;
	label=recetherhead+32;
	receiver->eh=(struct ethhdr*)recetherhead;
	memmove(inrcv,arg_rec,sizeof(RcvArg));
	rx_handler.inbuffer=inbuffer;
	rx_handler.inrcv=inrcv;
	rx_handler.receiver=receiver;
	rx_handler.timer0=timediff0;
	rx_handler.timer1=timediff1;
	pthread_cleanup_push(rx_cleanup_handler,(void*)&rx_handler);
	netinit_receiver(receiver);
	rec=0;
	while(control){
		recvfrom(receiver->sock,inbuffer,BUF_SIZE,0,NULL,NULL);
		gettimeofday(timediff0,NULL);
		if (receiver->eh->h_proto == ETH_P_NULL && memcmp( (const void*)receiver->eh->h_dest,(const void*)src_mac_rec,ETH_MAC_LEN) == 0)
		{
			//if(rec==NUM_OF_PACKETS){rec=0;} //For overflow check.
			byte2time(timer,timediff1);
			memmove(&Array[rec].ID,label,sizeof(unsigned int));
			//printf("%i____%i \n",timediff0,timediff1);
			Array[rec].LATENCY=(((timediff0->tv_sec-timediff1->tv_sec)*1000000)+(timediff0->tv_usec-timediff1->tv_usec));
			Array[rec].INTERARRIVAL_TIME=((timediff0->tv_sec*1000000)+timediff0->tv_usec);
			rec++;
			timediff1->tv_sec=0;
			timediff1->tv_usec=0;
		}
		else{
			;
		}
	}
	printf("\nReceiver Thread Exiting Received:       %8i \n",rec);
	printf("***Receiver Thread Exiting***\n");
	rx_result(inrcv->path,rec,receiver->sock,inrcv->loop,inrcv->size);
	pthread_cleanup_pop(1);
	pthread_exit(NULL);
	return NULL;
}

void rx_cleanup_handler(void *arg){
	rec_handler *in_rx_handler=(rec_handler*)arg;
	close(in_rx_handler->receiver->sock);
	free(in_rx_handler->inrcv);
	free(in_rx_handler->inbuffer);
	free(in_rx_handler->receiver);
	free(in_rx_handler->timer0);
	free(in_rx_handler->timer1);
	printf("\n____Inside RX CleaUp Handler____\n\n");
}
