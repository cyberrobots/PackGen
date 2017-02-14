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
 * sudo ./PacketGenerator devout enx00116b67376c devin enp0s25 dstmac 00:11:6b:67:37:6c proto 0x0808 num 1000000 inter 100 write 1
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
