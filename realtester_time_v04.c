/*
 * realtester_time_v02.c
 *
 *  Created on	: Jul 4, 2013
 *  Finished on	: Jul 10 2013
 *  Author		: Sotiris Lyras
 *  Version		: v02
 */

/* Files.
 * Source Files: threads.c, realtester_time_v02.c functions.c
 * Header Files: libraries.h, variables.h, functions.h
 *
 * Code Description.
 * This Packet generator produces RAW packets with destination defined in
 * dst_mac[] char vector. It keeps statistics for packets latency and
 * inter arrival time between two packets in order to collect data for
 * Jitter and Packet Delay Variation (PDV).
 *
 * Imports.
 * In order to execute you must provide the following:
 * 1)	Path name to store files in.
 * 2)	Number of streams user would like to produce.
 * 3)	The size of packets user would like to send.
 * 4)	The delay (in uSec) packets should have between them.(fix the rate (PPS)).
 * 5)	The delay (in uSec) interval which is going to be added in each stream,
 * 		if the user wants different delay for each stream.
 * 6)	The time interval (in Seconds) between two streams.
 *
 * Exports.
 * 1)	stream characteristics *_send_stats.txt
 * 2)	stream characteristic  *_recv_stats.txt
 * 3)	stream characteristic  *_time_analysis.txt
 *
 */

/*Only Predefined MAC ADDRESSES*/
#define ZERMATT 	//Define the target machine.

#include "libraries.h"
#include "variables.h"
#include "functions.h"



int main(int argc, char *argv[]){
	printf("\n***Starting The Generator***\n");
	printf("---System Initialize---\n");
	/*Pthread init*/
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
	/*------Static Variables--------*/
	/*Zermatt PC*/
#ifdef ZERMATT
	dst_mac[0]=0x00;dst_mac[1]=0x1f;
	dst_mac[2]=0x29;dst_mac[3]=0xdc;
	dst_mac[4]=0x28;dst_mac[5]=0x39;
#endif
	/*Panahaiko PC*/
#ifdef PANAHAIKO
	dst_mac[0]=0xd4;dst_mac[1]=0x85;
	dst_mac[2]=0x64;dst_mac[3]=0x06;
	dst_mac[4]=0x06;dst_mac[5]=0x57;
#endif
	/*Laptop*/
#ifdef LAPTOP
	dst_mac[0]=0x8C;dst_mac[1]=0x73;
	dst_mac[2]=0x6E;dst_mac[3]=0xA8;
	dst_mac[4]=0xD7;dst_mac[5]=0xC9;
#endif
	/*SOTIRIANA*/
#ifdef SOTIRIANA
	dst_mac[0]=0x00;dst_mac[1]=0x24;
	dst_mac[2]=0x1D;dst_mac[3]=0x12;
	dst_mac[4]=0x40;dst_mac[5]=0xDC;
#endif
	/*Thanos PC*/
#ifdef THANOSPC
	dst_mac[0]=0x94;dst_mac[1]=0xDE;
	dst_mac[2]=0x80;dst_mac[3]=0x27;
	dst_mac[4]=0x4B;dst_mac[5]=0xE5;
#endif
	/*Main*/
	memory_allocate(); 				//Allocate memory for the system.
	specification_print(argc,argv);	//Print systems specification.
	signal(SIGINT,sigint); 			//Set the signal.
	int i;
	for(i=0;i<NUM_OF_STREAMS;i++){
		Transmitter->loop=i;
		Receiver->loop=i;
		control=1;
		/*Fire up threads*/
		pthread_create(&Receiver_thr,&attr,receiver,Receiver);
		pthread_create(&Transmitter_thr,&attr,transmitter,Transmitter);

		/*Wait to finish the send.*/
		pthread_join(Transmitter_thr,statusTR);
		pthread_join(Receiver_thr,statusRC);
		Transmitter->delay=Transmitter->delay+STREAM_INTERVAL;

	}
	/*Memory cleanup function*/
	finish();
	return 0;
}
