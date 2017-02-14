/*
 * main.c
 *
 *  Created on	: Jul 4, 2013
 *  Finished on	: Feb 2017
 *  Author		: Sotiris Lyras
 *  Version		: v05
 */

/* --------------------------------------------------
 * Files.
 * --------------------------------------------------
 * Source Files: threads.c, realtester_time_v02.c functions.c
 * Header Files: libraries.h, variables.h, functions.h
 * --------------------------------------------------
 * Code Description.
 * --------------------------------------------------
 * This Packet generator produces RAW packets with destination defined in
 * dst_mac[] char vector. It keeps statistics for packets latency and
 * inter arrival time between two packets in order to collect data for
 * Jitter and Packet Delay Variation (PDV).
 * --------------------------------------------------
 * --------------------------------------------------
 * Imports (Mandatory).
 * --------------------------------------------------
 * In order to execute you must provide the following:
 * --------------------------------------------------
 * 1)	Path name to store files in.
 * 2)	Number of streams user would like to produce.
 * 3)	The size of packets user would like to send.
 * 4)	The delay (in uSec) packets should have between them.(fix the rate (PPS)).
 * 5)	The delay (in uSec) interval which is going to be added in each stream,
 * 		if the user wants different delay for each stream.
 * 6)	The time interval (in Seconds) between two streams.
 * 7)	Interface for receiving traffic.
 * 8)	Interface for sending traffic.
 * 9)	MAC address for the target machine. Format: "xx:xx:xx:xx:xx:xx".
 * --------------------------------------------------
 * Exports.
 * 1)	stream characteristics *_send_stats.txt
 * 2)	stream characteristic  *_recv_stats.txt
 * 3)	stream characteristic  *_time_analysis.txt
 * --------------------------------------------------
 * Example:
 * sudo gdb --args ./PacketGenerator ./export 1 1400 1000000 100 100 100 eth0 eth1 00:24:1d:12:40:d1
 */
/*Libraries*/
#include "libraries.h"
#include "variables.h"
#include "functions.h"
/*Main functions*/
int main(int argc, char *argv[])
{
	
	pid_t process = getpid();
	
	P_INFO("\n***Starting The Generator [%d] ***\n",process);

    // Check user id, if not sudo exit.
    //if(getuid()!=0){
    //    P_INFO("Root access required!");
    //    exit(0);
    //}
	
	
	
	packgen_t* packgen = new_packet_gen();
	if(!packgen){
		P_INFO("Init failed!");
		goto failure;
	}

	//Print systems specification.
    if(packet_gen_specification_read(argc,argv,packgen)<0){
		P_ERROR("Parameters");
		goto failure;
	}
	
	
	if(packet_gen_start(packgen)<0){
		P_ERROR("Packet Generator Failed.....");
		goto failure;
	}

failure:

	packet_gen_destroy(packgen);
	
	exit(EXIT_SUCCESS);
}
