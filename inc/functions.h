/*
 * functions.h
 *
 *  Created on	: Jul 4, 2013
 *  Finished on	: Feb 2017
 *  Author		: Sotiris Lyras
 *  Version		: v05
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
// Init
packgen_t* new_packet_gen(void);
// De-Init
void 	packet_gen_destroy(packgen_t *p);
// Specs Read
int 	packet_gen_specification_read(int argc, char *argv[],packgen_t* p);
// Init Sockets
int 	pack_gen_init_sock(packgen_t* p,uint8_t direction);
// Packet Generator start
int	 	packet_gen_start(packgen_t* p);
// Signal Handler
void 	packet_gen_signal(int signum);
// Threads
void* 	PackGen_Rx_Thread(void* args);
void* 	PackGen_Tx_Thread(void* args);
// Timing
void 	packet_gen_udelay(unsigned long microns);
// Output
void 	packet_gen_write_rx_result(packgen_t*p,unsigned long rx_num,packge_time_stats_t* stats,uint16_t size);

#endif /* FUNCTIONS_H_ */
