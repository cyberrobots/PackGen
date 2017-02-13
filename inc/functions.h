/*
 * functions.h
 *
 *  Created on	: Jul 4, 2013
 *  Finished on	: Jul 10 2013
 *  Author		: Sotiris Lyras
 *  Version		: v04
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

packgen_t* new_packet_gen(void);

void destroy_packet_gen(packgen_t *p);

void packet_gen_signal(int signum);

int packet_gen_specification_read(int argc, char *argv[],packgen_t* p);


int pack_gen_init_sock(packgen_t* p,uint8_t direction);


int packet_gen_start(packgen_t* p);




void* PackGen_Rx_Thread(void* args);
void* PackGen_Tx_Thread(void* args);






void time2byte(struct timeval *input, uint8_t* output);
void byte2time(uint8_t* input, struct timeval *output);

void label2packet(unsigned long* label,uint8_t* buffer);

void packet_gen_udelay(long long microns);

void rx_result(packgen_t*p,unsigned long rx_num,packge_time_stats_t* stats,uint16_t size);

#if 0
int memory_allocate();
int specification_read(int argc, char *argv[]);

/*Global*/
void udelay(long microns);
void sigint(int signum);
void finish();
int mac_import(char *input, unsigned char *out);

/*Threads Functions*/
void *transmitter(void *arg);
void tx_cleanup_handler(void *arg);

void *receiver(void *arg_rec);
void rx_cleanup_handler(void *arg);

/*In Transmitter Thread Functions*/
void netinit_transmitter(mynet *transmitter,unsigned char* dst_mac);
void netinit_receiver(mynet *receiver);
void time2byte(struct timeval *input, uint8_t* output);
void byte2time(uint8_t* input, struct timeval *output);
void label2packet(int label,uint8_t* buffer);
void datainit(void *buffer,int size,unsigned char *src_mac,unsigned char *dst_mac);
void time2byte(struct timeval *input, uint8_t* output);
void byte2time(uint8_t* input, struct timeval *output);
void static_memcpy(uint8_t *buffer, unsigned char *src_mac,unsigned char *dst_mac);
void label2packet(int label,uint8_t* buffer);

/*Statistics Print*/
void tx_result(char **tx_path,int sent,int tx_sock,int loop, int delay, int size,long elapsed);
void rx_result(char **rx_path,int rec,int rx_sock,int loop, int size);
#endif


#endif /* FUNCTIONS_H_ */
