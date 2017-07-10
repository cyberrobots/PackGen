/*
 * functions.c
 *
 *  Created on	: Jul 4, 2013
 *  Finished on	: Feb 2017
 *  Author		: Sotiris Lyras
 *  Version		: v05
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
	memset(gen->f_name,0,PGEN_MAX_PATHNAME_LEN);
	memset(gen->srcmac,0x00		,PGEN_ETH_MAC_LEN);
	memset(gen->dstmac,0x00		,PGEN_ETH_MAC_LEN);
	
	memset(gen->p_srcmac,0x00	,PGEN_ETH_MAC_LEN);
	memset(gen->p_dstmac,0x00	,PGEN_ETH_MAC_LEN);
	
	memset(gen->proto,0x08,PGEN_ETH_PROTO_LEN);

	gen->tx_interval 	= 0;
	gen->packetsNum  	= ULONG_MAX;
	gen->packetSize		= PGEN_ETH_FRAME;
	gen->tx_sock		= -1;
	gen->rx_sock		= -1;
	gen->WriteRxData	= 0;
	
	return gen;
}

void packet_gen_destroy(packgen_t *p)
{
	if(!p){
		return;
	}

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

	packet_gen_destroy(_packet);
	
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
IMPORT(filename_import)
IMPORT(write_output)

static pgen_variable_t pgen_var_table[] =
{
	{"help",		show_help			,""								},
	{"f_name",		filename_import		,"Filename"						},	
	{"path",		path_import			,"Relative"						},
	{"num",			numOfPackets_import	,"Packets Number"				},
	{"inter",		txInterval_import	,"Transmit interval (usec)"		},
	{"devout",		deviceOut_import	,"Transmit interface"			},
	{"devin",		deviceIn_import		,"Receive interface"			},
	{"dstmac",		deviceDstMac_import	,"Target's Rx MAC address"		},
	{"srcmac",		deviceSrcMac_import	,"Target's Tx MAC address"		},
	{"proto",		protocol_import		,"Protocol (default: 0x0808)"	},
	{"write",		write_output		,"Write to file."				},
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
	
	packet_gen_destroy(p);
	
	exit(0);
}	

void write_output(char* var,packgen_t* p)
{
	p->WriteRxData = (atoi(var)>0?1:0);
}

void path_import(char* var,packgen_t* p)
{
	if(var && p){
		strncpy(p->path,var,(strlen(var)<(PGEN_MAX_PATHNAME_LEN-1))?strlen(var):(PGEN_MAX_PATHNAME_LEN-1));
	}
	
	PP("Path [%s]",p->path);
}

void filename_import(char* var,packgen_t* p)
{
	if(var && p){
		strncpy(p->f_name,var,(strlen(var)<(PGEN_MAX_PATHNAME_LEN-1))?strlen(var):(PGEN_MAX_PATHNAME_LEN-1));
	}
	
	PP("FileName [%s]",p->f_name);
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
	
	PP("Num of packets [%lu]",p->packetsNum);
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
	
	PP("tx_interval [%lu]",p->tx_interval);
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
	
	/*Grab ingerface's MAC address.*/
	if(ioctl(s,SIOCGIFHWADDR,&ifr)==-1){
		perror("SIOCGIFHWADDR");
		goto failure;
	}

	if(direction == IF_OUT)
	{
		/////* Get settings */
		//int set = fcntl(s,F_GETFL,0);

		///* Set Settings plus NON Blocking flag */
		////O_DIRECT|O_SYNC
		////if(fcntl(s, F_SETFL, set | O_NONBLOCK)<0){

		//int err = fcntl(s, F_SETFL,set|O_NONBLOCK );
		//if(err<0){
		//	perror("fcntl");
		//	goto failure;
		//}
		
	}
	else
	{
		///* Get settings */
		int set = fcntl(s,F_GETFL,0);

		/* Set Settings plus NON Blocking flag */
		//O_DIRECT|O_SYNC
		//if(fcntl(s, F_SETFL, set | O_NONBLOCK)<0){

		int err = fcntl(s, F_SETFL,set|O_NONBLOCK );
		if(err<0){
			perror("fcntl");
			goto failure;
		}
		
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

	memset(&s_addr,0,sizeof(s_addr));	
	s_addr.sll_family 	= PF_PACKET;
	s_addr.sll_hatype 	= ARPHRD_ETHER;
	s_addr.sll_halen  	= ETH_ALEN;
	s_addr.sll_ifindex 	= ifindex;
	s_addr.sll_pkttype 	= PACKET_OTHERHOST;
	s_addr.sll_protocol	= htons(ETH_P_ALL);		//must check this variables
	if(direction == IF_IN){
		memcpy(s_addr.sll_addr,p->dstmac,6);
	}else{
		memcpy(s_addr.sll_addr,p->srcmac,6);
	}
	s_addr.sll_addr[6]	= 0x00;
	s_addr.sll_addr[7]	= 0x00;

    /* Bind socket to the interface. */
    bind(s,(const struct sockaddr *)&s_addr,sizeof(struct sockaddr_ll));
	
	
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
	cpu_set_t 		cpuset;
	
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

	memset(&cpuset,0,sizeof(cpuset));

	///*Pthread init*/
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
	
	P_INFO("---Packet Generator Start---");
	
	//CPU_ZERO(&cpuset);
	//CPU_SET(0, &cpuset);
	//pthread_setaffinity_np(packgen_rx_thread, sizeof(cpu_set_t), &cpuset);
	/*Fire up thread*/
	pthread_create(&packgen_rx_thread,&attr,PackGen_Rx_Thread,(void*)p);
	
	//CPU_ZERO(&cpuset);
	//CPU_SET(2, &cpuset);
	//pthread_setaffinity_np(packgen_tx_thread, sizeof(cpu_set_t), &cpuset);
	/*Fire up thread*/
	pthread_create(&packgen_tx_thread,&attr,PackGen_Tx_Thread,(void*)p);
	
	/*Wait to finish the send.*/
	pthread_join(packgen_rx_thread,(void**)&PackGenStatusRx);
	pthread_join(packgen_tx_thread,(void**)&PackGenStatusTx);
	
	pthread_attr_destroy(&attr);
	
	P_INFO("---Packet Generator Finish---");
	
	return P_SUCCESS;

failure:
	return P_FAILURE;
}

void packet_gen_udelay(unsigned long microns)
{
	if(!microns){
		//perror("Bad Microns Value...");
		return;
	}
	
	if(unlikely(microns >= 1000000))
	{
		/* Don't really care about accuracy here */
		unsigned int sleepTime = (microns / 1000000);
		
		do
		{
			sleepTime = sleep(sleepTime);

		}while(sleepTime!=0);

	}
	else
	{

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
}


void packet_gen_write_rx_result(packgen_t*p,unsigned long rx_num,packge_time_stats_t* stats,uint16_t size)
{
#define APPEND(m,...) fprintf(time_analysis,m"\n",##__VA_ARGS__)	
	/* Output Files 		*/
	char fname_time_stats[ 3 * PGEN_MAX_PATHNAME_LEN];
	FILE*				time_analysis 	= NULL;
	unsigned long i = 0, temp = 0;
	
	if(!stats || !p){
		return;
	}
	
	memset(fname_time_stats,0, 2 * PGEN_MAX_PATHNAME_LEN);

	if(strcmp(p->path,"./")==0)
	{
		if(strlen(p->f_name)!=0){
			snprintf(fname_time_stats, 2 * PGEN_MAX_PATHNAME_LEN,"./%s/PackGen_%d_stats_%s.mat",p->path,size,p->f_name);
		}else{
			snprintf(fname_time_stats, 2 * PGEN_MAX_PATHNAME_LEN,"./%s/PackGen_%d_stats.mat",p->path,size);
		}
	}else{
		if(strlen(p->f_name)!=0){
			snprintf(fname_time_stats, 2 * PGEN_MAX_PATHNAME_LEN,"%sPackGen_%d_stats_%s.mat",p->path,size,p->f_name);
		}else{
			snprintf(fname_time_stats, 2 * PGEN_MAX_PATHNAME_LEN,"%sPackGen_%d_stats.mat",p->path,size);
		}
	}
	
	time_analysis = fopen(fname_time_stats,"w");
	if(!time_analysis){
		perror("No File or Bad Directory");
		goto failure;
	}
	
	/*File printing*/
	APPEND("%%Results From Channel Tester.(Receive)\n");
	APPEND("%%Packet's Generator Configuration\n");
	APPEND("%%Results From Channel Tester.(Receive)");
	APPEND("%%Write to file          :%s.",p->WriteRxData==1?"Yes":"No");
	APPEND("%%Frame Size             :%u.",p->packetSize);
	APPEND("%%Packets Received       :%lu.",rx_num);
	APPEND("%%Packets Sent           :%lu.",p->packetsNum);
	APPEND("%%Tx interval            :%lu.",p->tx_interval);
	APPEND("%%Protocol               :0x%2.2X%2.2X.",p->proto[0],p->proto[1]);
	APPEND("%%Tx dev                 :%s.",p->tx_dev);
	APPEND("%%Tx dev MAC             :"MAC_ADDR_S".",MAC_ADDR_V(p->p_srcmac));
	APPEND("%%Rx dev                 :%s.",p->rx_dev);
	APPEND("%%Rx dev MAC             :"MAC_ADDR_S".",MAC_ADDR_V(p->p_dstmac));
	APPEND("%%Target's Rx dev MAC    :"MAC_ADDR_S".",MAC_ADDR_V(p->dstmac));
	APPEND("%%Target's Tx dev MAC    :"MAC_ADDR_S".",MAC_ADDR_V(p->srcmac));
	APPEND("\n");
	APPEND("%%Timining Analysis Report For MATLAB processing!");
	APPEND("\n");
	APPEND("%%Frame's ID | Travel Time (usec) | Interarrival Time (usec)");
	APPEND("\n");
	
	temp = stats[0].INTERARRIVAL_TIME;
	APPEND("vector=[\n");
	APPEND("%lu,%lu,%lu;",stats[i].ID,(unsigned long)0,(unsigned long)0);
	for(i = 1; i < rx_num; i++)
	{
		{
			temp = stats[i-1].INTERARRIVAL_TIME;
			APPEND("%lu,%lu,%lu;",stats[i].ID,stats[i].LATENCY,(stats[i].INTERARRIVAL_TIME-temp));
			packet_gen_udelay(100);
		}
	}
	
	APPEND("];\n");

	/*Close files*/
	fclose(time_analysis);
	time_analysis = NULL;
	
	/*Console Printing*/
	P_INFO("\nResults From Channel Tester.(Receive)");
	P_INFO("Data Size              :\t%u.",size);
	P_INFO("Packets Received       :\t%lu.",rx_num);
	
failure:
	return;
	
}
