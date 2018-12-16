/*
 * variables.h
 *
 *  Created on	: Jul 4, 2013
 *  Finished on	: Feb 2017
 *  Author		: Sotiris Lyras
 *  Version		: v05
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_

#if 1
#define PP(m,...) 		do{fprintf(stdout,"[%s][%d]"m"\r\n",__func__,__LINE__,##__VA_ARGS__);}while(0);
#define P_INFO(m,...) 	do{fprintf(stdout,"[INFO]"m"\r\n",##__VA_ARGS__);}while(0);
#define P_ERROR(m,...) 	do{fprintf(stderr,"[ERROR]"m"\r\n",##__VA_ARGS__);}while(0);
#else
#define PP(m,...) ;
#define P_INFO(m,...) 	do{fprintf(stdout,"[INFO]"m"\r\n",##__VA_ARGS__);}while(0);
#define P_ERROR(m,...) ;
#endif


#define P_SUCCESS1	( 1)
#define P_SUCCESS	( 0)
#define P_FAILURE	(-1)

#ifndef likely
#define likely(x) 	__builtin_expect((x),1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect((x),0)
#endif

/* Defines for configurations variables */
#define PGEN_MANDATORY_VAR_NUM	(2)
#define PGEN_MAX_VAR_NUM	 	(8)
#define PGEN_MAX_PATHNAME_LEN	(64)

/*Basic Defines for Network Interface*/
#define PGEN_ETH_HEADER_LEN		ETH_HLEN	/*Total octets in header.*/
#define PGEN_ETH_FRAME			(ETH_FRAME_LEN - (2* PGEN_ETH_MAC_LEN) - PGEN_ETH_PROTO_LEN - 20)
#define PGEN_ETH_PROTO_LEN 		(PGEN_ETH_HEADER_LEN - (2 * PGEN_ETH_MAC_LEN))
#define PGEN_ETH_MAC_LEN		(ETH_ALEN)
//#define ETH_P_NULL				0x0808 /*Packet Protocol, self defined/don't care.*/

#define MAC_ADDR_S		"[%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x]"
#define MAC_ADDR_V(s)	s[0],s[1],s[2],s[3],s[4],s[5]



/*==============================================
.________.						   .________.
|        |p_srcmac			dstmac |		|
|  pgen  |------------------------>| target	|
|        |<------------------------|		|
|________|p_dstmac			srcmac |________|
===============================================*/

typedef struct
{
	char				tx_dev	[IFNAMSIZ];
	char				rx_dev	[IFNAMSIZ];
	char				path  	[PGEN_MAX_PATHNAME_LEN];
	char				f_name  [PGEN_MAX_PATHNAME_LEN];
	unsigned long		tx_interval;
	unsigned long   	packetsNum;
	uint16_t			packetSize;
	/* Header Parameters */
	uint8_t				p_srcmac[PGEN_ETH_MAC_LEN];	/* p_gen  tx mac */
	uint8_t				p_dstmac[PGEN_ETH_MAC_LEN]; /* p_gen  rx mac */
	uint8_t				srcmac	[PGEN_ETH_MAC_LEN]; /* target tx mac */
	uint8_t				dstmac	[PGEN_ETH_MAC_LEN]; /* target rx mac */
	uint8_t				proto 	[PGEN_ETH_PROTO_LEN];
	unsigned int 		vlan ;						/* VID in vlans is 12 bits aka 3 bytes */
	/* Socket descriptors 	*/
	int					tx_sock;
	int					rx_sock;
	int					WriteRxData;
}packgen_t;


typedef  void (*pgen_variable_func_t)(char* variable,packgen_t* p);

typedef struct 
{
	char						*var_key;
	pgen_variable_func_t		cb;
	char						*comment;
}pgen_variable_t;

typedef enum{
	IF_IN 	= 0,
	IF_OUT	= 1,
}packgen_direction_t;


typedef struct{
	unsigned long	ID;
	unsigned long	LATENCY;
	unsigned long	INTERARRIVAL_TIME;
}packge_time_stats_t;


















































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
extern unsigned long long	NUM_OF_PACKETS;
extern unsigned int	STARTING_DELAY;
extern unsigned int	STREAM_INTERVAL;
extern volatile uint8_t			control;

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
extern char	path[PGEN_MAX_PATHNAME_LEN];
extern char	*free_path;
extern char	*fname_send_stats;
extern char	*fname_recv_stats;
extern char	*fname_time_analysis;

#endif /* VARIABLES_H_ */
