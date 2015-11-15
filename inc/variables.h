/*
 * variables.h
 *
 *  Created on	: Jul 4, 2013
 *  Finished on	: Jul 10 2013
 *  Author		: Sotiris Lyras
 *  Version		: v04
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_

/*Basic Defines for Network Interface*/
#define ETH_HEADER_LEN	ETH_HLEN	/*Total octets in header.*/
#define BUF_SIZE		1518
#define ETH_FRAME_		TOTALLEN	1518
#define ETH_MAC_LEN		ETH_ALEN
#define ETH_P_NULL		0x0808 /*Packet Protocol, self defined/don't care.*/

/*Depending you distro you must put here the proper interface name.*/
//#define DEVICE			"p5p1" /*Interface_1*/
//#define DEVICE2			"p5p1" /*Interface_2*/


#define MAX_FILE_NAME 			50		/*The maximum size for filename*/
#define MAX_PACKET_INT_SEC 		1		/*maximum time packet interval*/
#define MAX_PACKET_INT_USEC 	0


/*Type Definitions*/
typedef struct {
	long int		delay;
	int				size;
	int				loop;
	int				thr_interval;
	char			**path;
	unsigned char	*dst_mac;
}ThreadArg;

typedef struct {
	int		loop;
	int		size;
	char	**path;
}RcvArg;

typedef struct {
	int 				sock;
	struct	sockaddr_ll	socket_address;
	struct	ifreq		ifr;
	struct	ethhdr		*eh;
	unsigned char		mac[6];
	unsigned char		my_mac[6];
}mynet;

typedef struct{
	ThreadArg		*inthread;
	void			*buffer;
	mynet			*transmitter;
	struct timeval	*timer;
	FILE 			*sent_stats;
}tra_handler;

typedef struct{
	RcvArg			*inrcv;
	void			*inbuffer;
	mynet			*receiver;
	struct timeval	*timer0;
	struct timeval	*timer1;
}rec_handler;

typedef struct{
	unsigned int	ID;
	long			LATENCY;
	long			INTERARRIVAL_TIME;
}time_char;


/*Global Variables*/
extern unsigned char	*DEVICE;
extern unsigned char	*DEVICE2;
extern unsigned char	src_mac_rec[6];
extern ThreadArg		*Transmitter;
extern RcvArg			*Receiver;
extern unsigned int	NUM_OF_PACKETS;
extern unsigned int	STARTING_DELAY;
extern unsigned int	STREAM_INTERVAL;
extern uint8_t			control;

/*Packet Counters*/
extern unsigned int	sent;
extern unsigned int	rec;

/*Input Data*/
extern int	NUM_OF_STREAMS;
extern int	PACKET_SIZE;

/*Threads*/
extern pthread_t		Transmitter_thr;
extern pthread_t		Receiver_thr;
extern pthread_attr_t	attr;

extern void *statusTR;
extern void *statusRC;

/*Statistics*/
extern time_char *Array;

/*Files*/
extern FILE	*send_stats;
extern FILE	*recv_stats;
extern FILE	*time_analysis;
/*Strings for files labeling*/
extern char	*path;
extern char	*free_path;
extern char	*fname_send_stats;
extern char	*fname_recv_stats;
extern char	*fname_time_analysis;

#endif /* VARIABLES_H_ */
