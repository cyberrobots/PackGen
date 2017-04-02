# PackGen
================
Email : cyberrobots[at]gmail.com
Feel free contanting me.


Code Description.
--------------------------------------------------
This is a MAC layer "train/burst" Packet generator which produces RAW packets with destination defined in dst_mac char vector. It keeps statistics for packets latency and inter arrival time between two packets,in order to collect data for Packet Delay Variation (PDV).
This Packet generator produces RAW packets with destination defined in dst_mac[] char vector. It keeps statistics for packets latency and inter arrival time between two packets in order to collect data for Jitter and Packet Delay Variation (PDV).

# Build
make

make clean

#Usage
0)        help                                                                                                                                                                                                                                                  
1)        f_name          Filename
2)        path            Relative
3)        num             Packets Number
4)        inter           Transmit interval (usec)
5)        devout          Transmit interface
6)        devin           Receive interface
7)        dstmac          Target's Rx MAC address
8)        srcmac          Target's Tx MAC address
9)        proto           Protocol (default: 0x0808)
10)       write           Write to file.


# Examples
sudo ./PacketGenerator devout eth0 devin eth1 dstmac 00:aa:bb:cc:aa:aa proto 0x0808 num 1000000 inter 100 write 1
sudo ./PacketGenerator f_name test1 devout eth0 devin eth1 dstmac 00:aa:bb:cc:aa:aa proto 0x0808 num 100 inter 2 write 1
