/*
 * functions.c
 *
 *  Created on	: Jul 4, 2013
 *  Finished on	: Jul 10 2013
 *  Author		: Sotiris Lyras
 *  Version		: v04
 */

#include "libraries.h"
#include "variables.h"
#include "functions.h"



packgen_t* new_packet_gen(void)
{
	packgen_t* gen = malloc(sizeof(packgen_t));
	
	if(!gen){
		return NULL;
	}
	
	memset(gen,0,sizeof(*gen));
	
	/* Assign Initial Values */
	strcpy(gen->path,"./");
	memset(gen->srcmac,0x00		,PGEN_ETH_MAC_LEN);
	memset(gen->dstmac,0x00		,PGEN_ETH_MAC_LEN);
	
	memset(gen->p_srcmac,0x00	,PGEN_ETH_MAC_LEN);
	memset(gen->p_dstmac,0x00	,PGEN_ETH_MAC_LEN);
	
	memset(gen->proto,0x08,PGEN_ETH_PROTO_LEN);

	gen->tx_interval 	= 0;
	gen->packetsNum  	= 0xFFFFFFFFFFFFFFFF;
	gen->packetSize		= PGEN_ETH_FRAME;
	gen->send_stats	 	= NULL;
	gen->recv_stats		= NULL;
	gen->time_analysis	= NULL;
	gen->tx_sock		= -1;
	gen->rx_sock		= -1;
	
	return gen;
}

void destroy_packet_gen(packgen_t *p)
{
	if(!p){
		return;
	}
	
	if(p->send_stats)
		fclose(p->send_stats);
	
	if(p->recv_stats)
		fclose(p->recv_stats);
	
	if(p->time_analysis)
		fclose(p->send_stats);
	
	if(p->tx_sock > -1){
		//PP("CleanSock[%d]",p->tx_sock );
		close(p->tx_sock);
	}
	
	if(p->rx_sock > -1){
		//PP("CleanSock[%d]",p->rx_sock );
		close(p->rx_sock);
	}
		
	free(p);	
		
	return;
}


static packgen_t* _packet = NULL;

void packet_gen_signal(int signum)
{
	/*Application Kill CleanUp Function.*/
	P_INFO("Caught signal %d\n",signum);

	destroy_packet_gen(_packet);
	
	exit(0);
}


#define IMPORT(m)	void m(char* var,packgen_t* p);

IMPORT(show_help)
IMPORT(path_import)
IMPORT(numOfPackets_import)
IMPORT(txInterval_import)
IMPORT(deviceOut_import)
IMPORT(deviceIn_import)
IMPORT(deviceDstMac_import)
IMPORT(deviceSrcMac_import)
IMPORT(protocol_import)

static pgen_variable_t pgen_var_table[] =
{
	{"help",		show_help			,""								},
	{"path",		path_import			,"Relative"						},
	{"num",			numOfPackets_import	,"Packets Number"				},
	{"inter",		txInterval_import	,"Transmit interval (usec)"		},
	{"devout",		deviceOut_import	,"Transmit interface"			},
	{"devin",		deviceIn_import		,"Receive interface"			},
	{"dstmac",		deviceDstMac_import	,"Target's Rx MAC address"		},
	{"srcmac",		deviceSrcMac_import	,"Target's Tx MAC address"		},
	{"proto",		protocol_import		,"Protocol (default: 0x0808)"	},
};
#define NUM_OF_PARAMS	(sizeof(pgen_var_table) / sizeof(pgen_variable_t))

void pack_gen_usage( void )
{
	int i;
	
	P_INFO("Usage:");
	
	for(i=0;i<NUM_OF_PARAMS;i++)
	{
		P_INFO("%d)\t%s\t\t%s",i,pgen_var_table[i].var_key,pgen_var_table[i].comment);
	}

	return;
}

void show_help(char* var,packgen_t* p)
{
	pack_gen_usage();
	
	destroy_packet_gen(p);
	
	exit(0);
}	

void path_import(char* var,packgen_t* p)
{
	if(var && p){
		strncpy(p->path,var,strlen(var)<PGEN_MAX_PATHNAME_LEN?strlen(var):PGEN_MAX_PATHNAME_LEN);
	}
	
	PP("Path [%s]",p->path);
}

void numOfPackets_import(char* var,packgen_t* p)
{
	long long val = strtol(var,NULL,0);
	
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		P_ERROR("Problem while importing packets number");
		return;
	}
	
	p->packetsNum = val;
	
	PP("Num of packets [%llu]",p->packetsNum);
}

void txInterval_import(char* var,packgen_t* p)
{
	long long val = strtol(var,NULL,0);
	
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		P_ERROR("Problem while importing packets number");
		return;
	}
	
	p->tx_interval = val;
	
	PP("tx_interval [%llu]",p->tx_interval);
}

void deviceOut_import(char* var,packgen_t* p)
{
	
	if(var && p){
		strcpy(p->tx_dev,var);
	}
	
	PP("Device Out [%s]",p->tx_dev);
}

void deviceIn_import(char* var,packgen_t* p)
{
	if(var && p){
		strcpy(p->rx_dev,var);
	}
	
	PP("Device In [%s]",p->rx_dev);
}

void deviceDstMac_import(char* var,packgen_t* p)
{
	int values[PGEN_ETH_MAC_LEN];
    
    if(sscanf(var,"%x:%x:%x:%x:%x:%x" 
			  ,&values[0],&values[1],&values[2]
			  ,&values[3],&values[4],&values[5])!=PGEN_ETH_MAC_LEN)
	{
        P_ERROR("Failed reading MAC address!");
		return;
    }
    
	int i=0;
	/* convert to uint8_t */
    for( i = 0; i < PGEN_ETH_MAC_LEN; ++i )
        p->dstmac[i] = (uint8_t) values[i];
	
	
	PP("Destination MAC [%2.x:%2.x:%2.x:%2.x:%2.x:%2.x]"
													,p->dstmac[0]
													,p->dstmac[1]
													,p->dstmac[2]
													,p->dstmac[3]
													,p->dstmac[4]
													,p->dstmac[5]);
}

void deviceSrcMac_import(char* var,packgen_t* p)
{
	int values[PGEN_ETH_MAC_LEN];
    
    if(sscanf(var,"%x:%x:%x:%x:%x:%x" 
			  ,&values[0],&values[1],&values[2]
			  ,&values[3],&values[4],&values[5])!=PGEN_ETH_MAC_LEN)
	{
        P_ERROR("Failed reading MAC address!");
		return;
    }
    
	int i=0;
	/* convert to uint8_t */
    for( i = 0; i < PGEN_ETH_MAC_LEN; ++i )
        p->srcmac[i] = (uint8_t) values[i];
	
	
	PP("Source MAC [%2.x:%2.x:%2.x:%2.x:%2.x:%2.x]"
	   ,p->srcmac[0]
	   ,p->srcmac[1]
	   ,p->srcmac[2]
	   ,p->srcmac[3]
	   ,p->srcmac[4]
	   ,p->srcmac[5]);
}

void protocol_import(char* var,packgen_t* p)
{
	
	int values=0;
    
    if(sscanf(var,"0x%x",&values)!=1){
        P_ERROR("Failed reading");
		return;
    }

	memcpy(p->proto,&values,2);
	
	PP("Proto [0x%2.2x%2.2x]",p->proto[0],p->proto[1]);
}

int packet_gen_specification_read(int argc, char *argv[],packgen_t* args)
{
	P_INFO("---System Initialize---");
	/*Grab variables from console*/
	if( ( (argc) < (2 * PGEN_MANDATORY_VAR_NUM) ) || ((argc-1)%2) )
	{
		P_INFO("Variable Error....!");
		
		pack_gen_usage();
		
		goto failure;
	}else
	{
		int i=0,p=0;
		for(i = 1; i < argc ; i+=2)
		{
			for(p=0;p<NUM_OF_PARAMS;p++)
			{
				if(strcmp(pgen_var_table[p].var_key,argv[i])==0){
					pgen_var_table[p].cb(argv[i+1],args);
					break;
				}
			}
		}
		
	}

	
	return P_SUCCESS;

failure:		
	
	return P_FAILURE;
}

int pack_gen_init_sock(packgen_t* p,uint8_t direction)
{
	int s = -1;	int i=0;
	struct sockaddr_ll	s_addr;
	struct ifreq		ifr;
	int ifindex	= 0;
	char mac[ETH_ALEN];
	
	if(!p){
		P_ERROR("Params");
		return P_FAILURE;
	}
	
	
	if( (s = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL)) ) < 0){
		PP("Socket");
		return P_FAILURE;
	}
	
	memset(&ifr,0,sizeof(ifr));
	
	if(direction == IF_IN){
		/*Grab interface name.*/
		strncpy(ifr.ifr_name,(const char*)p->rx_dev,IFNAMSIZ);
	}else{
		strncpy(ifr.ifr_name,(const char*)p->tx_dev,IFNAMSIZ);
	}
	
 	if(ioctl(s,SIOCGIFFLAGS,&ifr) < 0){
		perror("SIOCGIFFLAGS");
		goto failure;
	}
    
    /* Check if the interface is active */
    if(!(ifr.ifr_flags & IFF_UP)){
        P_INFO("Interface is Down.");
		goto failure;
    }
	
	if(direction == IF_IN)
	{
		/* Set Interface in promiscuous mode. */
		ifr.ifr_flags |= IFF_PROMISC;
		if(ioctl(s,SIOCSIFFLAGS,&ifr) < 0){
			perror("SIOCSIFFLAGS");
			goto failure;
		}
	}

	if(ioctl(s, SIOCGIFINDEX,ifr)<0){	
		perror("SIOCGIFINDEX");
		goto failure;
	}
	/*Grab interface index from kernel.*/
	//ifindex = ifr.ifr_ifru.ifru_ivalue
	ifindex = ifr.ifr_ifindex;

	
	memset(&s_addr,0,sizeof(s_addr));	
	s_addr.sll_family 	= PF_PACKET;
	s_addr.sll_hatype 	= ARPHRD_ETHER;
	s_addr.sll_halen  	= ETH_ALEN;
	s_addr.sll_ifindex 	= ifindex;
	s_addr.sll_pkttype 	= PACKET_OTHERHOST;
	s_addr.sll_protocol	= htons(ETH_P_ALL);		//must check this variables
	s_addr.sll_addr[6]	= 0x00;
	s_addr.sll_addr[7]	= 0x00;
    /* Bind socket to the interface. */
    bind(s,(const struct sockaddr *)&s_addr,sizeof(struct sockaddr_ll));
	
	/*Grab ingerface's MAC address.*/
	if(ioctl(s,SIOCGIFHWADDR,&ifr)==-1){
		perror("SIOCGIFHWADDR");
		goto failure;
	}

	memset(mac,0,ETH_ALEN);
	/* Copy rx interface's MAC address */
	memcpy(mac,&(ifr.ifr_hwaddr).sa_data,ETH_ALEN);

    PP("/----------------------------------------------/");
	PP("Socket Desc [%d]",s);
	if(IF_IN == direction){
		p->rx_sock = s;
		PP("The Iface %s",(char*)p->rx_dev);
		for(i=0;i<ETH_ALEN;i++){
			p->p_srcmac[i] = (uint8_t)mac[i];
		}
		PP("The MAC "MAC_ADDR_S,MAC_ADDR_V(p->p_srcmac));
	}else{
		p->tx_sock = s;
		PP("The Iface %s",(char*)p->tx_dev);
		for(i=0;i<ETH_ALEN;i++){
			p->p_dstmac[i] = (uint8_t)mac[i];
		}
		PP("The MAC "MAC_ADDR_S,MAC_ADDR_V(p->p_dstmac));
	}
    PP("/----------------------------------------------/");

	///* Get settings */
	//set = fcntl(sockid,F_GETFL, 0);
	//if(set < 0){
	//	*error = -__LINE__;
	//	return -S_CREATE;
	//}

	///* Set Settings plus NON Blocking flag */
	//err = fcntl(sockid, F_SETFL, set | O_NONBLOCK);
	//if(err < 0){
	//	*error = -__LINE__;
	//	return -S_CREATE;
	//}
	return P_SUCCESS;
	
failure:
	
	if(s>-1){
		close(s);
	}
	
	return P_FAILURE;
	
}

int packet_gen_start(packgen_t* p)
{
	pthread_attr_t 	attr;
	pthread_t		packgen_rx_thread;
	pthread_t		packgen_tx_thread;
	
	uint32_t*		PackGenStatusRx	=	NULL;
	uint32_t*		PackGenStatusTx	=	NULL;
	
	
	if(!p){
		P_ERROR("Params");
		goto failure;
	}	
	
	_packet = p;
	
	
	if(pack_gen_init_sock(p,IF_IN)<0){
		PP("Socket initialize failed!");
		goto failure;
	}
	
	if(pack_gen_init_sock(p,IF_OUT)<0){
		PP("Socket initialize failed!");
		goto failure;
	}
	
	
	/* Set Kill signal. */
    signal(SIGINT,packet_gen_signal); 	
	

	///*Pthread init*/
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);	
	
	
	///*Fire up threads*/
	pthread_create(&packgen_rx_thread,&attr,PackGen_Rx_Thread,(void*)p);
	pthread_create(&packgen_tx_thread,&attr,PackGen_Tx_Thread,(void*)p);
	///*Wait to finish the send.*/
	pthread_join(packgen_rx_thread,(void**)&PackGenStatusRx);
	pthread_join(packgen_tx_thread,(void**)&PackGenStatusTx);
	
	pthread_attr_destroy(&attr);
	
	return P_SUCCESS;

failure:
	return P_FAILURE;
}



void packet_gen_udelay(long long microns)
{
	if(!microns){
		return;
	}
	
	/*Delay function with microseconds accuracy*/
	struct timeval delay;
	struct timeval target;
	struct timeval now;
	delay.tv_sec = 0;
	delay.tv_usec = microns;
	while (delay.tv_usec >= 1000000)
	{
		delay.tv_sec++;
		delay.tv_usec -= 1000000;
	}

	gettimeofday(&now, NULL);
	timeradd(&now, &delay, &target);

	do {
		gettimeofday(&now, NULL);
	} while (timercmp(&now, &target,<));

}


void rx_result(packgen_t*p,unsigned long rx_num,packge_time_stats_t* stats,uint16_t size)
{
	unsigned long i = 0;
	/*Receive statistics file open*/
	char fname_rx_stats  [2*PGEN_MAX_PATHNAME_LEN];
	char fname_time_stats[2*PGEN_MAX_PATHNAME_LEN];
	
	if(!stats || !p)
		return;
	
	
	memset(fname_rx_stats,0,2*PGEN_MAX_PATHNAME_LEN);
	memset(fname_time_stats,0,2*PGEN_MAX_PATHNAME_LEN);


	if(strcmp(p->path,"./")!=0){
		snprintf(fname_rx_stats,2*PGEN_MAX_PATHNAME_LEN,"./%s/PackGen_%d_recv_stats.txt",p->path,size);
	}else{
		snprintf(fname_rx_stats,2*PGEN_MAX_PATHNAME_LEN,"%sPackGen_%d_recv_stats.txt",p->path,size);
	}

	
	if(strcmp(p->path,"./")!=0){
		snprintf(fname_time_stats,2*PGEN_MAX_PATHNAME_LEN,"./%s/PackGen_%d_recv_stats.txt",p->path,size);
	}else{
		snprintf(fname_time_stats,2*PGEN_MAX_PATHNAME_LEN,"%sPackGen_%d_recv_stats.txt",p->path,size);
	}
	
	
	p->recv_stats = fopen(fname_rx_stats,"w");
	if(!p->recv_stats){
		perror("No File or Bad Directory");
		goto failure;
	}
	
	
	p->time_analysis = fopen(fname_time_stats,"w");
	if(!p->time_analysis){
		perror("No File or Bad Directory");
		goto failure;
	}
	
	
	
	/*File printing*/
	fprintf(p->recv_stats,"\nResults From Channel Tester.(Receive)\n");
	fprintf(p->recv_stats,"Data Size        :\t%u.\n",size);
	fprintf(p->recv_stats,"Packets Received	:\t%lu.\n",rx_num);
	
	
	
	fprintf(p->time_analysis,"%%Timining Analysis Report For MATLAB processing!\n\n");
	
	unsigned long temp=stats[0].INTERARRIVAL_TIME;
	
	fprintf(p->time_analysis,"%lu,%lu,%lu;\r\n",stats[i].ID,(unsigned long)0,(unsigned long)0);
	for(i = 1; i < rx_num; i++){
		//if(stats[i].ID!=0x00 && stats[i].INTERARRIVAL_TIME!=0 && stats[i].LATENCY!=0)
		{
			temp = stats[i-1].INTERARRIVAL_TIME;
			fprintf(p->time_analysis,"%lu,%lu,%lu;\r\n",stats[i].ID,stats[i].LATENCY,(stats[i].INTERARRIVAL_TIME-temp));
			packet_gen_udelay(100);
		}
	}
	fprintf(p->time_analysis,"];\n");
	
	
	/*Close files*/
	fclose(p->recv_stats);
	p->recv_stats = NULL;
	fclose(p->time_analysis);
	p->time_analysis = NULL;
	
	
	/*Console Printing*/
	printf("\nResults From Channel Tester.(Receive)\n");
	printf("Data Size              :\t%u.\n",size);
	printf("Packets Received       :\t%lu.\n",rx_num);
	
failure:
	return;
	
}






#if 0
int memory_allocate(){
	/*Path name to create*/
	//path						=(char*)		 malloc(MAX_FILE_NAME*sizeof(char));
	//free_path=path;
	/*File names*/
	fname_send_stats			=(char*)   	 	 malloc(PGEN_MAX_PATHNAME_LEN*sizeof(char));
	fname_recv_stats			=(char*)		 malloc(PGEN_MAX_PATHNAME_LEN*sizeof(char));
	fname_time_analysis			=(char*)		 malloc(PGEN_MAX_PATHNAME_LEN*sizeof(char));
	/*Thread structures*/
	Transmitter					=(ThreadArg*)	 malloc(sizeof(ThreadArg));
	Receiver					=(RcvArg*)		 malloc(sizeof(RcvArg));
	Transmitter->dst_mac		=(unsigned char*)malloc(sizeof(unsigned char)*ETH_MAC_LEN);
	
	if(
		fname_send_stats && 
		fname_recv_stats && 
		fname_time_analysis && 
		Transmitter	&&
		Receiver &&
		Transmitter->dst_mac){
		return 0;
	}
	
	
	return -1;
}


int mac_import(char *input,unsigned char *OUT)
{
	unsigned int MAC_in[6];
	int i = 0;

    memset(MAC_in,0,6);
    
    if(sscanf(input,"%x:%x:%x:%x:%x:%x",&MAC_in[0],&MAC_in[1],&MAC_in[2],&MAC_in[3],&MAC_in[4],&MAC_in[5])!=6){
        printf("Failed reading MAC address!");
        return 1;
    }
    

	printf("Target MAC");
	for(i=0;i<6;i++)
    {
        OUT[i] = (unsigned char)MAC_in[i];
		printf(":%2.2X",OUT[i]);
	}
	printf("\n");

	return 0; /*Success Code*/
}





void finish(){
	/*Normally Finish CleanUp Function.*/
	P_INFO("Terminate Packet Generator");
#define F_FREE(m) if(m!=NULL){free(m);}
	
	F_FREE(fname_send_stats);
	F_FREE(fname_recv_stats);
	F_FREE(fname_time_analysis);
	F_FREE(Transmitter->dst_mac);
	F_FREE(Transmitter);
	F_FREE(Receiver);
	F_FREE(Array);
	F_FREE(free_path);
	
	pthread_attr_destroy(&attr);
	return;
}

void netinit_transmitter(mynet *transmitter,unsigned char* dst_mac){
	/*Network Initialize Transmitter Code*/
	unsigned char src_mac[6];
	int j;
	int ifindex=0;
	/*Open RAW socket*/
	transmitter->sock=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
	/*Grab interface name.*/
	strncpy(transmitter->ifr.ifr_name,(const char*)DEVICE,IFNAMSIZ);
	/*Grab interface index from kernel.*/
	if (ioctl(transmitter->sock, SIOCGIFINDEX,&transmitter->ifr)==-1) {perror("SIOCGIFINDEX");exit(1);}
	ifindex=transmitter->ifr.ifr_ifindex;
	/*Grab interface's with above index MAC address.*/
	if(ioctl(transmitter->sock,SIOCGIFHWADDR,&transmitter->ifr)==-1){perror("SIOCGIFHWADDR");exit(1);}
	transmitter->socket_address.sll_family   = PF_PACKET;
	transmitter->socket_address.sll_protocol = htons(ETH_P_IP);
	transmitter->socket_address.sll_ifindex  = ifindex;
	transmitter->socket_address.sll_hatype   = ARPHRD_ETHER;
	transmitter->socket_address.sll_pkttype  = PACKET_OTHERHOST;
	transmitter->socket_address.sll_halen    = ETH_ALEN;
	transmitter->socket_address.sll_addr[0]  = dst_mac[0];
	transmitter->socket_address.sll_addr[1]  = dst_mac[1];
	transmitter->socket_address.sll_addr[2]  = dst_mac[2];
	transmitter->socket_address.sll_addr[3]  = dst_mac[3];
	transmitter->socket_address.sll_addr[4]  = dst_mac[4];
	transmitter->socket_address.sll_addr[5]  = dst_mac[5];
	transmitter->socket_address.sll_addr[6]  = 0x00;
	transmitter->socket_address.sll_addr[7]  = 0x00;
	for(j=0;j<6;j++){src_mac[j]=transmitter->ifr.ifr_hwaddr.sa_data[j];}
#ifdef Debug
	printf("Successfully got our MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",src_mac[0],src_mac[1],src_mac[2],src_mac[3],src_mac[4],src_mac[5]);
#endif
	memcpy(&transmitter->mac,&src_mac,ETH_MAC_LEN);
	transmitter->eh->h_proto=ETH_P_NULL;
	return;
}

void datainit(void *buffer,int size,unsigned char *src_mac,unsigned char *dst_mac)
{
	/*Initialize Data*/
	int i;
	unsigned char *data;
	data=(uint8_t*)buffer+14;
	static_memcpy(buffer,src_mac,dst_mac);
	for(i=0;i<size;i++){data[i]=(unsigned char)170;}
	return;
}

void static_memcpy(uint8_t *buffer, unsigned char *src_mac,unsigned char *dst_mac)
{
	/*||Destination_mac||Source_mac||Protocol||Data||CRC||*/
	memmove(buffer,dst_mac,ETH_MAC_LEN);
	memmove(buffer+ETH_MAC_LEN,(void*)src_mac,ETH_MAC_LEN);
	return;
}

void label2packet(int label,uint8_t* buffer){
	//label=htonl(label); /*Take care for little endian and big endian systems*/
	memmove(buffer,&label,sizeof(int));
	return;
}

void time2byte(struct timeval *input, uint8_t* output){
	/*Convert time stamp to byte ordering for placing on the packet.*/
	//printf("\nIN time2byte FUNCTION\n");
	//printf("%8.X__%8.X\n",input->tv_sec,input->tv_usec);
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
	//printf("%X %X %X %X %X %X %X %X %X %X %X %X %X\n",*(output+0),*(output+1),*(output+2),*(output+3),*(output+4),*(output+5),*(output+6),*(output+7),*(output+8),*(output+9),*(output+10),*(output+11),*(output+12));
	return;
}

void byte2time(uint8_t* input, struct timeval *output){
	/*Reversing the time2byte in order to calculate travel time.*/
	//printf("\nIN byte2time FUNCTION\n");
	//printf("%X %X %X %X %X %X %X %X %X %X %X %X %X\n",*(input+0),*(input+1),*(input+2),*(input+3),*(input+4),*(input+5),*(input+6),*(input+7),*(input+8),*(input+9),*(input+10),*(input+11),*(input+12));
	output->tv_sec=*(input);
	output->tv_sec=(output->tv_sec<<4)|*(input+1);
	output->tv_sec=(output->tv_sec<<4)|*(input+2);
	output->tv_sec=(output->tv_sec<<4)|*(input+3);
	output->tv_sec=(output->tv_sec<<4)|*(input+4);
	output->tv_sec=(output->tv_sec<<4)|*(input+5);
	output->tv_sec=(output->tv_sec<<4)|*(input+6);
	output->tv_sec=(output->tv_sec<<4)|*(input+7);
	output->tv_usec=(output->tv_usec<<4)|*(input+8);
	output->tv_usec=(output->tv_usec<<4)|*(input+9);
	output->tv_usec=(output->tv_usec<<4)|*(input+10);
	output->tv_usec=(output->tv_usec<<4)|*(input+11);
	output->tv_usec=(output->tv_usec<<4)|*(input+12);
	output->tv_usec=(output->tv_usec<<4)|*(input+13);
	output->tv_usec=(output->tv_usec<<4)|*(input+14);
	output->tv_usec=(output->tv_usec<<4)|*(input+15);
	//printf("%8.X__%8.X\n",output->tv_sec,output->tv_usec);
	return;
}



void rx_result(char **rx_path,int rec,int rx_sock,int loop, int size)
{
		/*Receive statistics file open*/
		int i;
		char loop_num[33];
		char packet_size[33];
		long temp;
		snprintf(loop_num, sizeof(loop_num), "%d", loop);
		snprintf(packet_size, sizeof(packet_size), "%d", size);
		strcpy(fname_recv_stats,"./");
		strcat(fname_recv_stats,*rx_path);
		strcat(fname_recv_stats,"/stream_");
		strcat(fname_recv_stats,loop_num);
		strcat(fname_recv_stats,"_size_");
		strcat(fname_recv_stats,packet_size);
		strcpy(fname_time_analysis,fname_recv_stats);
		strcat(fname_recv_stats,"_recv_stats.txt");
		strcat(fname_time_analysis,"_time_analysis.m");
		/*File open & descriptor*/
		recv_stats		=fopen(fname_recv_stats,"w");
		if(recv_stats==NULL){perror("No File or Bad Directory");exit(1);}
		time_analysis	=fopen(fname_time_analysis,"w");
		if(time_analysis==NULL){perror("No File or Bad Directory");exit(1);}
		/*File printing*/
		fprintf(recv_stats,"\nResults From Channel Tester.(Receive)\n");
		fprintf(recv_stats,"Iteration              :\t%8i.\n",loop);
		fprintf(recv_stats,"For Socket             :\t%8i.\n",rx_sock);
		fprintf(recv_stats,"Data Size              :\t%8i.\n",size);
		fprintf(recv_stats,"Packets Recv           :\t%8i.\n",rec);
		fprintf(time_analysis,"%%Timining Analysis Report For MATLAB processing!\n\n");
		fprintf(time_analysis,"vector%i=[\n",loop);
		temp=Array[0].INTERARRIVAL_TIME;
		fprintf(time_analysis,"%7i,%8li,%12li;\n",Array[0].ID,Array[0].LATENCY,(Array[0].INTERARRIVAL_TIME-temp));
		for(i=1;i<NUM_OF_PACKETS;i++){
			if(Array[i].ID!=0x00 && Array[i].INTERARRIVAL_TIME!=0 && Array[i].LATENCY!=0){
				temp=Array[i-1].INTERARRIVAL_TIME;
				fprintf(time_analysis,"%7i,%8li,%12li;\n",Array[i].ID,Array[i].LATENCY,(Array[i].INTERARRIVAL_TIME-temp));
			}
		}

		fprintf(time_analysis,"];\n");
		/*Close files*/
		fclose(recv_stats);
		fclose(time_analysis);
		/*Console Printing*/
		printf("\nResults From Channel Tester.(Receive)\n");
		printf("Iteration              :\t%8i.\n",loop);
		printf("For Socket             :\t%8i.\n",rx_sock);
		printf("Data Size              :\t%8i.\n",size);
		printf("Packets Received       :\t%8i.\n",rec);
}

void tx_result(char **tx_path,int sent,int tx_sock,int loop, int delay, int size,long elapsed)
{
	/*Filename and path fix*/
	char loop_num[33];
	char packet_size[33];
	snprintf(loop_num, sizeof(loop_num), "%d", loop);
	snprintf(packet_size, sizeof(packet_size), "%d", size);
	strcpy(fname_send_stats,"./");
	strcat(fname_send_stats,*tx_path);
	strcat(fname_send_stats,"/stream_");
	strcat(fname_send_stats,loop_num);
	strcat(fname_send_stats,"_size_");
	strcat(fname_send_stats,packet_size);
	strcat(fname_send_stats,"_send_stats.txt");
	/*File open&descriptor*/
	send_stats		=fopen(fname_send_stats,"w");
	if(send_stats==NULL){perror("No File or Bad Directory");exit(1);}
	/*File Print*/
	fprintf(send_stats,"\nResults From Channel Tester.\n");
	fprintf(send_stats,"Iteration              :\t%8i.\n",loop);
	fprintf(send_stats,"For Socket             :\t%8i.\n",tx_sock);
	fprintf(send_stats,"Delay                  :\t%8i.\n",delay);
	fprintf(send_stats,"Data Size              :\t%8i.\n",size);
	fprintf(send_stats,"Time of Receiving (us) :\t%8li.\n",elapsed);
	fprintf(send_stats,"Packet Rate (PPS)      :\t%8.0f.\n",(float)(NUM_OF_PACKETS/(elapsed/1000000)));
	fprintf(send_stats,"Packets Sent           :\t%8i.\n",sent);
	/*Close file*/
	fclose(send_stats);
	/*Console print*/
	printf("\nResults From Channel Tester.(Transmit)\n");
	printf("Iteration              :\t%8i.\n",loop);
	printf("For Socket             :\t%8i.\n",tx_sock);
	printf("Delay                  :\t%8i.\n",delay);
	printf("Data Size              :\t%8i.\n",size);
	printf("Time of Receiving (us) :\t%8li.\n",elapsed);
	printf("Packet Rate (PPS)      :\t%8.0f.\n",(float)(NUM_OF_PACKETS/(elapsed/1000000)));
	printf("Packets Sent           :\t%8i.\n",sent);
	printf("\n");
}

void netinit_receiver(mynet *receiver){
	int ifindex_rec,j;
	//unsigned char	src_mac_rec[6];
	receiver->sock=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
	strncpy(receiver->ifr.ifr_name,(const char*)DEVICE2,IFNAMSIZ);
	if (ioctl(receiver->sock, SIOCGIFINDEX,&receiver->ifr)==-1) {perror("SIOCGIFINDEX");exit(1);}
	ifindex_rec=receiver->ifr.ifr_ifindex;
	if(ioctl(receiver->sock,SIOCGIFHWADDR,&receiver->ifr)==-1){perror("SIOCGIFHWADDR");exit(1);}
	if(ioctl(receiver->sock, SIOCGIFFLAGS, &receiver->ifr)==-1){perror("SIOCGIFFLAGS");exit(1);}
	receiver->ifr.ifr_flags |= IFF_PROMISC;
	if(ioctl(receiver->sock, SIOCGIFFLAGS, &receiver->ifr)==-1){perror("SIOCGIFFLAGS");exit(1);}
	receiver->socket_address.sll_family   = PF_PACKET;
	receiver->socket_address.sll_protocol = htons(ETH_P_IP);
	receiver->socket_address.sll_ifindex  = ifindex_rec;
	receiver->socket_address.sll_hatype   = ARPHRD_ETHER;
	receiver->socket_address.sll_pkttype  = PACKET_OTHERHOST;
	receiver->socket_address.sll_halen    = ETH_ALEN;
	receiver->socket_address.sll_addr[6]  = 0x00;
	receiver->socket_address.sll_addr[7]  = 0x00;
	for(j=0;j<6;j++){
		src_mac_rec[j]=receiver->ifr.ifr_hwaddr.sa_data[j];
		//src_mac_rec[j]=receiver->ifr.ifr_hwaddr.sa_data[j];
	}
	//memcpy(mynet.src_mac_rec,ETH_MAC_LEN);
#ifdef DEBUG
	printf("Socket for Receiver with ID: %i created.\n",receiver->sock);
	printf("Successfully got our MAC address (receive interface): %02X:%02X:%02X:%02X:%02X:%02X\n",
					src_mac_rec[0],src_mac_rec[1],src_mac_rec[2],
					src_mac_rec[3],src_mac_rec[4],src_mac_rec[5]);
#endif

}


#endif

////#include <stdlib.h>
////#include <limits.h>
////#include <stdio.h>
////#include <errno.h>

//int
//str_lmain(int argc, char *argv[])
//{
//	int base;
//	char *endptr, *str;
//	long val;

//	if (argc < 2) {
//		fprintf(stderr, "Usage: %s str [base]\n", argv[0]);
//		exit(EXIT_FAILURE);
//	}

//	str = argv[1];
//	base = (argc > 2) ? atoi(argv[2]) : 10;

//	errno = 0;    /* To distinguish success/failure after call */
//	val = strtol(str, &endptr, base);

//	/* Check for various possible errors */

//	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
//			|| (errno != 0 && val == 0)) {
//		perror("strtol");
//		exit(EXIT_FAILURE);
//	}

//	if (endptr == str) {
//		fprintf(stderr, "No digits were found\n");
//		exit(EXIT_FAILURE);
//	}

//	/* If we got here, strtol() successfully parsed a number */

//	printf("strtol() returned %ld\n", val);

//	if (*endptr != '\0')        /* Not necessarily an error... */
//		printf("Further characters after number: %s\n", endptr);

//	exit(EXIT_SUCCESS);
//}
